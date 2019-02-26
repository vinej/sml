#include "khash.h"
#include "kdq.h"
#include <stdio.h>
#include "kexpr.h"
#include "command.h"
#include "stack.h"
#include "api.h"

int ke_command_import(sml_t* sml, int itok) {
	--sml->top;
	return itok;
}

int ke_command_if(sml_t* sml, int itok) {
	int i = sml_pop_int(sml);
	return i ? itok : sml_get_ijmp(sml);
}

int ke_command_def(sml_t* sml, int itok) {
	token_t * p;
	token_t * q;
	token_t * v;
	token_t **stack = sml_get_stack(sml);
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);
    p = (token_t *)stack[top - tokp->n_args];
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
        top = top - tokp->n_args;
		sml_set_top(sml, top);
		return itok;
    } else {
        // save position for futur use and find the end of the function
        // remove arguments from g_stack
		tokp->assigned = 1;
        int n = tokp->n_args - 1;
        while(n) {
            q = stack[--top];
            --n;
        }
		sml_set_top(sml, top);
		--top;
		sml_set_top(sml, top);
		return tokp->ijmp;
    }
}

int ke_command_exe(sml_t* sml, int itok) {
	token_t *p, *q;
	token_t **stack = sml_get_stack(sml);
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	int narg = tokp->n_args;
	while (narg > 1) {
		q = stack[--top];
		token_t * k = ke_malloc_memory(sml, sizeof(token_t));
		memcpy(k, q, sizeof(token_t));
		stack_push(sml->harg, k);
		narg--;
	}
	p = stack[--top];
	kdq_push(int, sml->callstack, itok);
	sml_set_top(sml,top);
	return tokp->ijmp;
}


int ke_command_for(sml_t* sml, int itok) {
	// field min, max inc
	token_t **stack = sml_get_stack(sml);
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

	int n = tokp->n_args;
	int top_m1 = top - n;
	if (!tokp->assigned) {
		token_t *p = stack[top_m1]; // copy of the real variable into the stack
		token_t *min = stack[top_m1 + 1];
		tokp->assigned = 1;
		tokp->obj.tokp = p;
		struct token_s * t = tokp->obj.tokp;
		t->r = min->r;
		t->i = (int64_t)t->r;
		pushfor(sml,itok);
	}
	else {
		token_t *p = tokp->obj.tokp; // copy of the real variable into the stack
		if (p->r >= stack[top_m1 + 2]->r) {
			tokp->assigned = 0;
			popfor(sml);
			return tokp->ijmp;
		}
		else {
			p->r += stack[top_m1 + 3]->r;
			p->i += stack[top_m1 + 3]->i;
		}
	}
	top = top_m1;
	sml_set_top(sml,top);
	return itok;
}

int ke_command_print_nonl(sml_t* sml, int itok) {
	token_t **stack = sml_get_stack(sml);
	int top = sml_get_top(sml);
	token_t * tokp = sml_get_tokp(sml);

    int ntmp = tokp->n_args;
    int n = tokp->n_args;
    token_t *p;
    --n;
    tokp->n_args = n;
    p = stack[--top];
    if (n) {
        top = ke_command_print_nonl(sml, itok);
    }
    ke_print_one(sml,p);
    tokp->n_args = ntmp;
    sml_set_top(sml,top);
	return itok;
}

int ke_command_print(sml_t* sml, int itok) {
    ke_command_print_nonl(sml, itok);
    printf("\n");
    return itok;
}

int ke_command_val_else(sml_t* sml, int itok) {
	return sml_get_ijmp(sml);
}

int  ke_command_val_end(sml_t* sml, int itok) {
	return itok;
}

int  ke_command_val_brk(sml_t* sml, int itok) {

	int ifor = popfor(sml);
	token_t *efor = ke_get_tokidx(sml,ifor);
	efor->assigned = 0;
	return sml_get_ijmp(sml);
}


int  ke_command_val_for(sml_t* sml, int itok) {
	token_t * tokp = sml_get_tokp(sml);
	if (!tokp->assigned) {
		tokp->assigned = 1;
		pushfor(sml,itok);
	}
	return itok;
}

int  ke_command_val_cnt(sml_t* sml, int itok) {
	return sml_get_ijmp(sml);
}

int  ke_command_val_next(sml_t* sml, int itok) {
	return sml_get_ijmp(sml);
}

int  ke_command_val_rtn(sml_t* sml, int itok) {
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
