#include <string.h>
#include "kexpr.h"
#include "matrix.h"
#include "khash.h"
#include <gsl/gsl_matrix.h>

static int ke_matrix_alloc(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->obj.matrix = gsl_matrix_alloc((size_t)p->i, (size_t)q->i); ke_inc_memory(sml);
    p->ttype = KET_VAL;
    p->vtype = KEV_MAT;
    return top;
}

int ke_matrix_prop_get(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v;
	p = &stack[--top];
	v = &stack[--top];
	q = &stack[top - 1];
	q->r = gsl_matrix_get(p->obj.matrix, (size_t)q->i, (size_t)v->i);
	q->ttype = KET_VAL;
	q->vtype = KEV_REAL;
	q->obj.matrix = NULL;
	return top;
}

int ke_matrix_get(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    p->r = gsl_matrix_get(p->obj.matrix, (size_t)q->i, (size_t)v->i);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.matrix = NULL;
    return top;
}

int ke_matrix_prop_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q, *v, *x;
	x = &stack[--top];
	p = &stack[--top];
	v = &stack[--top];
	q = &stack[--top];
	gsl_matrix_set(p->obj.matrix, (size_t)q->i, (size_t)v->i, x->r);
	return top;
}

int ke_matrix_set(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v, *x;
    x = &stack[--top],
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[--top];
    gsl_matrix_set(p->obj.matrix, (size_t)q->i, (size_t)v->i, x->r);
    return top;
}

static int ke_matrix_put_row(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok(sml);
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	ke1_t * row = (ke1_t *)&stack[top - e->n_args + 1];
	// set parameter value from the stack
	for (int j = 0; j < n - 2; ++j) {
		if (j == p->obj.matrix->size2) break;
		q = &stack[top - n + j + 2];
		gsl_matrix_set(p->obj.matrix, (size_t)row->i, (size_t)j, q->r);
	}
	return top - e->n_args;
}

static int ke_matrix_put_col(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	ke1_t *e = ke_get_tok(sml);
	int n = e->n_args;
	p = (ke1_t *)&stack[top - e->n_args];
	ke1_t * col = (ke1_t *)&stack[top - e->n_args + 1];
	// set parameter value from the stack
	for (int j = 0; j < n - 2; ++j) {
		if (j == p->obj.matrix->size1) break;
		q = &stack[top - n + j + 2];
		gsl_matrix_set(p->obj.matrix, (size_t)j, (size_t)col->i, q->r);
	}
	return top - e->n_args;
}

static int ke_matrix_free(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->obj.matrix = NULL;
    ke_set_null_matrix(sml, p->ifield);
    --top;
    return top;
}

static int ke_matrix_set_all(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_set_all(p->obj.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_set_zero(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_set_zero(p->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_set_identity(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_set_identity(p->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_swap_rows(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_rows(p->obj.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_swap_columns(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_columns(p->obj.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_rowcol(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q, *v;
    v = &stack[--top],
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap_rowcol(p->obj.matrix, (size_t)q->i, (size_t)v->i);
    --top;
    return top;
}

static int ke_matrix_transpose_memcpy(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_transpose_memcpy(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_transpose(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    gsl_matrix_transpose(p->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_add(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_add(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_sub(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_sub(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_mul_elements(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_mul_elements(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_div_elements(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_div_elements(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_scale(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],  p = &stack[top-1];
    gsl_matrix_scale(p->obj.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_add_constant(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_add_constant(p->obj.matrix, q->r);
    --top;
    return top;
}

static int ke_matrix_memcpy(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_memcpy(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_swap(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_swap(p->obj.matrix, q->obj.matrix);
    --top;
    return top;
}

static int ke_matrix_min(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_matrix_min(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_max(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = gsl_matrix_max(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_isnull(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isnull(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_ispos(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_ispos(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_isneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isneg(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_isnonneg(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = gsl_matrix_isnonneg(p->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_equal(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    gsl_matrix_equal(p->obj.matrix, q->obj.matrix);
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    p->obj.matrix = NULL;
    return top;
}

static int ke_matrix_fscanf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_matrix_fscanf(f, p->obj.matrix);
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fprintf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_matrix_fprintf(f, p->obj.matrix,"%5g");
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fread(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "r");
    gsl_matrix_fread(f, p->obj.matrix);
    fclose(f);
    --top;
    return top;
}

static int ke_matrix_fwrite(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    p = &stack[--top],
    q = &stack[top-1];
    FILE * f = fopen(q->obj.s, "w");
    gsl_matrix_fwrite(f, p->obj.matrix);
    fclose(f);
    --top;
    return top;
}

void ke_matrix_hash(sml_t * sml) {
    ke_hash_add(sml, (fncp)&ke_matrix_alloc, MATRIX);
    ke_hash_add(sml, (fncp)&ke_matrix_alloc, MATRIX_ALLOC);
    ke_hash_add(sml, (fncp)&ke_matrix_get, MATRIX_GET);
    ke_hash_add(sml, (fncp)&ke_matrix_set, MATRIX_SET);
	ke_hash_add(sml, (fncp)&ke_matrix_put_row, MATRIX_PUT_ROW);
	ke_hash_add(sml, (fncp)&ke_matrix_put_col, MATRIX_PUT_COL);
	ke_hash_add(sml, (fncp)&ke_matrix_free, MATRIX_FREE);
    ke_hash_add(sml, (fncp)&ke_matrix_set_all, MATRIX_SET_ALL);
    ke_hash_add(sml, (fncp)&ke_matrix_set_zero,MATRIX_SET_ZERO);
    ke_hash_add(sml, (fncp)&ke_matrix_set_identity,MATRIX_SET_IDENTITY);
    ke_hash_add(sml, (fncp)&ke_matrix_swap_rows,MATRIX_SWAP_ROWS);
    ke_hash_add(sml, (fncp)&ke_matrix_swap_columns,MATRIX_SWAP_COLUMNS);
    ke_hash_add(sml, (fncp)&ke_matrix_rowcol,MATRIX_SWAP_ROWCOL);
    ke_hash_add(sml, (fncp)&ke_matrix_transpose_memcpy,MATRIX_TRANSPOSE_MEMCPY);
    ke_hash_add(sml, (fncp)&ke_matrix_transpose,MATRIX_TRANSPOSE);
    ke_hash_add(sml, (fncp)&ke_matrix_add,MATRIX_ADD);
    ke_hash_add(sml, (fncp)&ke_matrix_sub,MATRIX_SUB);
    ke_hash_add(sml, (fncp)&ke_matrix_mul_elements,MATRIX_MUL);
    ke_hash_add(sml, (fncp)&ke_matrix_mul_elements,MATRIX_MUL_ELEMENTS);
    ke_hash_add(sml, (fncp)&ke_matrix_div_elements,MATRIX_DIV);
    ke_hash_add(sml, (fncp)&ke_matrix_div_elements,MATRIX_DIV_ELEMENTS);
    ke_hash_add(sml, (fncp)&ke_matrix_scale,MATRIX_SCALE);
    ke_hash_add(sml, (fncp)&ke_matrix_add_constant,MATRIX_ADD_CONSTANT);
    ke_hash_add(sml, (fncp)&ke_matrix_memcpy,MATRIX_MEMCPY);
    ke_hash_add(sml, (fncp)&ke_matrix_swap,MATRIX_SWAP);
    ke_hash_add(sml, (fncp)&ke_matrix_min,MATRIX_MIN);
    ke_hash_add(sml, (fncp)&ke_matrix_max,MATRIX_MAX);
    ke_hash_add(sml, (fncp)&ke_matrix_isnull,MATRIX_ISNULL);
    ke_hash_add(sml, (fncp)&ke_matrix_ispos,MATRIX_ISPOS);
    ke_hash_add(sml, (fncp)&ke_matrix_isneg,MATRIX_ISNEG);
    ke_hash_add(sml, (fncp)&ke_matrix_isnonneg,MATRIX_ISNONNEG);
    ke_hash_add(sml, (fncp)&ke_matrix_equal,MATRIX_EQUAL);
    ke_hash_add(sml, (fncp)&ke_matrix_fscanf,MATRIX_FSCANF);
    ke_hash_add(sml, (fncp)&ke_matrix_fscanf,MATRIX_SCAN);
    ke_hash_add(sml, (fncp)&ke_matrix_fprintf,MATRIX_FPRINTF);
    ke_hash_add(sml, (fncp)&ke_matrix_fprintf,MATRIX_PRINT);
    ke_hash_add(sml, (fncp)&ke_matrix_fread,MATRIX_FREAD);
    ke_hash_add(sml, (fncp)&ke_matrix_fread,MATRIX_READ);
    ke_hash_add(sml, (fncp)&ke_matrix_fwrite,MATRIX_FWRITE);
    ke_hash_add(sml, (fncp)&ke_matrix_fwrite,MATRIX_WRITE);
}

void ke_matrix_print(sml_t* sml, ke1_t *k) {
    printf("Matrix: %s\n", k->name);
    for(size_t i = 0; i < k->obj.matrix->size1; i++)
        for(size_t j = 0; j < k->obj.matrix->size2; j++)
            printf("%d,%d : v:%g\n", (int)i, (int)j, gsl_matrix_get(k->obj.matrix, i,j));
}

void ke_matrix_freemem(sml_t* sml, ke1_t *e) {
    if (e->obj.matrix && e->vtype == KEV_MAT) {
        gsl_matrix_free(e->obj.matrix); ke_dec_memory(sml);
        e->obj.matrix = NULL;
    }
}
