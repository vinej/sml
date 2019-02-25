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

#include "api.h"


#ifdef _DEBUG
void ke_validate_parameter_qte(sml_t* sml, token_t *p, int nb_param, char * function_name);
void ke_validate_parameter_vtype(sml_t* sml, token_t * p, int vtype, char * param_name, char * function_name);
void ke_validate_parameter_ttype(sml_t* sml, token_t * p, int ttype, char * function_name);
void ke_validate_parameter_not_null(sml_t* sml, token_t * p, void * ptr, char * param_name, char * function_name);
void ke_validate_parameter_int_gt_zero(sml_t* sml, token_t * p, char * param_name, char * function_name);
#endif // _DEBUG


void ke_set_jump(sml_t *sml, int info);
sml_t * ke_create_sml();
void ke_free_sml(sml_t *sml);
void ke_init_memory_count(sml_t *sml);
char *ke_mystrndup(sml_t *sml, char *src, size_t n);
utf8 *ke_mystr(sml_t *sml, utf8 *src, size_t n);
token_t * ke_get_tok(sml_t *sml);
token_t * ke_get_tokidx(sml_t *sml, int idx);
fncp ke_function(sml_t *sml, char * name);
typedef int(*DLLPROC)();
void ke_import(sml_t *sml, char * s);
void ke_load_dll(sml_t *sml, char * p);
void ke_free_val(sml_t *sml);
void ke_free_tokens(sml_t *sml);
int ke_fill_list(sml_t *sml, kexpr_t *ke);
void ke_set_int(sml_t* sml, token_t *tokp, int64_t y);
void ke_set_real(sml_t* sml, token_t *tokp, double x);
void ke_set_vector(sml_t * sml, token_t *tokp, gsl_vector * vecp);
void ke_set_vector_int(sml_t * sml, token_t *tokp, gsl_vector_int * vecp);
void ke_set_date(sml_t * sml, token_t *tokp, GDate_t * datep);
void ke_set_matrix(sml_t *sml, token_t *tokp, gsl_matrix * matp);
void ke_set_complex(sml_t *sml, token_t *tokp, gsl_complex complex);
static void ke_set_str_internal(sml_t* sml, token_t *tokp, char * tmp);
void ke_set_str(sml_t* sml, token_t *tokp, char *x);
void ke_set_str_direct(sml_t *sml, token_t * e, char *x);
void ke_set_record(sml_t *sml, token_t *source, token_t *dest);
void ke_set_image(sml_t *sml, token_t *source, token_t *dest);
void ke_set_file(sml_t *sml, token_t *source, token_t *dest);
void ke_set_buffer(sml_t *sml, token_t *source, token_t *dest);
void ke_print_one_stack(token_t * tokp);
void ke_print_one(sml_t *sml, token_t * tokp);
void ke_print_stack(sml_t* sml, token_t *tokp, int top);
void ke_print_range(kexpr_t *kexpr, int starti, int endi);
void ke_print_error_one(sml_t * sml, kexpr_t *kexpr, char * name, token_t * e, int itok);
void ke_print(sml_t* sml, kexpr_t *kexpr);
void ke_fill_hash(sml_t *sml);
token_t* ke_get_val_index(sml_t *sml, int i);
void ke_set_val_index(sml_t *sml, int i, token_t *tokp);
void ke_set_val(sml_t* sml, token_t* e, token_t *q);
void ke_push_stack(sml_t * sml, token_t * tokp, int *top);
void ke_free_dll(sml_t * sml);
void ke_destroy(sml_t *sml, kexpr_t *kexpr);
void ke_free_hash(sml_t *sml);
void ke_free(sml_t* sml, kexpr_t *kexpr);
int peekfor(sml_t* sml);
int popfor(sml_t* sml);
void pushfor(sml_t* sml, int val);

#endif

