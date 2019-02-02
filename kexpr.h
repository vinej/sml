#ifndef KEXPR_H
#define KEXPR_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex.h>
#include <plplot/plplot.h>
#include <stdint.h>
#include <stdio.h>

#define LOCAL_VAR '_'

// Parse errors
#define KEE_UNQU    0x01 // unmatched quotation marks
#define KEE_UNLP    0x02 // unmatched left parentheses
#define KEE_UNRP    0x04 // unmatched right parentheses
#define KEE_UNOP    0x08 // unknown operators
#define KEE_FUNC    0x10 // wrong function syntax
#define KEE_ARG     0x20
#define KEE_NUM     0x40 // fail to parse a number
//#define KEE_END     0x80 // fail to parse a number

// Evaluation errors
#define KEE_UNFUNC  0x40 // undefined function
#define KEE_UNVAR   0x80 // unassigned variable

// Return type
#define KEV_REAL 1
#define KEV_INT  2
#define KEV_STR  3
#define KEV_VEC  4
#define KEV_MAT  5
#define KEV_COMPLEX 6
#define KEV_CMD  6
#define KEV_FNC  7
#define KEV_REC  8
#define KEV_VEC_INT  9
#define KEV_DEF  10
#define KEV_IMAGE 11
#define KEV_FILE 12
#define KEV_BUFFER 13

#define KEO_NULL  0
#define KEO_POS   1
#define KEO_NEG   2
#define KEO_BNOT  3
#define KEO_LNOT  4
#define KEO_POW   5
#define KEO_MUL   6
#define KEO_DIV   7
#define KEO_IDIV  8
#define KEO_MOD   9
#define KEO_ADD  10
#define KEO_SUB  11
#define KEO_LSH  12
#define KEO_RSH  13
#define KEO_LT   14
#define KEO_LE   15
#define KEO_GT   16
#define KEO_GE   17
#define KEO_EQ   18
#define KEO_NE   19
#define KEO_BAND 20
#define KEO_BXOR 21
#define KEO_BOR  22
#define KEO_LAND 23
#define KEO_LOR  24
#define KEO_LET  25
#define KEO_NOP  26

#define KEO_MAX  27

#define KET_NULL  0
#define KET_CMD   1
#define KET_VCMD  2
#define KET_OP    3
#define KET_FUNC  4
#define KET_PROP  5
#define KET_CONST 6
#define KET_VAL   7
#define KET_VNAME 8
#define KET_REC   9
#define KET_FILE  10

#define KEF_NULL  0
#define KEF_REAL  1
#define KEF_LET   2
#define KEF_GET   3

struct kexpr_s;
typedef struct kexpr_s kexpr_t;
typedef int * reclistt;

struct ke1_s;
typedef struct ke1_s {
	double r;
	int64_t i;
	int32_t ijmp; // fast jmp  
	uint32_t ttype:16, vtype:10, assigned:1, icmd:4, realToken:1, propget:1, propset:1, tofree:1, futur:1, islocal:1; // ttype: token type; vtype: value type  
	int32_t op:8, n_args:8, ifield:16; // op: operator, n_args: number of arguments                               
	int32_t sourceLine; // fast jmp  
	union {    //                                                                                                 
		void (*builtin)(struct ke1_s *a, struct ke1_s *b); // execution function
		double (*real_func1)(double);
		double (*real_func2)(double, double);
		int (*defprop)(struct ke1_s* stack, struct ke1_s* prop, int top);
		int (*deffunc)(struct ke1_s* stack, struct ke1_s* tokp, int top);
		int(*defcmd)(struct kexpr_s*, struct ke1_s*, struct ke1_s*, int, int *);
		int (*defvcmd)(kexpr_t *ke, struct ke1_s*, int i);
		struct ke1_s * recp;
	} f;
	union {  //               
		struct ke1_s * tokp;
		char *s;
		char* image;
		char ** ms;
		FILE *file;
		void *buffer;
		gsl_vector_int * vector_int;
		gsl_vector * vector;
		gsl_matrix * matrix;
	    gsl_complex complex;
		reclistt reclist;
		PLGraphicsIn * plgrin;
		PLPointer * plptr;
		PLTRANSFORM_callback pltrcb;
		PLMAPFORM_callback plmpcb;
		PLDEFINED_callback pldefcb;
		PLLABEL_FUNC_callback pllblcb;
		PLFILL_callback plfillcb;
	} obj;
	int line;
	char *name; // variable name or function name                                                                 
} ke1_t;

struct kexpr_s {
	int n;
	ke1_t *e;
};

typedef struct ke1_t* ke1_p;
typedef int(*cmdp)(struct kexpr_s*, struct ke1_s*, struct ke1_s*, int, int *);
typedef int(*fncp)(struct ke1_s* s, struct ke1_s* t, int);
typedef int(*vcmdp)(struct kexpr_s*, struct ke1_s*, int);
char *ke_mystr(char *src, size_t n);
void ke_hash_add(fncp key, char * name);

// parse an expression and return errors in $err
kexpr_t *ke_parse(char *_s, int *err);

// free memory allocated during parsing
void ke_destroy(kexpr_t *ke);
// evaluate expression; return error code; final value is returned via pointers
int ke_eval(kexpr_t *ke, int64_t *_i, double *_r, char **_s, int *ret_type);
// print the expression in Reverse Polish notation (RPN)
void ke_print(kexpr_t *ke);
void ke_print_error_one(kexpr_t *ke, char * fnc, ke1_t *u, int i);
void ke_print_one(ke1_t * u);
void ke_fill_hash();

void ke_set_real(ke1_t *e, double x);
void ke_set_null_vector_int(int ival);
void ke_set_null_vector(int ival);
void ke_set_vector(ke1_t *e, gsl_vector * p);
void ke_set_null_matrix(int ival);
void ke_set_matrix(ke1_t *e, gsl_matrix * p);
void ke_set_complex(ke1_t *e, gsl_complex p);
void ke_set_str(ke1_t *e, char *x);
void ke_set_str_direct(int ival, char *x);
//void ke_set_str_direct2(kexpr_t *ke, char *var, char *x);
void ke_set_int(ke1_t *e, int64_t y);
void ke_inc_memory();
void ke_dec_memory();
void ke_init_memory_count();
void * ke_calloc_memory(size_t i, size_t x);
void * ke_malloc_memory(size_t i);
void ke_free_memory(void *);
void ke_free_tokens();
int ke_fill_list(kexpr_t *ke);
void ke_free_val();
void ke_free(kexpr_t *ke);
void ke_free_hash();
void ke_push_stack(ke1_t *e, int *top);
void ke_set_local_val(kexpr_t *ke, int i, ke1_t *p, ke1_t *q);
void ke_set_val(ke1_t* e, ke1_t *q);
void ke_set_val_index(int i, ke1_t *q);
ke1_t * ke_get_tok();
ke1_t * ke_get_tokidx(int idx);
ke1_t* ke_get_val_index(int i);


#endif

