#include "stdafx.h"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <string.h>
#include "kexpr.h"
#include "complex.h"
#include "khash.h"

static int ke_complex_alloc(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    GSL_SET_COMPLEX(&p->obj.complex, (double)p->i, (double)q->i);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_set_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    GSL_SET_REAL(&p->obj.complex, (double)q->r);
    return top;
}

static int ke_complex_set_imag(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    GSL_SET_IMAG(&p->obj.complex, q->r);
    return top;
}

static int ke_complex_arg(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_complex_arg(p->obj.complex);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    return top;
}

static int ke_complex_abs(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_complex_abs(p->obj.complex);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    return top;
}

static int ke_complex_abs2(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_complex_abs2(p->obj.complex);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    return top;
}

static int ke_complex_logabs(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_complex_logabs(p->obj.complex);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    return top;
}

static int ke_complex_add(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_add(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_sub(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sub(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_mul(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_mul(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_div(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_div(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_add_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_add_real(p->obj.complex, q->r);
    return top;
}

static int ke_complex_sub_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sub_real(p->obj.complex, q->r);
    return top;
}

static int ke_complex_mul_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_mul_real(p->obj.complex, q->r);
    return top;
}

static int ke_complex_div_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_div_real(p->obj.complex, q->r);
    return top;
}

static int ke_complex_add_imag(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_add_imag(p->obj.complex, q->r);
    return top;
}

static int ke_complex_sub_imag(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sub_imag(p->obj.complex, q->r);
    return top;
}

static int ke_complex_mul_imag(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_mul_imag(p->obj.complex, q->r);
    return top;
}

static int ke_complex_div_imag(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_div_imag(p->obj.complex, q->r);
    return top;
}

static int ke_complex_conjugate(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_conjugate(p->obj.complex);
    return top;
}

static int ke_complex_inverse(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_inverse(p->obj.complex);
    return top;
}

static int ke_complex_negative(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_negative(p->obj.complex);
    return top;
}

static int ke_complex_sqrt(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sqrt(p->obj.complex);
    return top;
}

static int ke_complex_sqrt_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sqrt_real(p->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_pow(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_pow(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_pow_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_pow_real(p->obj.complex, q->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_exp(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_exp(p->obj.complex);
    return top;
}

static int ke_complex_log(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_log(p->obj.complex);
    return top;
}

static int ke_complex_log10(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_log10(p->obj.complex);
    return top;
}

static int ke_complex_log_b(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->obj.complex = gsl_complex_log_b(p->obj.complex, q->obj.complex);
    return top;
}

static int ke_complex_sin(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sin(p->obj.complex);
    return top;
}

static int ke_complex_cos(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_cos(p->obj.complex);
    return top;
}

static int ke_complex_tan(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_tan(p->obj.complex);
    return top;
}

static int ke_complex_sec(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sec(p->obj.complex);
    return top;
}

static int ke_complex_csc(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_csc(p->obj.complex);
    return top;
}

static int ke_complex_cot(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_cot(p->obj.complex);
    return top;
}

static int ke_complex_arcsin(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsin(p->obj.complex);
    return top;
}

static int ke_complex_arcsin_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsin_real(p->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccos(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccos(p->obj.complex);
    return top;
}

static int ke_complex_arccos_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccos_real(p->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arctan(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arctan(p->obj.complex);
    return top;
}

static int ke_complex_arcsec(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsec(p->obj.complex);
    return top;
}

static int ke_complex_arcsec_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsec_real(p->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccsc(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccsc(p->obj.complex);
    return top;
}

static int ke_complex_arccsc_real(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccsc_real(p->r);
    p->ttype = KET_VAL;
    p->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccot(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccot(p->obj.complex);
    return top;
}

static int ke_complex_sinh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sinh(p->obj.complex);
    return top;
}

static int ke_complex_cosh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_cosh(p->obj.complex);
    return top;
}

static int ke_complex_tanh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_tanh(p->obj.complex);
    return top;
}

static int ke_complex_sech(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_sech(p->obj.complex);
    return top;
}

static int ke_complex_csch(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_csch(p->obj.complex);
    return top;
}

static int ke_complex_coth(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_coth(p->obj.complex);
    return top;
}

static int ke_complex_arcsinh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsinh(p->obj.complex);
    return top;
}

static int ke_complex_arccosh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccosh(p->obj.complex);
    return top;
}

static int ke_complex_arctanh(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arctanh(p->obj.complex);
    return top;
}

static int ke_complex_arcsech(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arcsech(p->obj.complex);
    return top;
}

static int ke_complex_arccsch(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccsch(p->obj.complex);
    return top;
}

static int ke_complex_arccoth(ke1_t *stack, ke1_t *tokp, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.complex = gsl_complex_arccoth(p->obj.complex);
    return top;
}

void ke_complex_hash() {
    ke_hash_add((fncp)&ke_complex_alloc, COMPLEX);
    ke_hash_add((fncp)&ke_complex_set_real, COMPLEX_SET_REAL);
    ke_hash_add((fncp)&ke_complex_set_imag, COMPLEX_SET_IMAG);
    ke_hash_add((fncp)&ke_complex_arg, COMPLEX_ARG);
    ke_hash_add((fncp)&ke_complex_abs, COMPLEX_ABS);
    ke_hash_add((fncp)&ke_complex_abs2, COMPLEX_ABS2);
    ke_hash_add((fncp)&ke_complex_logabs, COMPLEX_LOGABS);
    ke_hash_add((fncp)&ke_complex_add, COMPLEX_ADD);
    ke_hash_add((fncp)&ke_complex_sub, COMPLEX_SUB);
    ke_hash_add((fncp)&ke_complex_mul, COMPLEX_MUL);
    ke_hash_add((fncp)&ke_complex_div, COMPLEX_DIV);
    ke_hash_add((fncp)&ke_complex_add_real, COMPLEX_ADD_REAL);
    ke_hash_add((fncp)&ke_complex_sub_real, COMPLEX_SUB_REAL);
    ke_hash_add((fncp)&ke_complex_mul_real, COMPLEX_MUL_REAL);
    ke_hash_add((fncp)&ke_complex_div_real, COMPLEX_DIV_REAL);
    ke_hash_add((fncp)&ke_complex_add_imag, COMPLEX_ADD_IMAG);
    ke_hash_add((fncp)&ke_complex_sub_imag, COMPLEX_SUB_IMAG);
    ke_hash_add((fncp)&ke_complex_mul_imag, COMPLEX_MUL_IMAG);
    ke_hash_add((fncp)&ke_complex_div_imag, COMPLEX_DIV_IMAG);
    ke_hash_add((fncp)&ke_complex_conjugate, COMPLEX_CONJUGATE);
    ke_hash_add((fncp)&ke_complex_inverse, COMPLEX_INVERSE);
    ke_hash_add((fncp)&ke_complex_negative, COMPLEX_NEGATIVE);
    ke_hash_add((fncp)&ke_complex_sqrt, COMPLEX_SQRT);
    ke_hash_add((fncp)&ke_complex_sqrt_real, COMPLEX_SQRT_REAL);
    ke_hash_add((fncp)&ke_complex_pow, COMPLEX_POW);
    ke_hash_add((fncp)&ke_complex_pow_real, COMPLEX_POW_REAL);
    ke_hash_add((fncp)&ke_complex_exp, COMPLEX_EXP);
    ke_hash_add((fncp)&ke_complex_log, COMPLEX_LOG);
    ke_hash_add((fncp)&ke_complex_log10, COMPLEX_LOG10);
    ke_hash_add((fncp)&ke_complex_log_b, COMPLEX_LOG_B);
    ke_hash_add((fncp)&ke_complex_sin, COMPLEX_SIN);
    ke_hash_add((fncp)&ke_complex_cos, COMPLEX_COS);
    ke_hash_add((fncp)&ke_complex_tan, COMPLEX_TAN);
    ke_hash_add((fncp)&ke_complex_sec, COMPLEX_SEC);
    ke_hash_add((fncp)&ke_complex_csc, COMPLEX_CSC);
    ke_hash_add((fncp)&ke_complex_cot, COMPLEX_COT);
    ke_hash_add((fncp)&ke_complex_arcsin, COMPLEX_ARCSIN);
    ke_hash_add((fncp)&ke_complex_arcsin_real, COMPLEX_ARCSIN_REAL);
    ke_hash_add((fncp)&ke_complex_arccos, COMPLEX_ARCCOS);
    ke_hash_add((fncp)&ke_complex_arccos_real, COMPLEX_ARCCOS_REAL);
    ke_hash_add((fncp)&ke_complex_arctan, COMPLEX_ARCTAN);
    ke_hash_add((fncp)&ke_complex_arcsec, COMPLEX_ARCSEC);
    ke_hash_add((fncp)&ke_complex_arcsec_real, COMPLEX_ARCSEC_REAL);
    ke_hash_add((fncp)&ke_complex_arccsc, COMPLEX_ARCCSC);
    ke_hash_add((fncp)&ke_complex_arccsc_real, COMPLEX_ARCCSC_REAL);
    ke_hash_add((fncp)&ke_complex_arccot, COMPLEX_ARCCOT);
    ke_hash_add((fncp)&ke_complex_sinh, COMPLEX_SINH);
    ke_hash_add((fncp)&ke_complex_cosh, COMPLEX_COSH);
    ke_hash_add((fncp)&ke_complex_tanh, COMPLEX_TANH);
    ke_hash_add((fncp)&ke_complex_sech, COMPLEX_SECH);
    ke_hash_add((fncp)&ke_complex_csch, COMPLEX_CSCH);
    ke_hash_add((fncp)&ke_complex_coth, COMPLEX_COTH);
    ke_hash_add((fncp)&ke_complex_arcsinh, COMPLEX_ARCSINH);
    ke_hash_add((fncp)&ke_complex_arccosh, COMPLEX_ARCCOSH);
    ke_hash_add((fncp)&ke_complex_arctanh, COMPLEX_ARCTANH);
    ke_hash_add((fncp)&ke_complex_arcsech, COMPLEX_ARCSECH);
    ke_hash_add((fncp)&ke_complex_arccsch, COMPLEX_ARCCSCH);
    ke_hash_add((fncp)&ke_complex_arccoth, COMPLEX_ARCCOTH);
}

void ke_complex_print(ke1_t *k) {
    printf("Complex: %s\n", k->name);
    printf("%g,%g \n", GSL_REAL(k->obj.complex), GSL_IMAG(k->obj.complex));
}

