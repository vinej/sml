#ifndef KEXPR_H
#define KEXPR_H
#pragma warning( disable : 991)

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_complex.h>
#include <plplot/plplot.h>
#include <stdint.h>
#include <stdio.h>
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
#define KEV_CMD  6
#define KEV_FNC  7
#define KEV_REC  8
#define KEV_VEC_INT  9
#define KEV_DEF  10
#define KEV_IMAGE 11
#define KEV_FILE 12
#define KEV_BUFFER 13
#define KEV_DATE 14
#define KEV_PTR   15


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

struct ke1_s;
struct kexpr_s;
struct sml_s;

typedef struct ke1_s* ke1_p;
typedef int(*cmdp)(struct sml_s * sml, struct ke1_s* tokp, int top, int *);
typedef void (*fncp)(struct sml_s *);
typedef int(*vcmdp)(struct sml_s * sml, struct ke1_s* tokp, int top, int);
typedef int(__cdecl *dllke_hash_add_t)(struct sml_s *, fncp, char *);
typedef struct ke1_s *(__cdecl *dllke_get_out_t)(struct sml_s *);

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
	struct ke1_s **stack;        // stack for the evaluation of the program
	struct ke1_s *out;           // pointer to the array of out parameter to put into the stack
	struct kexpr_s *kexpr;
	struct ke1_s ** fields;		// array of all global fields of the program to exectue
	struct ke1_s ** tokens;     // array of pointers of all program tokens
	int top;
	struct ke1_s * tokp;
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
	struct ke1_s * recp[100];
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




struct ke1_s;
typedef struct ke1_s {
	int64_t i;
	double r;
	union {    //                                                                                                 
		void(*builtin)(struct ke1_s *a, struct ke1_s *b, struct ke1_s *out); // execution function
		double(*real_func1)(double);
		double(*real_func2)(double, double);
		int(*defprop)(sml_t* sml, struct ke1_s* prop, int top);
		void (*deffunc)(sml_t* sml);
		int(*defcmd)(sml_t* sml, struct ke1_s* tokp, int top, int *);
		int(*defvcmd)(sml_t* sml, struct ke1_s* tokp, int top, int i);
		struct ke1_s * recp;
	} f;
	union {  //     
		void * ptr;
		struct ke1_s * tokp;
		char *s;
		char* image;
		char ** ms;
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
	int8_t futur;
} ke1_t;

struct kexpr_s {
	int n;
	ke1_t *e;
};

#define __GLOBAL "g_"
#define __GLOBAL_SEP "_"
#define __GLOBAL_DSEP "__"

#ifdef _DEBUG
void ke_validate_parameter_qte(sml_t* sml, ke1_t *p, int nb_param, char * function_name);
void ke_validate_parameter_vtype(sml_t* sml, ke1_t * p, int vtype, char * param_name, char * function_name);
void ke_validate_parameter_ttype(sml_t* sml, ke1_t * p, int ttype, char * function_name);
void ke_validate_parameter_not_null(sml_t* sml, ke1_t * p, void * ptr, char * param_name, char * function_name);
void ke_validate_parameter_int_gt_zero(sml_t* sml, ke1_t * p, char * param_name, char * function_name);
#endif // _DEBUG


void ke_set_jump(sml_t *sml, int info);
sml_t * ke_create_sml();
void ke_free_sml(sml_t *sml);
void ke_init_memory_count(sml_t *sml);
void ke_inc_memory(sml_t *sml);
void ke_dec_memory(sml_t *sml);
void * ke_calloc_memory(sml_t *sml, size_t i, size_t x);
void * ke_malloc_memory(sml_t *sml, size_t i);
void ke_free_memory(sml_t *sml, void * m);
char *ke_mystrndup(sml_t *sml, char *src, size_t n);
utf8 *ke_mystr(sml_t *sml, utf8 *src, size_t n);
ke1_t * ke_get_tok(sml_t *sml);
ke1_t * ke_get_tokidx(sml_t *sml, int idx);
fncp ke_function(sml_t *sml, char * name);
typedef int(*DLLPROC)();
void ke_import(sml_t *sml, char * s);
void ke_load_dll(sml_t *sml, char * p);
void ke_free_val(sml_t *sml);
void ke_free_tokens(sml_t *sml);
int ke_fill_list(sml_t *sml, kexpr_t *ke);
void ke_set_int(sml_t* sml, ke1_t *tokp, int64_t y);
void ke_set_real(sml_t* sml, ke1_t *tokp, double x);
void ke_set_vector(sml_t * sml, ke1_t *tokp, gsl_vector * vecp);
void ke_set_vector_int(sml_t * sml, ke1_t *tokp, gsl_vector_int * vecp);
void ke_set_date(sml_t * sml, ke1_t *tokp, GDate_t * datep);
void ke_set_matrix(sml_t *sml, ke1_t *tokp, gsl_matrix * matp);
void ke_set_complex(sml_t *sml, ke1_t *tokp, gsl_complex complex);
static void ke_set_str_internal(sml_t* sml, ke1_t *tokp, char * tmp);
void ke_set_str(sml_t* sml, ke1_t *tokp, char *x);
void ke_set_str_direct(sml_t *sml, ke1_t * e, char *x);
void ke_set_record(sml_t *sml, ke1_t *source, ke1_t *dest);
void ke_set_image(sml_t *sml, ke1_t *source, ke1_t *dest);
void ke_set_file(sml_t *sml, ke1_t *source, ke1_t *dest);
void ke_set_buffer(sml_t *sml, ke1_t *source, ke1_t *dest);
void ke_print_one_stack(ke1_t * tokp);
void ke_print_one(sml_t *sml, ke1_t * tokp);
void ke_print_stack(sml_t* sml, ke1_t *tokp, int top);
void ke_print_range(kexpr_t *kexpr, int starti, int endi);
void ke_print_error_one(sml_t * sml, kexpr_t *kexpr, char * name, ke1_t * e, int itok);
void ke_print(sml_t* sml, kexpr_t *kexpr);
void ke_hash_add(sml_t *sml, fncp key, char * name);
void ke_fill_hash(sml_t *sml);
ke1_t* ke_get_val_index(sml_t *sml, int i);
void ke_set_val_index(sml_t *sml, int i, ke1_t *tokp);
void ke_set_val(sml_t* sml, ke1_t* e, ke1_t *q);
inline ke1_t * ke_get_out(sml_t *sml);
void ke_push_stack(sml_t * sml, ke1_t * tokp, int *top);
void ke_free_dll(sml_t * sml);
void ke_destroy(sml_t *sml, kexpr_t *kexpr);
void ke_free_hash(sml_t *sml);
void ke_free(sml_t* sml, kexpr_t *kexpr);
int peekfor(sml_t* sml);
int popfor(sml_t* sml);
void pushfor(sml_t* sml, int val);

#endif

