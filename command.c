#include "khash.h"
#include "kdq.h"
#include <stdio.h>
#include "kexpr.h"
#include "command.h"
//#include "stack.h"
#include "api.h"

int ke_command_import(sml_t* sml, int itok) {
	sml_dec_top(sml);
	return itok;
}

int ke_command_if(sml_t* sml, int itok) {
	// i != 0 =? true, continue the code
	// i == 0 => false, jmp to end
	int i = sml_pop_int(sml);
	return i ? itok : sml_get_ijmp(sml);
}

//def(sum, _a, _b)    //  stack sum _a _b
//a = exe(sum, 2, 3)  // stack sum 2   3
// [sum]  top-5    1
// [2]    top-4    2
// [3]    top-3    3
// [sum]  top-2    4
// [_a]   top-1    5
// [_b]   top      6    top == 6
// 6 == 3
// 5 == 2
int ke_command_def(sml_t* sml, int itok) {
	token_t * tokp = sml_get_tokp(sml);

	token_t **stack = sml_get_stack(sml);
	int n = sml_get_args(sml);
	int top = sml_get_top(sml);
    if (sml_get_assigned(sml)) {
		for (int i = 1; i < n; i++) {
			token_t * def_token = stack[top - i];          //   6   5
			token_t * exe_token = stack[sml, top - i - (n-1)]; //   3   2
			ke_set_val(sml, def_token, exe_token);
		}
		top = top - n - (n-1);
		sml_set_top(sml,top);
		return itok;
    } else {
		sml_set_assigned(sml, 1);
		// remove param from stack, dont need it
		top -= n;
		sml_set_top(sml, top);
		// jmp after the enddef
		return sml_get_ijmp(sml);
    }
}

int ke_command_exe(sml_t* sml, int itok) {
	kdq_push(int, sml->callstack, itok);
	return sml_get_ijmp(sml);
}


int ke_command_for(sml_t* sml, int itok) {
	// field min, max inc
	token_t **stack = sml_get_stack(sml);
	int rtop = sml_get_top(sml);
	int n = sml_get_args(sml);
	int top = rtop - n;

	token_t *var = stack[top]; // copy of the real variable into the stack
	if (!sml_get_assigned(sml)) {
	
		token_t *min = stack[top + 1];
		sml_set_assigned(sml, 1);
		var->ijmp = sml_get_ijmp(sml);
		var->r = min->r;
		var->i = min->i;
		pushfor(sml, itok);
	}
	else {
		if (var->r >= stack[top + 2]->r) {
			sml_set_assigned(sml, 0);
			popfor(sml);
			sml_set_top(sml, top);
			return var->ijmp;
		}
		else {
			if (n == 4) {
				var->r += stack[top + 3]->r;
				var->i += stack[top + 3]->i;
			}
			else {
				var->r += 1;
				++var->i;
			}
		}
	}
	sml_set_top(sml, top);
	return itok;
}

int ke_command_print_nonl(sml_t* sml, int itok) {
	token_t * tokp = sml_get_tokp(sml);
    int ntmp = sml_get_args(sml);
    int n = ntmp;
    --n;
    sml_set_args(sml,n);
    token_t *p = sml_pop_token(sml);
    if (n) {
        ke_command_print_nonl(sml, itok);
    }
    ke_print_one(sml,p);
	sml_set_args(sml, ntmp);
	return itok;
}

int ke_command_print(sml_t* sml, int itok) {
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);

    ke_command_print_nonl(sml, itok);
    printf("\n");

	top -= n;
	sml_set_top(sml, top);
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
	if (!sml_get_assigned(sml)) {
		sml_set_assigned(sml,1);
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
	sml->localtop -= sml_get_ijmp(sml);
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
