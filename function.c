#include "stdafx.h"
#include <math.h>
#include "khash.h"
#include "kexpr.h"
#include "function.h"

static int ke_function_exp(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = exp(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_pow(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = pow(p->r, q->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_log(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = log(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_log10(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = log10(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sqrt(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = sqrt(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sin(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = sin(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cos(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = cos(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_tan(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = tan(p->r);    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_floor(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = floor(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_ceil(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = ceil(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acos(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = acos(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asin(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = asin(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atan(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = atan(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cosh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = cosh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sinh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = sinh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_tanh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = tanh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atan2(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = atan2(p->r, q->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_fmod(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = fmod(p->r, q->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_csc(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/sin(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sec(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/cos(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cot(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/tan(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_csch(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/sinh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sech(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/cosh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_coth(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = 1/tanh(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acosh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(cosh(p->r),-1);
    p->vtype = KEV_REAL;
    p->ttype = KET_VAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asinh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(sinh(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atanh(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(tanh(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acsc(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/sin(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asec(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/cos(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acot(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/tan(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acsch(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/sinh(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asech(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/cosh(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acoth(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = pow(1/tanh(p->r),-1);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

void ke_function_hash() {
    ke_hash_add((fncp)&ke_function_exp, FUNCTION_EXP);
    ke_hash_add((fncp)&ke_function_pow, FUNCTION_POW);
    ke_hash_add((fncp)&ke_function_log, FUNCTION_LOG);
    ke_hash_add((fncp)&ke_function_log10, FUNCTION_LOG10);
    ke_hash_add((fncp)&ke_function_sqrt, FUNCTION_SQRT);
    ke_hash_add((fncp)&ke_function_sin, FUNCTION_SIN);
    ke_hash_add((fncp)&ke_function_cos, FUNCTION_COS);
    ke_hash_add((fncp)&ke_function_tan, FUNCTION_TAN);
    ke_hash_add((fncp)&ke_function_floor, FUNCTION_FLOOR);
    ke_hash_add((fncp)&ke_function_ceil, FUNCTION_CEIL);
    ke_hash_add((fncp)&ke_function_acos, FUNCTION_ACOS);
    ke_hash_add((fncp)&ke_function_asin, FUNCTION_ASIN);
    ke_hash_add((fncp)&ke_function_atan, FUNCTION_ATAN);
    ke_hash_add((fncp)&ke_function_cosh, FUNCTION_COSH);
    ke_hash_add((fncp)&ke_function_sinh, FUNCTION_SINH);
    ke_hash_add((fncp)&ke_function_tanh, FUNCTION_TANH);
    ke_hash_add((fncp)&ke_function_acosh, FUNCTION_ACOSH);
    ke_hash_add((fncp)&ke_function_asinh, FUNCTION_ASINH);
    ke_hash_add((fncp)&ke_function_atanh, FUNCTION_ATANH);
    ke_hash_add((fncp)&ke_function_atan2, FUNCTION_ATAN2);
    ke_hash_add((fncp)&ke_function_fmod, FUNCTION_FMOD);
    ke_hash_add((fncp)&ke_function_csc, FUNCTION_CSC);
    ke_hash_add((fncp)&ke_function_sec, FUNCTION_SEC);
    ke_hash_add((fncp)&ke_function_cot, FUNCTION_COT);
    ke_hash_add((fncp)&ke_function_csch, FUNCTION_CSCH);
    ke_hash_add((fncp)&ke_function_sech, FUNCTION_SECH);
    ke_hash_add((fncp)&ke_function_coth, FUNCTION_COTH);
    ke_hash_add((fncp)&ke_function_acsc, FUNCTION_ACSC);
    ke_hash_add((fncp)&ke_function_asec, FUNCTION_ASEC);
    ke_hash_add((fncp)&ke_function_acot, FUNCTION_ACOT);
    ke_hash_add((fncp)&ke_function_acsch, FUNCTION_ACSCH);
    ke_hash_add((fncp)&ke_function_asech, FUNCTION_ASECH);
    ke_hash_add((fncp)&ke_function_acoth, FUNCTION_ACOTH);
}

void ke_function_print(ke1_t *k) {
    printf("Function: %d\n", k->icmd);
    printf("%g\n", k->r);
}


