#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "kexpr.h"
#include "vector.h"
#include "khash.h"
#include <gsl/gsl_vector.h>

static int ke_vector_alloc(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.vector = gsl_vector_alloc((size_t)p->i); ke_inc_memory();
    p->ttype = KET_VAL;
    p->vtype = KEV_VEC;
    return top;
}

static int ke_vector_get(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = gsl_vector_get(p->obj.vector, (size_t)q->i);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_set(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[--top];
    gsl_vector_set(p->obj.vector, (size_t)q->i, v->r);
    return top;
}

static int ke_vector_put(ke1_t *stack, int top) {
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok();
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	// set parameter value from the stack
	for (int j = 0; j < n - 1; ++j) {
		if (j == p->obj.vector->size) break;
		q = &stack[top - n + j + 1];
		gsl_vector_set(p->obj.vector, (size_t)j, q->r);
	}
	return top - e->n_args;
}

static int ke_vector_free(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    ke_set_null_vector(p->ifield);
    --top;
    return top;
}

static int ke_vector_set_all(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_set_all(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_set_zero(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    gsl_vector_set_zero(p->obj.vector);
    --top;
    return top;
}

static int ke_vector_set_basis(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_set_basis(p->obj.vector, (size_t)q->i);
    --top;
    return top;
}

static int ke_vector_add(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_add(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_sub(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_sub(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_mul(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_mul(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_div(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_div(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_scale(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_scale(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_add_constant(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_add_constant(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_reverse(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    gsl_vector_reverse(p->obj.vector);
    --top;
    return top;
}

static int ke_vector_swap_elements(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_swap_elements(p->obj.vector, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_vector_memcpy(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_memcpy(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_swap(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_swap(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_min(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_vector_min(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_max(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_vector_max(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_isnull(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isnull(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_ispos(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_ispos(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_isneg(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isneg(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_isnonneg(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isnonneg(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_equal(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_equal(p->obj.vector, q->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_fscanf(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_vector_fscanf(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

static int ke_vector_fprintf(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_vector_fprintf(f, p->obj.vector,"%5g");
    fclose(f);
    --top;
    return top;
}

static int ke_vector_fread(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_vector_fread(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

static int ke_vector_fwrite(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_vector_fwrite(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

void ke_vector_hash() {
    ke_hash_add((fncp)&ke_vector_alloc, VECTOR);
    ke_hash_add((fncp)&ke_vector_alloc, VECTOR_ALLOC);
    ke_hash_add((fncp)&ke_vector_get, VECTOR_GET);
    ke_hash_add((fncp)&ke_vector_set, VECTOR_SET);
	ke_hash_add((fncp)&ke_vector_put, VECTOR_PUT);
	ke_hash_add((fncp)&ke_vector_free, VECTOR_FREE);
    ke_hash_add((fncp)&ke_vector_set_all, VECTOR_SET_ALL);
    ke_hash_add((fncp)&ke_vector_set_zero, VECTOR_SET_ZERO);
    ke_hash_add((fncp)&ke_vector_set_basis, VECTOR_SET_BASIS);
    ke_hash_add((fncp)&ke_vector_add, VECTOR_ADD);
    ke_hash_add((fncp)&ke_vector_sub, VECTOR_SUB);
    ke_hash_add((fncp)&ke_vector_mul, VECTOR_MUL);
    ke_hash_add((fncp)&ke_vector_div, VECTOR_DIV);
    ke_hash_add((fncp)&ke_vector_scale, VECTOR_SCALE);
    ke_hash_add((fncp)&ke_vector_add_constant, VECTOR_ADD_CONSTANT);
    ke_hash_add((fncp)&ke_vector_reverse, VECTOR_REVERSE);
    ke_hash_add((fncp)&ke_vector_swap_elements, VECTOR_SWAP_ELEMENTS);
    ke_hash_add((fncp)&ke_vector_memcpy, VECTOR_MEMCPY);
    ke_hash_add((fncp)&ke_vector_swap, VECTOR_SWAP);
    ke_hash_add((fncp)&ke_vector_min, VECTOR_MIN);
    ke_hash_add((fncp)&ke_vector_max, VECTOR_MAX);
    ke_hash_add((fncp)&ke_vector_isnull, VECTOR_ISNULL);
    ke_hash_add((fncp)&ke_vector_ispos, VECTOR_ISPOS);
    ke_hash_add((fncp)&ke_vector_isneg, VECTOR_ISNEG);
    ke_hash_add((fncp)&ke_vector_isnonneg, VECTOR_ISNONNEG);
    ke_hash_add((fncp)&ke_vector_equal, VECTOR_EQUAL);
    ke_hash_add((fncp)&ke_vector_fscanf, VECTOR_FSCANF);
    ke_hash_add((fncp)&ke_vector_fscanf, VECTOR_SCAN);
    ke_hash_add((fncp)&ke_vector_fprintf, VECTOR_FPRINTF);
    ke_hash_add((fncp)&ke_vector_fprintf, VECTOR_PRINT);
    ke_hash_add((fncp)&ke_vector_fread, VECTOR_FREAD);
    ke_hash_add((fncp)&ke_vector_fread, VECTOR_READ);
    ke_hash_add((fncp)&ke_vector_fwrite, VECTOR_FWRITE);
    ke_hash_add((fncp)&ke_vector_fwrite, VECTOR_WRITE);
}

void ke_vector_print(ke1_t *k) {
    printf("Vector: %s\n", k->name);
    for(size_t i = 0; i < k->obj.vector->size; i++) {
        printf("%d : v:%g\n", i, gsl_vector_get(k->obj.vector, i));
    }
}

void ke_vector_freemem(ke1_t *e) {
    if (e->obj.vector && e->vtype == KEV_VEC) {
        gsl_vector_free(e->obj.vector); ke_dec_memory();
        e->obj.vector = NULL;
    }
}

