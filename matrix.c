#include <gsl/gsl_matrix.h>
#include "kexpr.h"
#include "matrix.h"
#include "api.c"

static void ke_matrix_alloc(sml_t* sml) { 
	int n1 = sml_pop_int(sml);
	int n2 = sml_pop_int(sml);
	gsl_matrix * m = gsl_matrix_alloc(n1,n2);
	sml_mem_inc(sml);
	sml_push_matrix(sml, m)
}

void ke_matrix_prop_get(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	double r = gsl_matrix_get(m, i, j);
	sml_push_real(sml, r);
}

void ke_matrix_get(sml_t* sml) { 
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	double r = gsl_matrix_get(m,i,j);
	sml_push_real(sml, r);
}

void ke_matrix_prop_set(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_matrix_set(m, i, j, x);
}

static void ke_matrix_set(sml_t* sml) { 
	double x = sml_pop_real(sml);
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_set(m,i, j, x);
}

static void ke_matrix_put_row(sml_t* sml) { 
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);
	gsl_matrix * m = sml_peek_matrix(sml,(top-n));
	int i = sml_peek_int(sml,(top-n+1));
	for (size_t j = 0; j < n - 2; ++j) {
		if (j == m->size2) break;
		double x = sml_peek_real(sml, (top - n + j + 2));
		gsl_matrix_set(m, i, j, x);
	}

	top = top - n;
	sml_set_top(sml, top);
}

static void ke_matrix_put_col(sml_t* sml) { 
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);
	gsl_matrix * m = sml_peek_matrix(sml, (top - n));
	int j = sml_peek_int(sml, (top - n + 1));
	for (size_t i = 0; i < n - 2; ++i) {
		if (i == m->size2) break;
		double x = sml_peek_real(sml, (top - n + i + 2));
		gsl_matrix_set(m, i, j, x);
	}
	top = top - n;
	sml_set_top(sml, top);
}

static void ke_matrix_free(sml_t* sml) { 
	token_t * tokp = sml_pop_token(sml);
	void * m = sml_get_ptr(tokp);
	sml_free_ptr(sml, m);
	sml_set_ptr_null(tokp);
}

static void ke_matrix_set_all(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
    gsl_matrix_set_all(m,x);
}

static void ke_matrix_set_zero(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_set_zero(m);
}

static void ke_matrix_set_identity(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
    gsl_matrix_set_identity(m);
}

static void ke_matrix_swap_rows(sml_t* sml) { 
	int j = sml_pop_int(sml); 
	int i = sml_pop_int(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_swap_rows(m, i, j);
}

static void ke_matrix_swap_columns(sml_t* sml) { 
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_swap_columns(m,i,j);
}

static void ke_matrix_rowcol(sml_t* sml) { 
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_swap_rowcol(m,i,j);
}

static void ke_matrix_transpose_memcpy(sml_t* sml) { 
	gsl_matrix * src = sml_pop_matrix(sml);
	gsl_matrix * dest = sml_pop_matrix(sml);
    gsl_matrix_transpose_memcpy(dest, src);
}

static void ke_matrix_transpose(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_transpose(m);
}

static void ke_matrix_add(sml_t* sml) { 
	gsl_matrix * b = sml_pop_matrix(sml);
	gsl_matrix * a = sml_pop_matrix(sml);
    gsl_matrix_add(a,b);
}

static void ke_matrix_sub(sml_t* sml) { 
	gsl_matrix * b = sml_pop_matrix(sml);
	gsl_matrix * a = sml_pop_matrix(sml);
    gsl_matrix_sub(a,b);
}

static void ke_matrix_mul_elements(sml_t* sml) { 
	gsl_matrix * b = sml_pop_matrix(sml);
	gsl_matrix * a = sml_pop_matrix(sml);
	gsl_matrix_mul_elements(a,b);
}

static void ke_matrix_div_elements(sml_t* sml) { 
	gsl_matrix * b = sml_pop_matrix(sml);
	gsl_matrix * a = sml_pop_matrix(sml);
	gsl_matrix_div_elements(a,b);
}

static void ke_matrix_scale(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
    gsl_matrix_scale(m,x);
}

static void ke_matrix_add_constant(sml_t* sml) {
	double x = sml_pop_real(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	gsl_matrix_add_constant(m, x);
}

static void ke_matrix_memcpy(sml_t* sml) { 
	gsl_matrix * src = sml_pop_matrix(sml);
	gsl_matrix * dest = sml_pop_matrix(sml);
    gsl_matrix_memcpy(dest,src);
}

static void ke_matrix_swap(sml_t* sml) { 
	gsl_matrix * m2 = sml_pop_matrix(sml);
	gsl_matrix * m1 = sml_pop_matrix(sml);
    gsl_matrix_swap(m1,m2);
}

static void ke_matrix_min(sml_t* sml) {
	gsl_matrix * m = sml_pop_matrix(sml);
	double r = gsl_matrix_min(m);
	sml_push_real(sml, r);
}

static void ke_matrix_max(sml_t* sml) {
	gsl_matrix * m = sml_pop_matrix(sml);
	double r = gsl_matrix_max(m);
	sml_push_real(sml, r);
}

static void ke_matrix_isnull(sml_t* sml) {
	gsl_matrix * m = sml_pop_matrix(sml);
	int i = gsl_matrix_isnull(m);
	sml_push_int(sml, i);
}

static void ke_matrix_ispos(sml_t* sml) {
	gsl_matrix * m = sml_pop_matrix(sml);
	int i = gsl_matrix_ispos(m);
	sml_push_int(sml, i);
}

static void ke_matrix_isneg(sml_t* sml) {
	gsl_matrix * m = sml_pop_matrix(sml);
	int i = gsl_matrix_isneg(m);
	sml_push_int(sml, i);
}

static void ke_matrix_isnonneg(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	int i = gsl_matrix_isnonneg(m);
	sml_push_int(sml, i);
}

static void ke_matrix_equal(sml_t* sml) {
	gsl_matrix * b = sml_pop_matrix(sml);
	gsl_matrix * a = sml_pop_matrix(sml);
	int i = gsl_matrix_equal(a,b);
	sml_push_int(sml, i);
}

static void ke_matrix_fscanf(sml_t* sml) {
	char * filename = sml_pop_str(sml);
	gsl_matrix * m = sml_pop_matrix(sml);
	FILE * f = fopen(filename, "r");
    gsl_matrix_fscanf(f, m);
    fclose(f);
}

static void ke_matrix_fprintf(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
    gsl_matrix_fprintf(f, m,"%5g");
    fclose(f);
}

static void ke_matrix_fread(sml_t* sml) { 
	gsl_matrix * m = sml_pop_matrix(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "r");
    gsl_matrix_fread(f, m);
    fclose(f);
}

static void ke_matrix_fwrite(sml_t* sml) { 
ke1_t **stack = sml->stack;
	gsl_matrix * m = sml_pop_matrix(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
    gsl_matrix_fwrite(f, m);
    fclose(f);
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
