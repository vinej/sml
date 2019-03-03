#ifndef API_H_INCLUDED
#define API_H_INCLUDED

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex.h>
#include <plplot/plplot.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "utf8.h"
#include "gdate.h"
#include <setjmp.h>

#if defined(_MSC_VER) || defined(_WIN32)
#include <Windows.h>
#endif

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
#define KEV_FNC  7
#define KEV_REC  8
#define KEV_VEC_INT  9
#define KEV_DEF  10
#define KEV_IMAGE 11
#define KEV_FILE 12
#define KEV_BUFFER 13
#define KEV_DATE 14
#define KEV_PTR   15
#define KEV_FPOS  16

#define KEV_REAL_S "REAL"
#define KEV_INT_S "INT"
#define KEV_STR_S "STR"
#define KEV_VEC_S  "VEC"
#define KEV_MAT_S  "MAT"
#define KEV_COMPLEX_S "COMPLEX"
#define KEV_CMD_S  "CMD"
#define KEV_FNC_S  "FNC"
#define KEV_REC_S  "REC"
#define KEV_VEC_INT_S  "VEC_INT"
#define KEV_DEF_S  "DEF"
#define KEV_IMAGE_S "IMAGE"
#define KEV_FILE_S "FILE"
#define KEV_BUFFER_S "BUFFER"
#define KEV_DATE_S "DATE"
#define KEV_PTR_S  "PTR"
#define KEV_FPOS_S  "FPOS"

extern char * kev_to_str[17];

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

#include "kdq.h"
#include "khash.h"

struct token_s;
struct kexpr_s;
struct sml_s;

typedef struct token_s* ke1_p;
typedef int(*cmdp)(struct sml_s * sml, int);
typedef void(*fncp)(struct sml_s *);
typedef int(*vcmdp)(struct sml_s * sml, int);
typedef int(__cdecl *dllke_hash_add_t)(struct sml_s *, fncp, char *);
typedef struct token_s *(__cdecl *dllke_get_out_t)(struct sml_s *);

KHASH_MAP_INIT_STR(0, cmdp)
KHASH_MAP_INIT_STR(1, vcmdp)
KHASH_MAP_INIT_STR(2, int)
KHASH_MAP_INIT_INT(3, int)
KHASH_MAP_INIT_STR(5, fncp)
KHASH_MAP_INIT_STR(6, int)

#pragma warning( push )
#pragma warning( disable : 273)
#pragma warning( disable : 4018)
#pragma warning( disable : 4334)
#pragma warning( disable : 4627)
#pragma warning( disable : 4267)
KDQ_INIT(int)
#pragma warning( pop )

struct sml_s;
typedef struct sml_s {
	struct token_s **stack;        // stack for the evaluation of the program
	double *rstack;                    // stack for the evaluation of the program
	struct token_s *out;           // pointer to the array of out parameter to put into the stack
	struct kexpr_s *kexpr;
	struct token_s ** fields;		// array of all global fields of the program to exectue
	struct token_s ** tokens;     // array of pointers of all program tokens
	int top;
	struct token_s * tokp;
	int out_qte;
	int tok_idx;
	int val;
	jmp_buf env_buffer;
	// GLOBAL VARIABLE USED BY ALL FUNCTIONS
	int field_qte;				// number of global fields  
	int mem_count;				// current count of memory allocation
								// parser variables
	int isNextDefName;			// flag to indicate that the next token is the def name
	char currentDefName[100];	// current name of the current function
	int sourceCodeLine;			// use to keep the line number to show better error trapping
	int isFirstToken;			// use to remove separators at the beginning of the program
	int isLastTokenNop;			// use to manage the command seperator, the rule is to have one separator between each comand
	char lastErrorMessage[256]; // 
	struct token_s * recp[100];
	int rec_qte;        // number of global fields  
	khash_t(5) *hfunction;
	khash_t(6) *hname;
#if defined(_MSC_VER) || defined(_WIN32)
	HMODULE libhandle[64];
	int libhandle_qte;      // number of global fields
#endif
	kdq_t(int) *callstack;
	kdq_t(int) *hiforcommand;
	kdq_t(int) *hinextcommand;
	khash_t(3) *hidefcommand;
	khash_t(0) *hcommand;
	khash_t(1) *hvcommand;
	khash_t(3) *hdefcommand;
	kstack_t *harg;
	int g_forstack[20];
	int g_fortop;
	int lastDef;
	dllke_hash_add_t dllke_hash_add;
	dllke_get_out_t dllke_get_out;
} sml_t;

struct kexpr_s;
typedef struct kexpr_s kexpr_t;
typedef int * reclistt;

struct token_s;
typedef struct token_s {
	int64_t i;
	double r;
	union {    //                                                                                                 
		void(*builtin)(struct token_s *a, struct token_s *b, struct token_s *out); // execution function
		double(*real_func1)(double);
		double(*real_func2)(double, double);
		int(*defprop)(sml_t* sml, struct token_s* prop, int top);
		void(*deffunc)(sml_t* sml);
		int(*defcmd)(sml_t* sml, int itok);
		int(*defvcmd)(sml_t* sml, int itok);
		struct token_s * recp;
	} f;
	union {  //     
		void * ptr;
		//struct token_s * tokp;
		char *s;
		char* image;
		char ** ms;
		fpos_t * fpos;
		FILE *file;
		void *buffer;
		GDate_t * date;
		gsl_vector_int * vector_int;
		gsl_vector * vector;
		gsl_matrix * matrix;
		gsl_complex tcomplex;
		reclistt reclist;
		PLGraphicsIn * plgrin;
		PLPointer * plptr;
		PLTRANSFORM_callback pltrcb;
		PLMAPFORM_callback plmpcb;
		PLDEFINED_callback pldefcb;
		PLLABEL_FUNC_callback pllblcb;
		PLFILL_callback plfillcb;
	} obj;
	char *name; // variable name or function name                                                                 
	int32_t ijmp;
	int32_t sourceLine; // fast jmp  
	uint16_t ttype;
	uint16_t vtype;
	int16_t ifield;
	int8_t icmd;
	int8_t assigned;
	int8_t propget;
	int8_t propset;
	int8_t tofree;
	int8_t islocal;
	int8_t op;
	int8_t n_args;
	int8_t realToken;
	int8_t isfor;
} token_t;


struct kexpr_s {
	int n;
	token_t *e;
};

token_t * ke_get_out(sml_t *sml);
void ke_hash_add(sml_t *sml, fncp key, char * name);
void ke_inc_memory(sml_t *sml);
void ke_dec_memory(sml_t *sml);
void * ke_calloc_memory(sml_t *sml, size_t i, size_t x);
void * ke_malloc_memory(sml_t *sml, size_t i);
void ke_free_memory(sml_t *sml, void * m);

#define __GLOBAL "g_"
#define __GLOBAL_SEP "_"
#define __GLOBAL_DSEP "__"

#define sml_get_top(sml) sml->top
#define sml_dec_top(sml) --sml->top
#define sml_set_top(sml,top) sml->top=top
#define sml_get_stack(sml) sml->stack
#define sml_get_tokp(sml) sml->tokp
#define sml_get_args(sml) sml->tokp->n_args
#define sml_set_args(sml,a) sml->tokp->n_args = a
#define sml_get_ttype(sml) sml->tokp->ttype
#define sml_get_vtype(sml) sml->tokp->vtype
#define sml_get_ijmp(sml) sml->tokp->ijmp
#define sml_get_assigned(sml) sml->tokp->assigned
#define sml_set_assigned(sml,a) sml->tokp->assigned = a

#define sml_pop_token(sml) sml->stack[--sml->top]
#define sml_pop_int(sml) sml->stack[--sml->top]->i
#define sml_pop_real(sml) sml->stack[--sml->top]->r
#define sml_pop_ptr(sml) sml->stack[--sml->top]->obj.ptr
#define sml_pop_file(sml) sml->stack[--sml->top]->obj.file
#define sml_pop_fpos(sml) sml->stack[--sml->top]->obj.fpos
#define sml_pop_str(sml) sml->stack[--sml->top]->obj.s
#define sml_pop_date(sml) sml->stack[--sml->top]->obj.date
#define sml_pop_complex(sml) sml->stack[--sml->top]->obj.tcomplex
#define sml_pop_complex_adr(sml) &sml->stack[--sml->top]->obj.tcomplex
#define sml_pop_day(sml) sml->stack[--sml->top]->i
#define sml_pop_month(sml) sml->stack[--sml->top]->i
#define sml_pop_year(sml) sml->stack[--sml->top]->i
#define sml_pop_matrix(sml) sml->stack[--sml->top]->obj.matrix
#define sml_pop_vector(sml) sml->stack[--sml->top]->obj.vector
#define sml_pop_vector_int(sml) sml->stack[--sml->top]->obj.vector_int

#define sml_peek_token(sml,i) sml->stack[i]
#define sml_peek_int(sml,ii) sml->stack[ii]->i
#define sml_peek_real(sml,i) sml->stack[i]->r
#define sml_peek_ptr(sml,i) sml->stack[i]->obj.ptr
#define sml_peek_file(sml,i) sml->stack[i]->obj.file
#define sml_peek_fpos(sml,i) sml->stack[i]->obj.fpos
#define sml_peek_str(sml,i) sml->stack[i]->obj.s
#define sml_peek_date(sml,i) sml->stack[i]->obj.date
#define sml_peek_complex(sml,i) sml->stack[i]->obj.tcomplex
#define sml_peek_complex_adr(sml,i) &sml->stack[i]->obj.tcomplex
#define sml_peek_day(sml,i) (GDateDay)sml->stack[i]->i
#define sml_peek_month(sml,i) (GDateMonth)sml->stack[i]->i
#define sml_peek_year(sml,i) (GDateYear)sml->stack[i]->i
#define sml_peek_matrix(sml,i) sml->stack[i]->obj.matrix
#define sml_peek_vector(sml,i) sml->stack[i]->obj.vector
#define sml_peek_vector_int(sml,i) sml->stack[i]->obj.vector_int

#define sml_get_complex_adr  &out->obj.tcomplex

#define sml_get_type(t) t->vtype
#define sml_get_str(t) t->obj.s
#define sml_get_real(t) t->r
#define sml_get_int(t) t->i
#define sml_set_int(t,ii) t->i = ii
#define sml_get_ptr(t) t->obj.ptr
#define sml_get_file(t) t->obj.file
#define sml_get_buffer(t) t->obj.buffer
#define sml_get_fpos(t) t->obj.fpos
#define sml_set_fpos(t,fposp) t->obj.fpos = fposp
#define sml_set_ptr_null(t) t->obj.ptr = NULL
#define sml_get_len(t) t->i

#define sml_adr_str(t) &t->obj.s
#define sml_adr_ptr(t) &t->obj

#define sml_push_buffer(sml, b, ii) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.buffer = b; \
	out->i = ii; \
	out->r == (double)ii; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_BUFFER; \
	out->assigned = 1;

#define sml_push_file(sml, file) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.file = file; \
	out->i = 1; \
	out->r == 1.0; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_FILE; \
	out->assigned = 1;

#define sml_push_new_complex(sml) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_COMPLEX; \
	out->assigned = 1;

#define sml_push_complex(sml, z) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.tcomplex = z; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_COMPLEX; \
	out->assigned = 1;

#define sml_push_real(sml, r) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->r = r; \
	out->i = (int64_t)r; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_REAL; \
	out->assigned = 1;

#define sml_push_date(sml, dt) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.date = dt; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_DATE; \
	out->assigned = 1;

#define  sml_push_int(sml, ii) token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->i = ii; \
	out->r = (double)ii; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_INT; \
	out->assigned = 1;

#define  sml_push_ptr(sml, ptr) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.ptr = ptr; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_PTR; \
	out->assigned = 1; 

#define  sml_push_str(sml, str) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.s = str; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_STR; \
	out->assigned = 1; 

#define  sml_push_matrix(sml, m) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.matrix = m; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_MAT; \
	out->assigned = 1; 

#define  sml_push_vector(sml, v) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.vector = v; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_VEC; \
	out->assigned = 1; 

#define  sml_push_vector_int(sml, v) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.vector_int = v; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_VEC_INT; \
	out->assigned = 1; 

#define  sml_push_image(sml, image) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.image = image; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_IMAGE; \
	out->assigned = 1; 

#define  sml_push_fpos(sml, fposp) \
	token_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.fpos = fposp; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_FPOS; \
	out->assigned = 1; 

#define sml_out_date out->obj.date

#define sml_free_ptr(sml,ptr) ke_free_memory(sml,ptr); ke_dec_memory(sml); ptr = NULL

#define sml_new_ptr(sml,size) ke_calloc_memory(sml, size,1)

#define sml_mem_inc ke_inc_memory

#define sml_save_str(sml, tokp, tmp) \
	if (tokp->vtype == KEV_STR && tokp->obj.s) { \
		ke_free_memory(sml, tokp->obj.s); \
	} \
	tokp->obj.s = tmp; \
	tokp->i = 0, tokp->r = 0, tokp->assigned = 1; \
	tokp->vtype = KEV_STR;

#endif

#ifdef _DEBUG
#define sml_assert_args(sml,i,fnc) if (sml->tokp->n_args != i) { \
		printf("ASSERT ERROR : Invalid number of parameters, expected <%d> got <%d> at line <%d> for function <%s>", i, sml->tokp->n_args, sml->tokp->sourceLine,fnc);	\
		longjmp(sml->env_buffer, 1); \
	} 
#else
#define sml_assert_args(sml,i,s)
#endif 

#ifdef _DEBUG
#define sml_assert_args_min(sml,i,fnc) if (sml->tokp->n_args < i) { \
		printf("ASSERT ERROR : Invalid number of parameters, expected at least <%d> got <%d> at line <%d> for function <%s>", i, sml->tokp->n_args, sml->tokp->sourceLine,fnc);	\
		longjmp(sml->env_buffer, 1); \
	} 
#else
#define sml_assert_args(sml,i,s)
#endif 

#ifdef _DEBUG
#define sml_assert_type(sml,i,t,fnc) \
	if (sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->vtype != t) { \
		printf("ASSERT ERROR : Invalid type for parameter #%d, expected <%s> got <%s> at line <%d> for function <%s>", i, kev_to_str[t], kev_to_str[sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->vtype], sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_type(sml,i,t,s)
#endif 

#ifdef _DEBUG
#define sml_assert_type_int_or_real(sml,i,fnc) \
	if (sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->vtype != KEV_INT && sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->vtype != KEV_REAL) { \
		printf("ASSERT ERROR : Invalid type for parameter #%d, expected <INT OR REAL> got <%s> at line <%d> for function <%s>", i, kev_to_str[sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->vtype], sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_type_int_or_real(sml,i,s)
#endif 

#ifdef _DEBUG
#define sml_assert_str_in(sml,i,search,fnc) \
	if (!strstr(search, sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->obj.s)) { \
		printf("ASSERT ERROR : Invalid parameter value for parameter #%d, at line <%d> for function <%s>", i, sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_str_in(sml,i,f,s)
#endif 

#ifdef _DEBUG
#define sml_assert_str(sml,i,fnc) \
	if (!strlen(sml->stack[sml->top - 1 - (sml->tokp->n_args - i)]->obj.s)) { \
		printf("ASSERT ERROR : Invalid parameter value (empty) for parameter #%d, at line <%d> for function <%s>", i, sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_str(sml,i,fnc)
#endif 

#ifdef _DEBUG
#define sml_assert_size(sml,size,i,fnc) \
	if (size <= 0) { \
		printf("ASSERT ERROR : Invalid parameter value (<=0) for parameter #%d, at line <%d> for function <%s>", i, sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_size(sml,size,i,fnc)
#endif 

#ifdef _DEBUG
#define sml_assert_gz(sml,size,fnc) \
	if (size < 0) { \
		printf("ASSERT ERROR : Invalid parameters (size validation < 0) at line <%d> for function <%s>", sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_gz(sml,size,fnc)
#endif 

#ifdef _DEBUG
#define sml_assert_ptr(sml,p,i,fnc) \
	if (p == NULL) { \
		printf("ASSERT ERROR : Invalid parameter value (==NULL) for parameter #%d, at line <%d> for function <%s>", i, sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_ptr(sml,p,i,fnc)
#endif 

#ifdef _DEBUG
#define sml_assert_range(sml,i,v,from,to,fnc) \
	if (v < from || v > to) { \
		printf("ASSERT ERROR : Invalid parameter value <%d> expected (%d - %d) for parameter #%d, at line <%d> for function <%s>", v, from, to, i, sml->tokp->sourceLine,fnc); \
		longjmp(sml->env_buffer, 1); \
	}
#else
#define sml_assert_ptr(sml,p,i,fnc)
#endif 


#define sml_fatal_error(sml,c,fnc) \
		printf("ASSERT ERROR : Fatal error <%s> at line <%d> for function <%s>", c, sml->tokp->sourceLine , fnc); \
		longjmp(sml->env_buffer, 1);
