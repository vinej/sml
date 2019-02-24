#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <string.h>
#include "kexpr.h"
#include "complex.h"
#include "api.c"

static void ke_complex_alloc(sml_t* sml) { 
	int y = sml_pop_int(sml);
	int x = sml_pop_int(sml);
	sml_push_new_complex(sml);
	GSL_SET_COMPLEX(sml_get_complex_adr, (double)x, (double)y);
}

static void ke_complex_set_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex *z = sml_pop_complex_adr(sml);
	GSL_SET_REAL(z, x);
}

static void ke_complex_set_imag(sml_t* sml) { 
	double y = sml_pop_real(sml);
	gsl_complex *z = sml_pop_complex_adr(sml);
	GSL_SET_IMAG(z, y);
}

static void ke_complex_arg(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	double r = gsl_complex_arg(z);
	sml_push_real(sml,r);
}

static void ke_complex_abs(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	double r = gsl_complex_abs(z);
	sml_push_real(sml, r);
}

static void ke_complex_abs2(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	double r = gsl_complex_abs2(z);
	sml_push_real(sml, r);
}

static void ke_complex_logabs(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	double r = gsl_complex_logabs(z);
	sml_push_real(sml, r);
}

static void ke_complex_add(sml_t* sml) { 
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_add(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_sub(sml_t* sml) { 
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sub(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_mul(sml_t* sml) { 
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_mul(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_div(sml_t* sml) { 
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_div(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_add_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_add_real(a, x);
	sml_push_complex(sml, z);
}


static void ke_complex_sub_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sub_real(a, x);
	sml_push_complex(sml, z);
}

static void ke_complex_mul_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_mul_real(a, x);
	sml_push_complex(sml, z);
}

static void ke_complex_div_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_div_real(a, x);
	sml_push_complex(sml, z);
}

static void ke_complex_add_imag(sml_t* sml) {
	double y = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_add_imag(a, y);
	sml_push_complex(sml, z);
}

static void ke_complex_sub_imag(sml_t* sml) { 
	double y = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sub_imag(a, y);
	sml_push_complex(sml, z);
}

static void ke_complex_mul_imag(sml_t* sml) { 
	double y = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_mul_imag(a, y);
	sml_push_complex(sml, z);
}

static void ke_complex_div_imag(sml_t* sml) { 
	double y = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_div_imag(a, y);
	sml_push_complex(sml, z);
}

static void ke_complex_conjugate(sml_t* sml) {
	gsl_complex z = sml_pop_complex(sml);
	z = gsl_complex_conjugate(z);
	sml_push_complex(sml, z);
}

static void ke_complex_inverse(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	z = gsl_complex_inverse(z);
	sml_push_complex(sml, z);
}

static void ke_complex_negative(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	z = gsl_complex_negative(z);
	sml_push_complex(sml, z);
}

static void ke_complex_sqrt(sml_t* sml) { 
	gsl_complex z = sml_pop_complex(sml);
	z = gsl_complex_sqrt(z);
	sml_push_complex(sml, z);
}

static void ke_complex_sqrt_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex z = gsl_complex_sqrt_real(x);
	sml_push_complex(sml, z);
}

static void ke_complex_pow(sml_t* sml) {
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_pow(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_pow_real(sml_t* sml) {
	double b = sml_pop_real(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_pow_real(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_exp(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_exp(a);
	sml_push_complex(sml, z);
}

static void ke_complex_log(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_log(a);
	sml_push_complex(sml, z);
}

static void ke_complex_log10(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_log10(a);
	sml_push_complex(sml, z);
}

static void ke_complex_log_b(sml_t* sml) {
	gsl_complex b = sml_pop_complex(sml);
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_log_b(a, b);
	sml_push_complex(sml, z);
}

static void ke_complex_sin(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sin(a);
	sml_push_complex(sml, z);
}

static void ke_complex_cos(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_cos(a);
	sml_push_complex(sml, z);
}

static void ke_complex_tan(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_tan(a);
	sml_push_complex(sml, z);
}

static void ke_complex_sec(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sec(a);
	sml_push_complex(sml, z);
}

static void ke_complex_csc(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_csc(a);
	sml_push_complex(sml, z);
}

static void ke_complex_cot(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_cot(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsin(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arcsin(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsin_real(sml_t* sml) {
	double x = sml_pop_real(sml);
	gsl_complex z = gsl_complex_arcsin_real(x);
	sml_push_complex(sml, z);
}

static void ke_complex_arccos(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccos(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arccos_real(sml_t* sml) {
	double x = sml_pop_real(sml);
	gsl_complex z = gsl_complex_arccos_real(x);
	sml_push_complex(sml, z);
}

static void ke_complex_arctan(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arctan(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsec(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arcsec(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsec_real(sml_t* sml) {
	double x = sml_pop_real(sml);
	gsl_complex z = gsl_complex_arcsec_real(x);
	sml_push_complex(sml, z);
}

static void ke_complex_arccsc(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccsc(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arccsc_real(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_complex z = gsl_complex_arccsc_real(x);
	sml_push_complex(sml, z);
}

static void ke_complex_arccot(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccot(a);
	sml_push_complex(sml, z);
}

static void ke_complex_sinh(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sinh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_cosh(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_cosh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_tanh(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_tanh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_sech(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_sech(a);
	sml_push_complex(sml, z);
}

static void ke_complex_csch(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_csch(a);
	sml_push_complex(sml, z);
}

static void ke_complex_coth(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_coth(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsinh(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arcsinh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arccosh(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccosh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arctanh(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arctanh(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arcsech(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arcsech(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arccsch(sml_t* sml) {
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccsch(a);
	sml_push_complex(sml, z);
}

static void ke_complex_arccoth(sml_t* sml) { 
	gsl_complex a = sml_pop_complex(sml);
	gsl_complex z = gsl_complex_arccoth(a);
	sml_push_complex(sml, z);
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

