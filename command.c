#include "khash.h"
#include "kdq.h"
#include <stdio.h>
#include "kexpr.h"
#include "command.h"
#include "stack.h"

int ke_command_import(sml_t* sml, ke1_t * tokp, int top, int * itokp) {
	return top - 1;
}

int ke_command_if(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
	*itokp = sml->stack[--top]->i ? *itokp : tokp->ijmp;
    return top;
}

int ke_command_def(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
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
		   ke_set_val(sml, q, v);
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

int ke_command_exe(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
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


int ke_command_for(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
	// field min, max inc
	ke1_t **stack = sml->stack;
	int n = tokp->n_args;
	int top_m1 = top - n;
	if (!tokp->assigned) {
		ke1_t *p = stack[top_m1]; // copy of the real variable into the stack
		ke1_t *min = stack[top_m1 + 1];
		tokp->assigned = 1;
		tokp->obj.tokp = p;
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
	return top_m1;
}

int ke_command_print_nonl(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
	ke1_t **stack = sml->stack;
    int ntmp = tokp->n_args;
    int n = tokp->n_args;
    ke1_t *p;
    --n;
    tokp->n_args = n;
    p = stack[--top];
    if (n) {
        top = ke_command_print_nonl(sml, tokp, top, itokp);
    }
    ke_print_one(sml,p);
    tokp->n_args = ntmp;
    return top;
}

int ke_command_print(sml_t* sml, struct ke1_s* tokp, int top, int * itokp) {
    top = ke_command_print_nonl(sml, tokp, top, itokp);
    printf("\n");
    return top;
}

int ke_command_val_else(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_end(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
	return itok;
}

int  ke_command_val_brk(sml_t* sml, struct ke1_s* tokp, int top, int itok) {

	int ifor = popfor(sml);
	ke1_t *efor = ke_get_tokidx(sml,ifor);
	efor->assigned = 0;
	return tokp->ijmp;
}


int  ke_command_val_for(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
	if (!tokp->assigned) {
		tokp->assigned = 1;
		pushfor(sml,itok);
	}
	return itok;
}

int  ke_command_val_cnt(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_next(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
	return tokp->ijmp;
}

int  ke_command_val_rtn(sml_t* sml, struct ke1_s* tokp, int top, int itok) {
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
