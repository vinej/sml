#include <string.h>
#include <stdio.h>
#include "kexpr.h"
#include "vector.h"
#include "khash.h"
#include <gsl/gsl_vector.h>

static int ke_vector_alloc(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.vector = gsl_vector_alloc((size_t)p->i); ke_inc_memory(sml);
    p->ttype = KET_VAL;
    p->vtype = KEV_VEC;
    return top;
}

static int ke_vector_int_alloc(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->obj.vector_int = gsl_vector_int_alloc((size_t)p->i); ke_inc_memory(sml);
	p->ttype = KET_VAL;
	p->vtype = KEV_VEC_INT;
	return top;
}

static int ke_vector_free(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	ke_set_null_vector(sml,p->ifield);
	--top;
	return top;
}

static int ke_vector_int_free(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	ke_set_null_vector_int(sml, p->ifield);
	--top;
	return top;
}

int ke_vector_get(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
	p = &stack[--top];
	q = &stack[top - 1];
    q->r = gsl_vector_get(p->obj.vector, (size_t)q->i);
    q->ttype = KET_VAL;
    q->vtype = KEV_REAL;
    q->obj.vector = NULL;
    return top;
}

int ke_vector_int_get(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	p->i = gsl_vector_int_get(p->obj.vector_int, (size_t)q->i);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector_int = NULL;
	return top;
}

int ke_vector_prop_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v;
	v = &stack[--top],
	p = &stack[--top];
	q = &stack[--top],
	gsl_vector_set(p->obj.vector, (size_t)q->i, v->r);
	return top;
}

int ke_vector_int_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v;
	v = &stack[--top],
		q = &stack[--top],
		p = &stack[--top];
	gsl_vector_int_set(p->obj.vector_int, (int)q->i, (int)v->i);
	return top;
}

int ke_vector_prop_int_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v;
	v = &stack[--top],
	p = &stack[--top];
	q = &stack[--top],
	gsl_vector_int_set(p->obj.vector_int, (size_t)q->i, (int)v->i);
	return top;
}

int ke_vector_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[--top];
    gsl_vector_set(p->obj.vector, (size_t)q->i, v->r);
    return top;
}

int ke_vector_put(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok(sml);
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

int ke_vector_int_put(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok(sml);
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	// set parameter value from the stack
	for (int j = 0; j < n - 1; ++j) {
		if (j == p->obj.vector_int->size) break;
		q = &stack[top - n + j + 1];
		gsl_vector_int_set(p->obj.vector_int, (size_t)j, (int)q->i);
	}
	return top - e->n_args;
}

static int ke_vector_set_all(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_set_all(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_int_set_all(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_set_all(p->obj.vector_int, (int)q->i);
	--top;
	return top;
}

static int ke_vector_set_zero(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    gsl_vector_set_zero(p->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_set_zero(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	gsl_vector_int_set_zero(p->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_set_basis(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_set_basis(p->obj.vector, (size_t)q->i);
    --top;
    return top;
}

static int ke_vector_int_set_basis(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_set_basis(p->obj.vector_int, (size_t)q->i);
	--top;
	return top;
}

static int ke_vector_add(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_add(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_add(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_add(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_sub(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_sub(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_sub(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_sub(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_mul(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_mul(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_mul(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_mul(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_div(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_div(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_div(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_div(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_scale(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_scale(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_int_scale(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_scale(p->obj.vector_int, (int)q->r);
	--top;
	return top;
}

static int ke_vector_add_constant(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_add_constant(p->obj.vector, q->r);
    --top;
    return top;
}

static int ke_vector_int_add_constant(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_add_constant(p->obj.vector_int, (int)q->i);
	--top;
	return top;
}

static int ke_vector_reverse(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    gsl_vector_reverse(p->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_reverse(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	gsl_vector_int_reverse(p->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_swap_elements(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_swap_elements(p->obj.vector, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_vector_int_swap_elements(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v;
	v = &stack[--top],
		q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_swap_elements(p->obj.vector_int, (size_t)q->i, (size_t)v->i);
	--top;
	return top;
}

static int ke_vector_memcpy(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_memcpy(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_memcpy(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_memcpy(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_swap(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_swap(p->obj.vector, q->obj.vector);
    --top;
    return top;
}

static int ke_vector_int_swap(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_swap(p->obj.vector_int, q->obj.vector_int);
	--top;
	return top;
}

static int ke_vector_min(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_vector_min(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_min(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i= gsl_vector_int_min(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector_int = NULL;
	return top;
}

static int ke_vector_max(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_vector_max(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_max(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i = gsl_vector_int_max(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_isnull(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isnull(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_isnull(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i = gsl_vector_int_isnull(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_ispos(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_ispos(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_ispos(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i = gsl_vector_int_ispos(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_isneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isneg(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_isneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i = gsl_vector_int_isneg(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_isnonneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_vector_isnonneg(p->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_isnonneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	p->i = gsl_vector_int_isnonneg(p->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_equal(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_vector_equal(p->obj.vector, q->obj.vector);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.vector = NULL;
    return top;
}

static int ke_vector_int_equal(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	q = &stack[--top],
		p = &stack[top - 1];
	gsl_vector_int_equal(p->obj.vector_int, q->obj.vector_int);
	p->ttype = KET_VAL;
	p->vtype = KEV_INT;
	p->obj.vector = NULL;
	return top;
}

static int ke_vector_fscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_vector_fscanf(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

static int ke_vector_int_fscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	p = &stack[--top],
		q = &stack[top - 1];
	FILE * f = fopen(q->obj.s, "r");
	gsl_vector_int_fscanf(f, p->obj.vector_int);
	fclose(f);
	--top;
	return top;
}

static int ke_vector_fprintf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_vector_fprintf(f, p->obj.vector,"%5g");
    fclose(f);
    --top;
    return top;
}

static int ke_vector_int_fprintf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	p = &stack[--top],
		q = &stack[top - 1];
	FILE * f = fopen(q->obj.s, "w");
	gsl_vector_int_fprintf(f, p->obj.vector_int, "%d");
	fclose(f);
	--top;
	return top;
}

static int ke_vector_fread(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_vector_fread(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

static int ke_vector_int_fread(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	p = &stack[--top],
		q = &stack[top - 1];
	FILE * f = fopen(q->obj.s, "r");
	gsl_vector_int_fread(f, p->obj.vector_int);
	fclose(f);
	--top;
	return top;
}

static int ke_vector_fwrite(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_vector_fwrite(f, p->obj.vector);
    fclose(f);
    --top;
    return top;
}

static int ke_vector_int_fwrite(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	p = &stack[--top],
		q = &stack[top - 1];
	FILE * f = fopen(q->obj.s, "w");
	gsl_vector_int_fwrite(f, p->obj.vector_int);
	fclose(f);
	--top;
	return top;
}

void ke_vector_hash(sml_t* sml) {
    ke_hash_add(sml, (fncp)&ke_vector_alloc, VECTOR);
    ke_hash_add(sml, (fncp)&ke_vector_alloc, VECTOR_ALLOC);
    ke_hash_add(sml, (fncp)&ke_vector_get, VECTOR_GET);
    ke_hash_add(sml, (fncp)&ke_vector_set, VECTOR_SET);
	ke_hash_add(sml, (fncp)&ke_vector_get, VECTOR_SGET);
	ke_hash_add(sml, (fncp)&ke_vector_set, VECTOR_SSET);
	ke_hash_add(sml, (fncp)&ke_vector_put, VECTOR_PUT);
	ke_hash_add(sml, (fncp)&ke_vector_free, VECTOR_FREE);
    ke_hash_add(sml, (fncp)&ke_vector_set_all, VECTOR_SET_ALL);
    ke_hash_add(sml, (fncp)&ke_vector_set_zero, VECTOR_SET_ZERO);
    ke_hash_add(sml, (fncp)&ke_vector_set_basis, VECTOR_SET_BASIS);
    ke_hash_add(sml, (fncp)&ke_vector_add, VECTOR_ADD);
    ke_hash_add(sml, (fncp)&ke_vector_sub, VECTOR_SUB);
    ke_hash_add(sml, (fncp)&ke_vector_mul, VECTOR_MUL);
    ke_hash_add(sml, (fncp)&ke_vector_div, VECTOR_DIV);
    ke_hash_add(sml, (fncp)&ke_vector_scale, VECTOR_SCALE);
    ke_hash_add(sml, (fncp)&ke_vector_add_constant, VECTOR_ADD_CONSTANT);
    ke_hash_add(sml, (fncp)&ke_vector_reverse, VECTOR_REVERSE);
    ke_hash_add(sml, (fncp)&ke_vector_swap_elements, VECTOR_SWAP_ELEMENTS);
    ke_hash_add(sml, (fncp)&ke_vector_memcpy, VECTOR_MEMCPY);
    ke_hash_add(sml, (fncp)&ke_vector_swap, VECTOR_SWAP);
    ke_hash_add(sml, (fncp)&ke_vector_min, VECTOR_MIN);
    ke_hash_add(sml, (fncp)&ke_vector_max, VECTOR_MAX);
    ke_hash_add(sml, (fncp)&ke_vector_isnull, VECTOR_ISNULL);
    ke_hash_add(sml, (fncp)&ke_vector_ispos, VECTOR_ISPOS);
    ke_hash_add(sml, (fncp)&ke_vector_isneg, VECTOR_ISNEG);
    ke_hash_add(sml, (fncp)&ke_vector_isnonneg, VECTOR_ISNONNEG);
    ke_hash_add(sml, (fncp)&ke_vector_equal, VECTOR_EQUAL);
    ke_hash_add(sml, (fncp)&ke_vector_fscanf, VECTOR_FSCANF);
    ke_hash_add(sml, (fncp)&ke_vector_fscanf, VECTOR_SCAN);
    ke_hash_add(sml, (fncp)&ke_vector_fprintf, VECTOR_FPRINTF);
    ke_hash_add(sml, (fncp)&ke_vector_fprintf, VECTOR_PRINT);
    ke_hash_add(sml, (fncp)&ke_vector_fread, VECTOR_FREAD);
    ke_hash_add(sml, (fncp)&ke_vector_fread, VECTOR_READ);
    ke_hash_add(sml, (fncp)&ke_vector_fwrite, VECTOR_FWRITE);
    ke_hash_add(sml, (fncp)&ke_vector_fwrite, VECTOR_WRITE);

	ke_hash_add(sml, (fncp)&ke_vector_int_alloc, VECTOR_INT);
	ke_hash_add(sml, (fncp)&ke_vector_int_alloc, VECTOR_INT_ALLOC);
	ke_hash_add(sml, (fncp)&ke_vector_int_get, VECTOR_INT_GET);
	ke_hash_add(sml, (fncp)&ke_vector_int_set, VECTOR_INT_SET);
	ke_hash_add(sml, (fncp)&ke_vector_int_put, VECTOR_INT_PUT);
	ke_hash_add(sml, (fncp)&ke_vector_int_free, VECTOR_INT_FREE);
	ke_hash_add(sml, (fncp)&ke_vector_int_set_all, VECTOR_INT_SET_ALL);
	ke_hash_add(sml, (fncp)&ke_vector_int_set_zero, VECTOR_INT_SET_ZERO);
	ke_hash_add(sml, (fncp)&ke_vector_int_set_basis, VECTOR_INT_SET_BASIS);
	ke_hash_add(sml, (fncp)&ke_vector_int_add, VECTOR_INT_ADD);
	ke_hash_add(sml, (fncp)&ke_vector_int_sub, VECTOR_INT_SUB);
	ke_hash_add(sml, (fncp)&ke_vector_int_mul, VECTOR_INT_MUL);
	ke_hash_add(sml, (fncp)&ke_vector_int_div, VECTOR_INT_DIV);
	ke_hash_add(sml, (fncp)&ke_vector_int_scale, VECTOR_INT_SCALE);
	ke_hash_add(sml, (fncp)&ke_vector_int_add_constant, VECTOR_INT_ADD_CONSTANT);
	ke_hash_add(sml, (fncp)&ke_vector_int_reverse, VECTOR_INT_REVERSE);
	ke_hash_add(sml, (fncp)&ke_vector_int_swap_elements, VECTOR_INT_SWAP_ELEMENTS);
	ke_hash_add(sml, (fncp)&ke_vector_int_memcpy, VECTOR_INT_MEMCPY);
	ke_hash_add(sml, (fncp)&ke_vector_int_swap, VECTOR_INT_SWAP);
	ke_hash_add(sml, (fncp)&ke_vector_int_min, VECTOR_INT_MIN);
	ke_hash_add(sml, (fncp)&ke_vector_int_max, VECTOR_INT_MAX);
	ke_hash_add(sml, (fncp)&ke_vector_int_isnull, VECTOR_INT_ISNULL);
	ke_hash_add(sml, (fncp)&ke_vector_int_ispos, VECTOR_INT_ISPOS);
	ke_hash_add(sml, (fncp)&ke_vector_int_isneg, VECTOR_INT_ISNEG);
	ke_hash_add(sml, (fncp)&ke_vector_int_isnonneg, VECTOR_INT_ISNONNEG);
	ke_hash_add(sml, (fncp)&ke_vector_int_equal, VECTOR_INT_EQUAL);
	ke_hash_add(sml, (fncp)&ke_vector_int_fscanf, VECTOR_INT_FSCANF);
	ke_hash_add(sml, (fncp)&ke_vector_int_fscanf, VECTOR_INT_SCAN);
	ke_hash_add(sml, (fncp)&ke_vector_int_fprintf, VECTOR_INT_FPRINTF);
	ke_hash_add(sml, (fncp)&ke_vector_int_fprintf, VECTOR_INT_PRINT);
	ke_hash_add(sml, (fncp)&ke_vector_int_fread, VECTOR_INT_FREAD);
	ke_hash_add(sml, (fncp)&ke_vector_int_fread, VECTOR_INT_READ);
	ke_hash_add(sml, (fncp)&ke_vector_int_fwrite, VECTOR_INT_FWRITE);
	ke_hash_add(sml, (fncp)&ke_vector_int_fwrite, VECTOR_INT_WRITE);

}

void ke_vector_print(sml_t* sml, ke1_t *k) {
    printf("Vector: %s\n", k->name);
    for(size_t i = 0; i < k->obj.vector->size; i++) {
        printf("%d : v:%g\n", (int)i, gsl_vector_get(k->obj.vector, i));
    }
}

void ke_vector_freemem(sml_t* sml,ke1_t *e) {
    if (e->obj.vector && e->vtype == KEV_VEC) {
        gsl_vector_free(e->obj.vector); ke_dec_memory(sml);
        e->obj.vector = NULL;
    }
}

void ke_vector_int_print(sml_t* sml,ke1_t *k) {
	printf("Vector: %s\n", k->name);
	for (size_t i = 0; i < k->obj.vector_int->size; i++) {
		printf("%d : v:%d\n", (int)i, gsl_vector_int_get(k->obj.vector_int, i));
	}
}

void ke_vector_int_freemem(sml_t* sml,ke1_t *e) {
	if (e->obj.vector_int && e->vtype == KEV_VEC_INT) {
		gsl_vector_int_free(e->obj.vector_int); ke_dec_memory(sml);
		e->obj.vector_int = NULL;
	}
}
