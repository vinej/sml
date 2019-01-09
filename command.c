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


// g_forstack max 20 level of for
int g_forstack[20]; int g_fortop = 0;
int inline peekfor() {	return g_forstack[g_fortop-1];}
int inline popfor() { return g_forstack[--g_fortop]; }
void inline pushfor(int val) { g_forstack[g_fortop++] = val;  }

// g_forstack max 20 level of for
int lastDef = -1;

int ifind_forward_rtn(kexpr_t *kexpr, int starti) {
	int i = -1;
	for (int i = starti + 1; i < kexpr->n; i++) {
		ke1_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IRTN) {
			return i;
		}
	}
	printf("->*** ERROR, %s:%d:%d", "not found for iforward", CMD_IRTN, starti);
	return -1;
}

int ifind_forward_next(kexpr_t *kexpr, int starti) {
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
	printf("->*** ERROR: %s|%d", "not found next", starti);
	return -1;
}

int ifind_forward_end(kexpr_t *kexpr, int starti) {
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
	printf("->*** ERROR: %s|%d", "not found: end", starti);
	return -1;
}

int ifind_else_or_end(kexpr_t *kexpr, int starti) {
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
    printf("->*** ERROR: %s|%d", "not find: end|else", starti);
    return -1;
}

void ke_set_ijmp(kexpr_t *kexpr, ke1_t ** tokens) {
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
					tokp->ijmp = ifind_forward_next(kexpr, itok);
				}
				break;
			case CMD_ICNT:
				if (!tokp->ijmp) {
					tokp->ijmp = peekfor();
				}
				break;
			case CMD_IEXE:
				if (!tokp->ijmp) {
					khint_t iter = kh_get(KH_IDEF, hidefcommand, tokp->ifield);
					tokp->ijmp = (int)kh_val(hidefcommand, iter);
				}
				break;
			case CMD_IDEF:
				lastDef = itok;
				int absent;
				khint_t iter = kh_put(KH_IDEF, hidefcommand, tokp->ifield, &absent);
				kh_val(hidefcommand, iter) = ((itok) - tokp->n_args - 1);

				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_rtn(kexpr, itok);
				}
				break;
			case CMD_IIF:
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_else_or_end(kexpr, itok);
				}
				break;
			case CMD_IELSE:
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_end(kexpr, itok);
				}
				break;
			case CMD_IEND:
				break;
			case CMD_IRTN:
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
					tokp->ijmp = ifind_forward_next(kexpr, itok);
				}
				break;
			case CMD_INEXT:
				if (!tokp->ijmp) {
					tokp->ijmp = popfor();
				}
			default:
				break;
			}
		}
	}
}

int ke_command_if(kexpr_t *kexpr, ke1_t * tokp, ke1_t *stack, int top, int * itokp) {
    ke1_t *p, *q;
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
    int absent;

    if (tokp->assigned) {
        // execute it
        int n = tokp->n_args;
        // set parameter value from the g_stack
        for (int j = 0; j < n-1; ++j) {
           q = &stack[top-n+j+1];
		   v = stack_pop(harg);
           ke_set_local_val(kexpr,*itokp + 1, q, v);
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
	//if (!tokp->ijmp) {
	//	khint_t iter = kh_get(KH_IDEF, hidefcommand, tokp->ifield);
	//	tokp->ijmp = (int)kh_val(hidefcommand, iter);
	//}
	*itokp = tokp->ijmp;
	--top;
	return top;
}


int ke_command_for(kexpr_t *kexpr, ke1_t *tokp, ke1_t *stack, int top, int * itokp) {
	int n = tokp->n_args;
	ke1_t *p = &stack[top - n];
    if (!tokp->assigned) {
		ke1_t *inc = &stack[top - n + 1];
		ke1_t *max = &stack[top - n + 2];
		ke1_t *min = &stack[top - n + 3];
        p->vtype = KEV_INT;
        p->i = min->i;
        tokp->assigned = 1;
		tokp->i = inc->i;
		tokp->imax = max->i;
		if (p->islocal){
            ke_set_local_val(kexpr, *itokp, p, p);
        } else {
			tokp->gsl.tokp = ke_get_tokidx(*itokp - n);
			tokp->gsl.tokp->r = p->i;
			tokp->gsl.tokp->i = p->i;
		}
    } else {
		if (p->i >= tokp->imax) {
	        tokp->assigned = 0;
			*itokp = tokp->ijmp;
		} else {
			p->i += tokp->i;
			if (p->islocal){
				ke_set_local_val(kexpr, *itokp, p, p);
			} else {
				tokp->gsl.tokp->i += tokp->i;
				tokp->gsl.tokp->r = (double)tokp->gsl.tokp->i;
			}
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
	ke1_t *efor = ke_get_tokidx(ifor + tokp->i);
	efor->assigned = 0;
	return tokp->ijmp;
}


int  ke_command_val_for(kexpr_t *kexpr, ke1_t *tokp, int itok) {
	if (!tokp->assigned) {
		tokp->assigned = 1;
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
