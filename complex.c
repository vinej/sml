#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <string.h>
#include "kexpr.h"
#include "complex.h"
#include "khash.h"

static int ke_complex_alloc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
   	ke1_t *out,*p, *q;
    q = stack[--top];
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	GSL_SET_COMPLEX(&out->obj.tcomplex, (double)p->i, (double)q->i);
    out->ttype = KET_VAL;
    out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_set_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top];
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	GSL_SET_REAL(&out->obj.tcomplex, (double)q->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_set_imag(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top];
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	GSL_SET_IMAG(&out->obj.tcomplex, q->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arg(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
   	ke1_t *out,*p;
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->r = gsl_complex_arg(p->obj.tcomplex);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
    return top;
}

static int ke_complex_abs(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->r = gsl_complex_abs(p->obj.tcomplex);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
    return top;
}

static int ke_complex_abs2(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->r = gsl_complex_abs2(p->obj.tcomplex);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
    return top;
}

static int ke_complex_logabs(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->r = gsl_complex_logabs(p->obj.tcomplex);
	out->ttype = KET_VAL;
    out->vtype = KEV_REAL;
    return top;
}

static int ke_complex_add(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top];
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_add(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_sub(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sub(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_mul(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_mul(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_div(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_div(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_add_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_add_real(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sub_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sub_real(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_mul_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_mul_real(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_div_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_div_real(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_add_imag(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_add_imag(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sub_imag(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sub_imag(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_mul_imag(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_mul_imag(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_div_imag(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_div_imag(p->obj.tcomplex, q->r);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_conjugate(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_conjugate(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_inverse(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_inverse(p->obj.tcomplex);
    return top;
}

static int ke_complex_negative(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_negative(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sqrt(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sqrt(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sqrt_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sqrt_real(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_pow(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_pow(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_pow_real(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_pow_real(p->obj.tcomplex, q->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_exp(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_exp(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_log(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_log(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_log10(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_log10(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_log_b(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	q = stack[--top];
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_log_b(p->obj.tcomplex, q->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sin(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sin(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_cos(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_cos(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_tan(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_tan(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sec(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sec(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_csc(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_csc(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_cot(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_cot(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsin(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsin(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsin_real(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsin_real(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccos(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccos(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arccos_real(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccos_real(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arctan(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arctan(p->obj.tcomplex);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsec(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsec(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsec_real(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsec_real(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccsc(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccsc(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arccsc_real(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccsc_real(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_COMPLEX;
    return top;
}

static int ke_complex_arccot(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccot(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sinh(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sinh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_cosh(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_cosh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_tanh(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_tanh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_sech(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_sech(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_csch(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_csch(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_coth(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_coth(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsinh(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsinh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arccosh(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccosh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arctanh(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arctanh(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arcsech(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arcsech(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arccsch(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccsch(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

static int ke_complex_arccoth(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->obj.tcomplex = gsl_complex_arccoth(p->obj.tcomplex);
	out->ttype = p->ttype;
	out->vtype = KEV_COMPLEX;
	return top;
}

void ke_complex_hash(sml_t* sml) {
    ke_hash_add(sml, (fncp)&ke_complex_alloc, COMPLEX);
    ke_hash_add(sml, (fncp)&ke_complex_set_real, COMPLEX_SET_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_set_imag, COMPLEX_SET_IMAG);
    ke_hash_add(sml, (fncp)&ke_complex_arg, COMPLEX_ARG);
    ke_hash_add(sml, (fncp)&ke_complex_abs, COMPLEX_ABS);
    ke_hash_add(sml, (fncp)&ke_complex_abs2, COMPLEX_ABS2);
    ke_hash_add(sml, (fncp)&ke_complex_logabs, COMPLEX_LOGABS);
    ke_hash_add(sml, (fncp)&ke_complex_add, COMPLEX_ADD);
    ke_hash_add(sml, (fncp)&ke_complex_sub, COMPLEX_SUB);
    ke_hash_add(sml, (fncp)&ke_complex_mul, COMPLEX_MUL);
    ke_hash_add(sml, (fncp)&ke_complex_div, COMPLEX_DIV);
    ke_hash_add(sml, (fncp)&ke_complex_add_real, COMPLEX_ADD_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_sub_real, COMPLEX_SUB_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_mul_real, COMPLEX_MUL_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_div_real, COMPLEX_DIV_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_add_imag, COMPLEX_ADD_IMAG);
    ke_hash_add(sml, (fncp)&ke_complex_sub_imag, COMPLEX_SUB_IMAG);
    ke_hash_add(sml, (fncp)&ke_complex_mul_imag, COMPLEX_MUL_IMAG);
    ke_hash_add(sml, (fncp)&ke_complex_div_imag, COMPLEX_DIV_IMAG);
    ke_hash_add(sml, (fncp)&ke_complex_conjugate, COMPLEX_CONJUGATE);
    ke_hash_add(sml, (fncp)&ke_complex_inverse, COMPLEX_INVERSE);
    ke_hash_add(sml, (fncp)&ke_complex_negative, COMPLEX_NEGATIVE);
    ke_hash_add(sml, (fncp)&ke_complex_sqrt, COMPLEX_SQRT);
    ke_hash_add(sml, (fncp)&ke_complex_sqrt_real, COMPLEX_SQRT_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_pow, COMPLEX_POW);
    ke_hash_add(sml, (fncp)&ke_complex_pow_real, COMPLEX_POW_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_exp, COMPLEX_EXP);
    ke_hash_add(sml, (fncp)&ke_complex_log, COMPLEX_LOG);
    ke_hash_add(sml, (fncp)&ke_complex_log10, COMPLEX_LOG10);
    ke_hash_add(sml, (fncp)&ke_complex_log_b, COMPLEX_LOG_B);
    ke_hash_add(sml, (fncp)&ke_complex_sin, COMPLEX_SIN);
    ke_hash_add(sml, (fncp)&ke_complex_cos, COMPLEX_COS);
    ke_hash_add(sml, (fncp)&ke_complex_tan, COMPLEX_TAN);
    ke_hash_add(sml, (fncp)&ke_complex_sec, COMPLEX_SEC);
    ke_hash_add(sml, (fncp)&ke_complex_csc, COMPLEX_CSC);
    ke_hash_add(sml, (fncp)&ke_complex_cot, COMPLEX_COT);
    ke_hash_add(sml, (fncp)&ke_complex_arcsin, COMPLEX_ARCSIN);
    ke_hash_add(sml, (fncp)&ke_complex_arcsin_real, COMPLEX_ARCSIN_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_arccos, COMPLEX_ARCCOS);
    ke_hash_add(sml, (fncp)&ke_complex_arccos_real, COMPLEX_ARCCOS_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_arctan, COMPLEX_ARCTAN);
    ke_hash_add(sml, (fncp)&ke_complex_arcsec, COMPLEX_ARCSEC);
    ke_hash_add(sml, (fncp)&ke_complex_arcsec_real, COMPLEX_ARCSEC_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_arccsc, COMPLEX_ARCCSC);
    ke_hash_add(sml, (fncp)&ke_complex_arccsc_real, COMPLEX_ARCCSC_REAL);
    ke_hash_add(sml, (fncp)&ke_complex_arccot, COMPLEX_ARCCOT);
    ke_hash_add(sml, (fncp)&ke_complex_sinh, COMPLEX_SINH);
    ke_hash_add(sml, (fncp)&ke_complex_cosh, COMPLEX_COSH);
    ke_hash_add(sml, (fncp)&ke_complex_tanh, COMPLEX_TANH);
    ke_hash_add(sml, (fncp)&ke_complex_sech, COMPLEX_SECH);
    ke_hash_add(sml, (fncp)&ke_complex_csch, COMPLEX_CSCH);
    ke_hash_add(sml, (fncp)&ke_complex_coth, COMPLEX_COTH);
    ke_hash_add(sml, (fncp)&ke_complex_arcsinh, COMPLEX_ARCSINH);
    ke_hash_add(sml, (fncp)&ke_complex_arccosh, COMPLEX_ARCCOSH);
    ke_hash_add(sml, (fncp)&ke_complex_arctanh, COMPLEX_ARCTANH);
    ke_hash_add(sml, (fncp)&ke_complex_arcsech, COMPLEX_ARCSECH);
    ke_hash_add(sml, (fncp)&ke_complex_arccsch, COMPLEX_ARCCSCH);
    ke_hash_add(sml, (fncp)&ke_complex_arccoth, COMPLEX_ARCCOTH);
}

void ke_complex_print(sml_t* sml,ke1_t *k) {
    printf("Complex: %s\n", k->name);
    printf("%g,%g \n", GSL_REAL(k->obj.tcomplex), GSL_IMAG(k->obj.tcomplex));
}

