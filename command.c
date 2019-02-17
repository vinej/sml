#include "khash.h"
#include "kdq.h"
#include <stdio.h>
#include "kexpr.h"
#include "command.h"
#include "stack.h"

int peekfor(sml_t* sml) 
{	
	//printf("\npeek %d\n", g_fortop);
	return sml->g_forstack[sml->g_fortop-1];
}

int popfor(sml_t* sml) {
	//printf("\npop %d\n", g_fortop);
	//--g_fortop;
	return sml->g_forstack[--sml->g_fortop];
}

void pushfor(sml_t* sml,int val) { 
	//printf("\npush %d\n", g_fortop);
	sml->g_forstack[sml->g_fortop++] = val;
	//++g_fortop;
}

int ifind_backward_defname(ke1_t ** tokens, int starti, ke1_t * tokp) {
	for (int i = starti+1; i > -1; --i) {
		ke1_t *f = tokens[i];
		if (f->vtype == KEV_DEF) {
			return i;
		}
	}
	printf("SML: ERROR: Command <defname> not found at line <%d>\n", tokp->sourceLine);
	return -1;
}

int ifind_forward_rtn(kexpr_t *kexpr, int starti, ke1_t * tokp) {
	int i = -1;
	for (int i = starti + 1; i < kexpr->n; i++) {
		ke1_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IRTN) {
			return i;
		}
	}
	printf("SML: ERROR: Command <enddef> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_forward_next(kexpr_t *kexpr, int starti, ke1_t *tokp) {
	int qte_for = 0;
	for (int i = starti + 1; i < kexpr->n; i++) {
		ke1_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IFOR) {
			++qte_for;
		}
		else if (f->ttype == KET_VCMD) {
			if (qte_for && f->icmd == CMD_INEXT)
				--qte_for;
			else if (!qte_for && f->icmd == CMD_INEXT)
				return i;
		}
	}
	printf("SML: ERROR: Command <next> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_forward_end(kexpr_t *kexpr, int starti, ke1_t *tokp) {
	int qte_if = 0;
	for (int i = starti + 1; i < kexpr->n; i++) {
		ke1_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IIF) {
			++qte_if;
		} else if (f->ttype == KET_VCMD) {
			if (qte_if && f->icmd == CMD_IEND)
				--qte_if;
			else if (!qte_if && (f->icmd == CMD_IEND))
				return i;
		}
	}
	printf("SML: ERROR: Command <end> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_else_or_end(kexpr_t *kexpr, int starti, ke1_t *tokp) {
    int i = -1;
    int qte_if = 0;
    for(int i = starti + 1; i < kexpr->n; i++) {
        ke1_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IIF) {
			++qte_if;
		} else if  (f->ttype == KET_VCMD) {
			if (qte_if && f->icmd == CMD_IEND)
				--qte_if;
			else if (!qte_if && (f->icmd == CMD_IEND || f->icmd == CMD_IELSE))
				return i;
		}
    }
	printf("SML: ERROR: Command <end or else> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ke_set_ijmp(sml_t* sml,kexpr_t *kexpr, ke1_t ** tokens) {
	ke1_t *tokp = NULL;
	int n = kexpr->n;
	int lastForCmd = 0;
	int lastNop = 0;
	for (int itok = 0; itok < n; ++itok) {
		tokp = tokens[itok];
		if (tokp->ttype == KET_OP && tokp->op == KEO_NOP) {
			lastNop = itok;
		} else 	if (tokp->ttype == KET_CMD || tokp->ttype == KET_VCMD) {
			register int icmd = tokp->icmd;
			if (icmd == CMD_IBRK) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			} else if (icmd == CMD_ICNT) {
				if (!tokp->ijmp) {
					if (sml->g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <continue> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = peekfor(sml);
				}
			} else if (icmd == CMD_IEXE) {
				if (!tokp->ijmp) {
					int idefname = ifind_backward_defname(tokens, itok, tokp);
					if (idefname != -1) {
						ke1_t *def_name = tokens[idefname];
						khint_t iter = kh_get(3, sml->hidefcommand, def_name->ifield);
						if (kh_end(sml->hidefcommand) == iter) {
							printf("SML: ERROR: Command <def> not found for token <exe>(%s) at line <%d>\n", def_name->name, tokp->sourceLine);
							return -1;
						}
						tokp->ijmp = (int)kh_val(sml->hidefcommand, iter);
					}
				}
			} else if (icmd == CMD_IDEF) {
				sml->lastDef = itok;
				int absent;
				ke1_t *def_name = tokens[itok - tokp->n_args];
				def_name->vtype = KEV_DEF;
				khint_t iter = kh_put(3, sml->hidefcommand, def_name->ifield, &absent);
				kh_val(sml->hidefcommand, iter) = ((itok)-tokp->n_args - 1);

				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_rtn(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			} else if (icmd == CMD_IIF) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_else_or_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			} else if (icmd == CMD_IELSE) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			} else if (icmd == CMD_IRTN) {
				if (sml->lastDef == -1) {
					printf("SML: ERROR: Command <def> not found for token <enddef> at line <%d>\n", tokp->sourceLine);
					return -1;
				}
				if (!tokp->ijmp) {
					tokp->ijmp = sml->lastDef;
				}
			} else if (icmd == CMD_IFOR) {
				if (tokp->ttype == KET_CMD) {
					pushfor(sml, lastNop);
				}
				else
				{
					pushfor(sml, itok - 1);
				}
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			} else if (icmd == CMD_INEXT) {
				if (!tokp->ijmp) {
					if (sml->g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <next> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = popfor(sml);
				}
			}
			// nothing to do for CMD_IEND
		}
	}
	return 0;
}

int ke_command_import(sml_t* sml, kexpr_t *kexpr, ke1_t * tokp, int top, int * itokp) {
	return top - 1;
}

int ke_command_if(sml_t* sml, kexpr_t *kexpr, ke1_t * tokp, int top, int * itokp) {
	ke1_t **stack = sml->stack;
    ke1_t *p;
    p = NULL;
	*itokp = stack[--top]->i ? *itokp : tokp->ijmp;
    return top;
}

int ke_command_def(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int top, int  * itokp) {
	ke1_t * p;
	ke1_t * q;
	ke1_t * v;
	ke1_t **stack = sml->stack;
    // get the def name
    p = (ke1_t *)stack[top - tokp->n_args];
    if (tokp->assigned) {
        // execute it
        int n = tokp->n_args;
        // set parameter value from the g_stack
        for (int j = 0; j < n-1; ++j) {
           q = stack[top-n+j+1];
		   v = stack_pop(sml->harg);
		   ke_set_val(sml, sml->fields[q->ifield], v);
		   ke_free_memory(sml, v);
        }
        return top - tokp->n_args;
    } else {
        // save position for futur use and find the end of the function
        // remove arguments from g_stack
		tokp->assigned = 1;
        int n = tokp->n_args - 1;
        while(n) {
            q = stack[--top];
            --n;
        }
		*itokp = tokp->ijmp;
        --top;
        return top;
    }
}

int ke_command_exe(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int top, int * itokp) {
	ke1_t *p, *q;
	ke1_t **stack = sml->stack;
	int narg = tokp->n_args;
	while (narg > 1) {
		q = stack[--top];
		ke1_t * k = ke_malloc_memory(sml, sizeof(ke1_t));
		memcpy(k, q, sizeof(ke1_t));
		stack_push(sml->harg, k);
		narg--;
	}
	p = stack[--top];
	kdq_push(int, sml->callstack, *itokp);
	*itokp = tokp->ijmp;
	return top;
}


int ke_command_for(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int top, int * itokp) {
	// field min, max inc
	ke1_t **stack = sml->stack;
	int n = tokp->n_args;
	int top_m1 = top - n;
	if (!tokp->assigned) {
		ke1_t *p = stack[top_m1]; // copy of the real variable into the stack
		ke1_t *min = stack[top_m1 + 1];
		tokp->assigned = 1;
		tokp->obj.tokp = sml->fields[p->ifield];
		struct ke1_s * t = tokp->obj.tokp;
		t->r = min->r;
		t->i = (int64_t)t->r;
		pushfor(sml,*itokp);
	}
	else {
		ke1_t *p = tokp->obj.tokp; // copy of the real variable into the stack
		if (p->r >= stack[top_m1 + 2]->r) {
			tokp->assigned = 0;
			popfor(sml);
			*itokp = tokp->ijmp;
		}
		else {
			p->r += stack[top_m1 + 3]->r;
			p->i += stack[top_m1 + 3]->i;
		}
	}
	return top - n;
}

int ke_command_print_nonl(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int top, int * itokp) {
	ke1_t **stack = sml->stack;
    int ntmp = tokp->n_args;
    int n = tokp->n_args;
    ke1_t *p;
    --n;
    tokp->n_args = n;
    p = stack[--top];
    if (n) {
        top = ke_command_print_nonl(sml,kexpr,tokp,top,itokp);
    }
    ke_print_one(sml,p);
    tokp->n_args = ntmp;
    return top;
}

int ke_command_print(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int top, int * itokp) {
    top = ke_command_print_nonl(sml,kexpr,tokp,top,itokp);
    printf("\n");
    return top;
}

int ke_command_val_else(sml_t* sml, kexpr_t *ke, ke1_t *e, int itok) {
	return e->ijmp;
}

int  ke_command_val_end(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return itok;
}

int  ke_command_val_brk(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {

	int ifor = popfor(sml);
	ke1_t *efor = ke_get_tokidx(sml,ifor);
	efor->assigned = 0;
	return tokp->ijmp;
}


int  ke_command_val_for(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {
	if (!tokp->assigned) {
		tokp->assigned = 1;
		pushfor(sml,itok);
	}
	return itok;
}

int  ke_command_val_cnt(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_next(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_rtn(sml_t* sml, kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return *kdq_pop(int, sml->callstack);
}

void ke_command_hash_add(sml_t* sml, cmdp key, char * name) {
	int absent;
	khint_t iter = kh_put(0, sml->hcommand, name, &absent);
	kh_val(sml->hcommand, iter) = key;
}

void ke_vcommand_hash_add(sml_t* sml, vcmdp key, char * name) {
	int absent;
	khint_t iter = kh_put(1, sml->hvcommand, name, &absent);
	kh_val(sml->hvcommand, iter) = key;
}

void ke_command_hash(sml_t* sml) {
	sml->callstack = kdq_init(int); ke_inc_memory(sml);
	sml->harg = stack_create(16); ke_inc_memory(sml);
	sml->hcommand = kh_init(0); ke_inc_memory(sml);
	sml->hvcommand = kh_init(1); ke_inc_memory(sml);
	sml->hidefcommand = kh_init(3); ke_inc_memory(sml);
	sml->hiforcommand = kdq_init(int); ke_inc_memory(sml);
	sml->hinextcommand = kdq_init(int); ke_inc_memory(sml);

    ke_command_hash_add(sml, (cmdp)&ke_command_if, CMD_IF);
    ke_command_hash_add(sml, (cmdp)&ke_command_print, CMD_PRINT);
	//ke_command_hash_add((cmdp)&ke_command_printf, CMD_PRINTF);
	//ke_command_hash_add((cmdp)&ke_command_sprintf, CMD_SPRINTF);
	ke_command_hash_add(sml, (cmdp)&ke_command_import, CMD_IMPORT);
	ke_command_hash_add(sml, (cmdp)&ke_command_print_nonl, CMD_PRINTNOLN);
    ke_command_hash_add(sml, (cmdp)&ke_command_def, CMD_DEF);
    ke_command_hash_add(sml, (cmdp)&ke_command_exe, CMD_EXE);
    ke_command_hash_add(sml, (cmdp)&ke_command_for, CMD_FOR);

	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_for, CMD_FOR);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_else, CMD_ELSE);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_end, CMD_END);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_brk, CMD_BRK);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_cnt, CMD_CNT);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_next, CMD_NEXT);
	ke_vcommand_hash_add(sml, (vcmdp)&ke_command_val_rtn, CMD_RTN);
}


cmdp ke_command(sml_t* sml, char * name) {
    khint_t iter = kh_get(0, sml->hcommand, name);
    if (kh_end(sml->hcommand) != iter) {
        return (cmdp)kh_val(sml->hcommand, iter);
    } else {
        return NULL;
    }
}

int ke_command_icmd(sml_t* sml, char * name) {
	if (strcmp(name, CMD_FOR) == 0) {
		return CMD_IFOR;
	}
	else if (strcmp(name, CMD_NEXT) == 0) {
		return CMD_INEXT;
	}
	else if (strcmp(name, CMD_BRK) == 0) {
		return CMD_IBRK;
	}
	else if (strcmp(name, CMD_CNT) == 0) {
		return CMD_ICNT;
	}
	else if (strcmp(name, CMD_IF) == 0) {
		return CMD_IIF;
	}
	else if (strcmp(name, CMD_ELSE) == 0) {
		return CMD_IELSE;
	}
	else if (strcmp(name, CMD_END) == 0) {
		return CMD_IEND;
	}
	else if (strcmp(name, CMD_DEF) == 0) {
		return CMD_IDEF;
	}
	else if (strcmp(name, CMD_RTN) == 0) {
		return CMD_IRTN;
	}
	else if (strcmp(name, CMD_EXE) == 0) {
		return CMD_IEXE;
	}
	else {
		return -1;
		printf("Command not found : %s", name);
	}
}

vcmdp ke_command_val(sml_t* sml,char * name) {
	khint_t iter = kh_get(1, sml->hvcommand, name);
	if (kh_end(sml->hvcommand) != iter) {
		return (vcmdp)kh_val(sml->hvcommand, iter);
	}
	else {
		return NULL;
	}
}

void ke_command_destroy(sml_t* sml) {
	kdq_destroy(int, sml->callstack); ke_dec_memory(sml);
	kdq_destroy(int, sml->hiforcommand); ke_dec_memory(sml);
	kdq_destroy(int, sml->hinextcommand); ke_dec_memory(sml);
	stack_destroy(sml->harg); ke_dec_memory(sml);

    kh_destroy(0, sml->hcommand); ke_dec_memory(sml);
	kh_destroy(1, sml->hvcommand); ke_dec_memory(sml);
	kh_destroy(3, sml->hidefcommand); ke_dec_memory(sml);
}

int ke_command_get_rtn(sml_t* sml) {
    return *kdq_pop(int, sml->callstack);
}
