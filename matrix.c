#include "stdafx.h"
#include <string.h>
#include "kexpr.h"
#include "matrix.h"
#include "khash.h"
#include <gsl/gsl_matrix.h>

static int ke_matrix_alloc(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->gsl.matrix = gsl_matrix_alloc((size_t)p->i, (size_t)q->i); ke_inc_memory();
    p->ttype = KET_VAL;
    p->vtype = KEV_MAT;
    return top;
}

static int ke_matrix_get(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    p->r = gsl_matrix_get(p->gsl.matrix, (size_t)q->i, (size_t)v->i);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_set(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v, *x;
    x = &stack[--top],
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_set(p->gsl.matrix, (size_t)q->i, (size_t)v->i, x->r);
    --top;
    return top;
}

static int ke_matrix_put_row(ke1_t *stack, int top) {
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok();
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	ke1_t * row = (ke1_t *)&stack[top - e->n_args + 1];
	// set parameter value from the stack
	for (int j = 0; j < n - 2; ++j) {
		if (j == p->gsl.matrix->size2) break;
		q = &stack[top - n + j + 2];
		gsl_matrix_set(p->gsl.matrix, (size_t)row->i, (size_t)j, q->r);
	}
	return top - e->n_args;
}

static int ke_matrix_put_col(ke1_t *stack, int top) {
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok();
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	ke1_t * col = (ke1_t *)&stack[top - e->n_args + 1];
	// set parameter value from the stack
	for (int j = 0; j < n - 2; ++j) {
		if (j == p->gsl.matrix->size1) break;
		q = &stack[top - n + j + 2];
		gsl_matrix_set(p->gsl.matrix, (size_t)j, (size_t)col->i, q->r);
	}
	return top - e->n_args;
}

static int ke_matrix_free(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->gsl.matrix = NULL;
    ke_set_null_matrix(p->ifield);
    --top;
    return top;
}

static int ke_matrix_set_all(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_set_all(p->gsl.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_set_zero(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_set_zero(p->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_set_identity(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_set_identity(p->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_swap_rows(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_rows(p->gsl.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_swap_columns(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_columns(p->gsl.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_rowcol(ke1_t *stack, int top) {
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_rowcol(p->gsl.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_transpose_memcpy(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_transpose_memcpy(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_transpose(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_transpose(p->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_add(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_add(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_sub(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_sub(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_mul_elements(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_mul_elements(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_div_elements(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_div_elements(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_scale(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],  p = &stack[top-1];
    gsl_matrix_scale(p->gsl.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_add_constant(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_add_constant(p->gsl.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_memcpy(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_memcpy(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_swap(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap(p->gsl.matrix, q->gsl.matrix);
    --top;
    return top;
}

static int ke_matrix_min(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_matrix_min(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_max(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_matrix_max(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_isnull(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isnull(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_ispos(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_ispos(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_isneg(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isneg(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_isnonneg(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isnonneg(p->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_equal(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_equal(p->gsl.matrix, q->gsl.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->gsl.matrix = NULL;
    return top;
}

static int ke_matrix_fscanf(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->s, "r");
    gsl_matrix_fscanf(f, p->gsl.matrix);
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fprintf(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->s, "w");
    gsl_matrix_fprintf(f, p->gsl.matrix,"%5g");
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fread(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->s, "r");
    gsl_matrix_fread(f, p->gsl.matrix);
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fwrite(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->s, "w");
    gsl_matrix_fwrite(f, p->gsl.matrix);
    fclose(f);
    --top;
    return top;
}

void ke_matrix_hash() {
    ke_hash_add((fncp)&ke_matrix_alloc, MATRIX);
    ke_hash_add((fncp)&ke_matrix_alloc, MATRIX_ALLOC);
    ke_hash_add((fncp)&ke_matrix_get, MATRIX_GET);
    ke_hash_add((fncp)&ke_matrix_set, MATRIX_SET);
	ke_hash_add((fncp)&ke_matrix_put_row, MATRIX_PUT_ROW);
	ke_hash_add((fncp)&ke_matrix_put_col, MATRIX_PUT_COL);
	ke_hash_add((fncp)&ke_matrix_free, MATRIX_FREE);
    ke_hash_add((fncp)&ke_matrix_set_all, MATRIX_SET_ALL);
    ke_hash_add((fncp)&ke_matrix_set_zero,MATRIX_SET_ZERO);
    ke_hash_add((fncp)&ke_matrix_set_identity,MATRIX_SET_IDENTITY);
    ke_hash_add((fncp)&ke_matrix_swap_rows,MATRIX_SWAP_ROWS);
    ke_hash_add((fncp)&ke_matrix_swap_columns,MATRIX_SWAP_COLUMNS);
    ke_hash_add((fncp)&ke_matrix_rowcol,MATRIX_SWAP_ROWCOL);
    ke_hash_add((fncp)&ke_matrix_transpose_memcpy,MATRIX_TRANSPOSE_MEMCPY);
    ke_hash_add((fncp)&ke_matrix_transpose,MATRIX_TRANSPOSE);
    ke_hash_add((fncp)&ke_matrix_add,MATRIX_ADD);
    ke_hash_add((fncp)&ke_matrix_sub,MATRIX_SUB);
    ke_hash_add((fncp)&ke_matrix_mul_elements,MATRIX_MUL);
    ke_hash_add((fncp)&ke_matrix_mul_elements,MATRIX_MUL_ELEMENTS);
    ke_hash_add((fncp)&ke_matrix_div_elements,MATRIX_DIV);
    ke_hash_add((fncp)&ke_matrix_div_elements,MATRIX_DIV_ELEMENTS);
    ke_hash_add((fncp)&ke_matrix_scale,MATRIX_SCALE);
    ke_hash_add((fncp)&ke_matrix_add_constant,MATRIX_ADD_CONSTANT);
    ke_hash_add((fncp)&ke_matrix_memcpy,MATRIX_MEMCPY);
    ke_hash_add((fncp)&ke_matrix_swap,MATRIX_SWAP);
    ke_hash_add((fncp)&ke_matrix_min,MATRIX_MIN);
    ke_hash_add((fncp)&ke_matrix_max,MATRIX_MAX);
    ke_hash_add((fncp)&ke_matrix_isnull,MATRIX_ISNULL);
    ke_hash_add((fncp)&ke_matrix_ispos,MATRIX_ISPOS);
    ke_hash_add((fncp)&ke_matrix_isneg,MATRIX_ISNEG);
    ke_hash_add((fncp)&ke_matrix_isnonneg,MATRIX_ISNONNEG);
    ke_hash_add((fncp)&ke_matrix_equal,MATRIX_EQUAL);
    ke_hash_add((fncp)&ke_matrix_fscanf,MATRIX_FSCANF);
    ke_hash_add((fncp)&ke_matrix_fscanf,MATRIX_SCAN);
    ke_hash_add((fncp)&ke_matrix_fprintf,MATRIX_FPRINTF);
    ke_hash_add((fncp)&ke_matrix_fprintf,MATRIX_PRINT);
    ke_hash_add((fncp)&ke_matrix_fread,MATRIX_FREAD);
    ke_hash_add((fncp)&ke_matrix_fread,MATRIX_READ);
    ke_hash_add((fncp)&ke_matrix_fwrite,MATRIX_FWRITE);
    ke_hash_add((fncp)&ke_matrix_fwrite,MATRIX_WRITE);
}

void ke_matrix_print(ke1_t *k) {
    printf("Matrix: %s\n", k->name);
    for(size_t i = 0; i < k->gsl.matrix->size1; i++)
        for(size_t j = 0; j < k->gsl.matrix->size2; j++)
            printf("%d,%d : v:%g\n", i, j, gsl_matrix_get(k->gsl.matrix, i,j));
}

void ke_matrix_freemem(ke1_t *e) {
    if (e->gsl.matrix && e->vtype == KEV_MAT) {
        gsl_matrix_free(e->gsl.matrix); ke_dec_memory();
        e->gsl.matrix = NULL;
    }
}
