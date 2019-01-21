#include "stdafx.h"
#include "khash.h"
#include "kdq.h"
#include <stdio.h>
#include "kexpr.h"
#include "command.h"
#include "stack.h"

// KHASH DEFINITION
const int KH_CMD = 0;
const int KH_STR = 1;
const int KH_VCMD = 2;
const int KH_ICMD = 3;
const int KH_IDEF = 4;

KDQ_INIT(int)
KHASH_MAP_INIT_STR(KH_CMD, cmdp)
KHASH_MAP_INIT_STR(KH_VCMD, vcmdp)
KHASH_MAP_INIT_STR(KH_STR, int)
KHASH_MAP_INIT_INT(KH_IDEF, int)
kdq_t(int) *callstack;
kdq_t(int) *hiforcommand;
kdq_t(int) *hinextcommand;
khash_t(KH_IDEF) *hidefcommand;
khash_t(KH_CMD) *hcommand;
khash_t(KH_VCMD) *hvcommand;
khash_t(KH_IDEF) *hdefcommand;
stack_t *harg;

extern ke1_t ** g_gbl_fields;

// g_forstack max 20 level of for
int g_forstack[20]; int g_fortop = 0;

int peekfor() 
{	
	//printf("\npeek %d\n", g_fortop);
	return g_forstack[g_fortop-1];
}

int popfor() { 
	//printf("\npop %d\n", g_fortop);
	//--g_fortop;
	return g_forstack[--g_fortop]; 
}

void pushfor(int val) { 
	//printf("\npush %d\n", g_fortop);
	g_forstack[g_fortop++] = val;
	//++g_fortop;
}

// g_forstack max 20 level of for
int lastDef = -1;

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

int ke_set_ijmp(kexpr_t *kexpr, ke1_t ** tokens) {
	ke1_t *tokp = NULL;
	int n = kexpr->n;
	int lastForCmd = 0;
	for (int itok = 0; itok < n; ++itok) {
		tokp = tokens[itok];
		if (tokp->ttype == KET_CMD || tokp->ttype == KET_VCMD) {
			switch (tokp->icmd) {
			case CMD_IBRK:
				// use for find the for token to set assigned to 0
				tokp->i = lastForCmd;
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
				break;
			case CMD_ICNT:
				if (!tokp->ijmp) {
					if (g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <continue> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = peekfor();
				}
				break;
			case CMD_IEXE:
				if (!tokp->ijmp) {
					int idefname = ifind_backward_defname(tokens, itok, tokp);
					if (idefname != -1) {
						ke1_t *def_name = tokens[idefname];
						khint_t iter = kh_get(KH_IDEF, hidefcommand, def_name->ifield);
						if (kh_end(hidefcommand) == iter) {
							printf("SML: ERROR: Command <def> not found for token <exe>(%s) at line <%d>\n", def_name->name, tokp->sourceLine);
							return -1;
						}
						tokp->ijmp = (int)kh_val(hidefcommand, iter);
					}
				}
				break;
			case CMD_IDEF:
				lastDef = itok;
				int absent;
				ke1_t *def_name = tokens[itok - tokp->n_args];
				def_name->vtype = KEV_DEF;
				khint_t iter = kh_put(KH_IDEF, hidefcommand, def_name->ifield, &absent);
				kh_val(hidefcommand, iter) = ((itok) - tokp->n_args - 1);

				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_rtn(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
				break;
			case CMD_IIF:
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_else_or_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
				break;
			case CMD_IELSE:
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
				break;
			case CMD_IEND:
				break;
			case CMD_IRTN:
				if (lastDef == -1) {
					printf("SML: ERROR: Command <def> not found for token <enddef> at line <%d>\n", tokp->sourceLine);
					return -1;
				}
				if (!tokp->ijmp) {
					tokp->ijmp = lastDef;
				}
				break;
			case CMD_IFOR:
				if (tokp->ttype == KET_CMD) {
					lastForCmd = 5;
					pushfor(itok - 5);
				}
				else
				{
					lastForCmd = 1;
					pushfor(itok - 1);
				}
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
				break;
			case CMD_INEXT:
				if (!tokp->ijmp) {
					if (g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <next> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = popfor();
				}
			default:
				break;
			}
		}
	}
	return 0;
}

int ke_command_if(kexpr_t *kexpr, ke1_t * tokp, ke1_t *stack, int top, int * itokp) {
    ke1_t *p;
    p = NULL;
	*itokp = (&stack[--top])->i ? *itokp : tokp->ijmp;
    return top;
}


int ke_command_def(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int  * itokp) {
	ke1_t * p;
	ke1_t * q;
	ke1_t * v;

    // get the def name
    p = (ke1_t *)&stack[top - tokp->n_args];
    if (tokp->assigned) {
        // execute it
        int n = tokp->n_args;
        // set parameter value from the g_stack
        for (int j = 0; j < n-1; ++j) {
           q = &stack[top-n+j+1];
		   v = stack_pop(harg);
		   ke_set_val(g_gbl_fields[q->ifield], v);
		   ke_free_memory(v);
        }
        return top - tokp->n_args;
    } else {
        // save position for futur use and find the end of the function
        // remove arguments from g_stack
		tokp->assigned = 1;
        int n = tokp->n_args - 1;
        while(n) {
            q = &stack[--top];
            --n;
        }
		*itokp = tokp->ijmp;
        --top;
        return top;
    }
}

int ke_command_exe(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int * itokp) {
	ke1_t *p, *q;
	int narg = tokp->n_args;
	while (narg > 1) {
		q = &stack[--top];
		ke1_t * k = ke_malloc_memory(sizeof(ke1_t));
		memcpy(k, q, sizeof(ke1_t));
		stack_push(harg, k);
		narg--;
	}
	p = &stack[top - 1];
	kdq_push(int, callstack, *itokp);
	*itokp = tokp->ijmp;
	--top;
	return top;
}


int ke_command_for(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int * itokp) {
	// field min, max inc
	int n = tokp->n_args;
	ke1_t *p = &stack[top - n]; // copy of the real variable into the stack

    if (!tokp->assigned) {
		ke1_t *min = &stack[top - n + 1];
		tokp->assigned = 1;
		tokp->obj.tokp = ke_get_tokidx(*itokp - n);
		struct ke1_s * t = tokp->obj.tokp;
		t->r = min->r;
		t->i = (int64_t)t->r;
		pushfor(*itokp);
    } else {
		ke1_t *max = &stack[top - n + 2];
		if (p->r >= max->r) {
	        tokp->assigned = 0;
			popfor();
			*itokp = tokp->ijmp;
		} else {
			ke1_t *inc = &stack[top - n + 3];
			tokp->obj.tokp->r += inc->r;
			tokp->obj.tokp->i = (int64_t)tokp->obj.tokp->r;
		}
	}
	return top - n;
}

int ke_command_print_nonl(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int * itokp) {
    int ntmp = tokp->n_args;
    int n = tokp->n_args;
    ke1_t *p;
    --n;
    tokp->n_args = n;
    p = &stack[--top];
    if (n) {
        top = ke_command_print_nonl(kexpr,tokp,stack,top,itokp);
    }
    ke_print_one(p);
    tokp->n_args = ntmp;
    return top;
}

int ke_command_print(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int * itokp) {
    top = ke_command_print_nonl(kexpr,tokp,stack,top,itokp);
    printf("\n");
    return top;
}

int ke_command_val_else(kexpr_t *ke, ke1_t *e, int itok) {
	return e->ijmp;
}

int  ke_command_val_end(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return itok;
}

int  ke_command_val_brk(kexpr_t *kexpr, ke1_t *tokp, int itok) {

	int ifor = popfor();
	// tokp->i is to determine is the for was a single for or the one with 4 parameters
	ke1_t *efor = ke_get_tokidx(ifor);
	efor->assigned = 0;
	return tokp->ijmp;
}


int  ke_command_val_for(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	if (!tokp->assigned) {
		tokp->assigned = 1;
		pushfor(itok);
	}
	return itok;
}

int  ke_command_val_cnt(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_next(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_rtn(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	return *kdq_pop(int, callstack);
}

void ke_command_hash_add(cmdp key, char * name) {
	int absent;
	khint_t iter = kh_put(KH_CMD, hcommand, name, &absent);
	kh_val(hcommand, iter) = key;
}

void ke_vcommand_hash_add(vcmdp key, char * name) {
	int absent;
	khint_t iter = kh_put(KH_VCMD, hvcommand, name, &absent);
	kh_val(hvcommand, iter) = key;
}

void ke_command_hash() {
    callstack = kdq_init(int); ke_inc_memory();
	harg = stack_create(16);
    hcommand = kh_init(KH_CMD); ke_inc_memory();
	hvcommand = kh_init(KH_VCMD); ke_inc_memory();
	hidefcommand = kh_init(KH_IDEF); ke_inc_memory();
	hiforcommand = kdq_init(int); ke_inc_memory();
	hinextcommand = kdq_init(int); ke_inc_memory();

    ke_command_hash_add((cmdp)&ke_command_if, CMD_IF);
    ke_command_hash_add((cmdp)&ke_command_print, CMD_PRINT);
    ke_command_hash_add((cmdp)&ke_command_print_nonl, CMD_PRINTNOLN);
    ke_command_hash_add((cmdp)&ke_command_def, CMD_DEF);
    ke_command_hash_add((cmdp)&ke_command_exe, CMD_EXE);
    ke_command_hash_add((cmdp)&ke_command_for, CMD_FOR);

	ke_vcommand_hash_add((vcmdp)&ke_command_val_for, CMD_FOR);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_else, CMD_ELSE);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_end, CMD_END);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_brk, CMD_BRK);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_cnt, CMD_CNT);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_next, CMD_NEXT);
	ke_vcommand_hash_add((vcmdp)&ke_command_val_rtn, CMD_RTN);
}


cmdp ke_command(char * name) {
    khint_t iter = kh_get(KH_CMD, hcommand, name);
    if (kh_end(hcommand) != iter) {
        return (cmdp)kh_val(hcommand, iter);
    } else {
        return NULL;
    }
}

int ke_command_icmd(char * name) {
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

vcmdp ke_command_val(char * name) {
	khint_t iter = kh_get(KH_VCMD, hvcommand, name);
	if (kh_end(hvcommand) != iter) {
		return (vcmdp)kh_val(hvcommand, iter);
	}
	else {
		return NULL;
	}
}

void ke_command_destroy() {
	kdq_destroy(int, callstack); ke_dec_memory();
	kdq_destroy(int, hiforcommand); ke_dec_memory();
	kdq_destroy(int, hinextcommand); ke_dec_memory();
	stack_destroy(harg); ke_dec_memory();

    kh_destroy(KH_CMD, hcommand); ke_dec_memory();
	kh_destroy(KH_VCMD, hvcommand); ke_dec_memory();
	kh_destroy(KH_IDEF, hidefcommand); ke_dec_memory();
}

int ke_command_get_rtn() {
    return *kdq_pop(int, callstack);
}
