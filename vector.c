#include <gsl/gsl_vector.h>
#include <stdio.h>
#include "vector.h"
#include "api.h"


 static void ke_vector_alloc(sml_t* sml) {
	int n = sml_pop_int(sml);
	gsl_vector * v = gsl_vector_alloc(n);
	sml_mem_inc(sml);
	sml_push_vector(sml, v)
}

 static void ke_vector_int_alloc(sml_t* sml) { 
	int n = sml_pop_int(sml);
	gsl_vector_int * v = gsl_vector_int_alloc(n);
	sml_mem_inc(sml);
	sml_push_vector_int(sml, v);
}

 static void ke_vector_free(sml_t* sml) { 
	token_t * tokp = sml_pop_token(sml);
	void * v = sml_get_ptr(tokp);
	sml_free_ptr(sml, v);
	sml_set_ptr_null(tokp);
}

 static void ke_vector_int_free(sml_t* sml) { 
	 token_t * tokp = sml_pop_token(sml);
	 void * v = sml_get_ptr(tokp);
	 sml_free_ptr(sml, v);
	 sml_set_ptr_null(tokp);
}

void ke_vector_get(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	int i = sml_pop_int(sml);
	double r = gsl_vector_get(v, i);
	sml_push_real(sml, r);
}

void ke_vector_int_get(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int idx = sml_pop_int(sml);
	int i = gsl_vector_int_get(v, idx);
	sml_push_int(sml, i);
}

void ke_vector_prop_set(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_vector * v = sml_pop_vector(sml);
	int i = sml_pop_int(sml);
	gsl_vector_set(v, i, x);
}

void ke_vector_int_set(sml_t* sml) {
	int x = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_vector_int * v = sml_pop_vector_int(sml);
	gsl_vector_int_set(v, i, x);
}

void ke_vector_prop_int_set(sml_t* sml) { 
	int x = sml_pop_int(sml);
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = sml_pop_int(sml);
	gsl_vector_int_set(v,i,x);
}

void ke_vector_set(sml_t* sml) { 
	double x = sml_pop_real(sml);
	int i = sml_pop_int(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_set(v,i,x);
}

void ke_vector_put(sml_t* sml) { 
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);
	gsl_vector * v = sml_peek_vector(sml, (top - n));
	for (size_t i = 0; i < n - 1; ++i) {
		if (i == v->size) break;
		double x = sml_peek_real(sml, (top - n + i + 1));
		gsl_vector_set(v, i, x);
	}
	top = top - n;
	sml_set_top(sml, top);
}

static void ke_vector_int_put(sml_t* sml) { 
	int top = sml_get_top(sml);
	int n = sml_get_args(sml);
	gsl_vector_int * v = sml_peek_vector_int(sml, (top - n));
	for (size_t i = 0; i < n - 1; ++i) {
		if (i == v->size) break;
		int x = sml_peek_int(sml, (top - n + i + 1));
		gsl_vector_int_set(v, i, x);
	}
	top = top - n;
	sml_set_top(sml, top);
}

 static void ke_vector_set_all(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_set_all(v,x);
}

 static void ke_vector_int_set_all(sml_t* sml) { 
	 int x = sml_pop_int(sml);
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 gsl_vector_int_set_all(v, x);
}

 static void ke_vector_set_zero(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_set_zero(v);
}

 static void ke_vector_int_set_zero(sml_t* sml) {
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 gsl_vector_int_set_zero(v);
}

 static void ke_vector_set_basis(sml_t* sml) {
	int i = sml_pop_int(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_set_basis(v, i);
}

 static void ke_vector_int_set_basis(sml_t* sml) {
	 int i = sml_pop_int(sml);
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 gsl_vector_int_set_basis(v, i);
}

 static void ke_vector_add(sml_t* sml) { 
	gsl_vector * b = sml_pop_vector(sml);
	gsl_vector * a = sml_pop_vector(sml);
	gsl_vector_add(a,b);
}

 static void ke_vector_int_add(sml_t* sml) {
	gsl_vector_int * b = sml_pop_vector_int(sml);
	gsl_vector_int * a = sml_pop_vector_int(sml);
	gsl_vector_int_add(a, b);
}

 static void ke_vector_sub(sml_t* sml) { 
	gsl_vector * b = sml_pop_vector(sml);
	gsl_vector * a = sml_pop_vector(sml);
	gsl_vector_sub(a, b);
}

 static void ke_vector_int_sub(sml_t* sml) { 
	gsl_vector_int * b = sml_pop_vector_int(sml);
	gsl_vector_int * a = sml_pop_vector_int(sml);
	gsl_vector_int_sub(a, b);
}

 static void ke_vector_mul(sml_t* sml) { 
	gsl_vector * b = sml_pop_vector(sml);
	gsl_vector * a = sml_pop_vector(sml);
	gsl_vector_mul(a, b);
}

 static void ke_vector_int_mul(sml_t* sml) { 
	gsl_vector_int * b = sml_pop_vector_int(sml);
	gsl_vector_int * a = sml_pop_vector_int(sml);
	gsl_vector_int_mul(a, b);
 }

 static void ke_vector_div(sml_t* sml) { 
	gsl_vector * b = sml_pop_vector(sml);
	gsl_vector * a = sml_pop_vector(sml);
	gsl_vector_div(a, b);
}

 static void ke_vector_int_div(sml_t* sml) { 
	gsl_vector_int * b = sml_pop_vector_int(sml);
	gsl_vector_int * a = sml_pop_vector_int(sml);
	gsl_vector_int_div(a, b);
}

 static void ke_vector_scale(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_scale(v,x);
}

 static void ke_vector_int_scale(sml_t* sml) { 
	 double x = sml_pop_real(sml);
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 gsl_vector_int_scale(v, x);
}

 static void ke_vector_add_constant(sml_t* sml) { 
	double x = sml_pop_real(sml);
	gsl_vector * a = sml_pop_vector(sml);
	gsl_vector_add_constant(a,x);
}

 static void ke_vector_int_add_constant(sml_t* sml) { 
	 double x = sml_pop_real(sml);
	 gsl_vector_int * a = sml_pop_vector_int(sml);
	 gsl_vector_int_add_constant(a, x);
}

 static void ke_vector_reverse(sml_t* sml) {
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_reverse(v);
}

 static void ke_vector_int_reverse(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	gsl_vector_int_reverse(v);
}

 static void ke_vector_swap_elements(sml_t* sml) { 
	int j = sml_pop_int(sml);
	int i = sml_pop_int(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_swap_elements(v,i,j);
}

 static void ke_vector_int_swap_elements(sml_t* sml) {
	 int j = sml_pop_int(sml);
	 int i = sml_pop_int(sml);
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 gsl_vector_int_swap_elements(v, i, j);
}

 static void ke_vector_memcpy(sml_t* sml) { 
	gsl_vector * src = sml_pop_vector(sml);
	gsl_vector * dest = sml_pop_vector(sml);
	gsl_vector_memcpy(dest, src);
}

 static void ke_vector_int_memcpy(sml_t* sml) { 
	gsl_vector_int * src = sml_pop_vector_int(sml);
	gsl_vector_int * dest = sml_pop_vector_int(sml);
	gsl_vector_int_memcpy(dest, src);
}

 static void ke_vector_swap(sml_t* sml) { 
	gsl_vector * w = sml_pop_vector(sml);
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector_swap(v,w);
}

 static void ke_vector_int_swap(sml_t* sml) { 
	gsl_vector_int * w = sml_pop_vector_int(sml);
	gsl_vector_int * v = sml_pop_vector_int(sml);
	gsl_vector_int_swap(v, w);
}

 static void ke_vector_min(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	double r = gsl_vector_min(v);
	sml_push_real(sml,r);
}

 static void ke_vector_int_min(sml_t* sml) { 
	 gsl_vector_int * v = sml_pop_vector_int(sml);
	 int i = gsl_vector_int_min(v);
	 sml_push_int(sml, i);
}

 static void ke_vector_max(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	double r = gsl_vector_max(v);
	sml_push_real(sml, r);
}

 static void ke_vector_int_max(sml_t* sml) {
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = gsl_vector_int_max(v);
	sml_push_int(sml, i);
}

 static void ke_vector_isnull(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	int i = gsl_vector_isnull(v);
	sml_push_int(sml, i);
}

 static void ke_vector_int_isnull(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = gsl_vector_int_isnull(v);
	sml_push_int(sml, i);
}

 static void ke_vector_ispos(sml_t* sml) {
	gsl_vector * v = sml_pop_vector(sml);
	int i = gsl_vector_ispos(v);
	sml_push_int(sml, i);
}

 static void ke_vector_int_ispos(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = gsl_vector_int_ispos(v);
	sml_push_int(sml, i);
}

 static void ke_vector_isneg(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	int i = gsl_vector_isneg(v);
	sml_push_int(sml, i);
}

 static void ke_vector_int_isneg(sml_t* sml) {
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = gsl_vector_int_isneg(v);
	sml_push_int(sml, i);
}

 static void ke_vector_isnonneg(sml_t* sml) {
	gsl_vector * v = sml_pop_vector(sml);
	int i = gsl_vector_isnonneg(v);
	sml_push_int(sml, i);
}

 static void ke_vector_int_isnonneg(sml_t* sml) {
	gsl_vector_int * v = sml_pop_vector_int(sml);
	int i = gsl_vector_int_isnonneg(v);
	sml_push_int(sml, i);
}

 static void ke_vector_equal(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	gsl_vector * u = sml_pop_vector(sml);
	int i  = gsl_vector_equal(u,v);
	sml_push_int(sml, i);
 }

 static void ke_vector_int_equal(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	gsl_vector_int * u = sml_pop_vector_int(sml);
	int i = gsl_vector_int_equal(u, v);
	sml_push_int(sml, i);
}

 static void ke_vector_fscanf(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	char * filename = sml_pop_str(sml);
    FILE * f = fopen(filename, "r");
    gsl_vector_fscanf(f, v);
    fclose(f);
}

 static void ke_vector_int_fscanf(sml_t* sml) {
	gsl_vector_int * v = sml_pop_vector_int(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "r");
	gsl_vector_int_fscanf(f, v);
	fclose(f);
}

 static void ke_vector_fprintf(sml_t* sml) {
	gsl_vector * v = sml_pop_vector(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
    gsl_vector_fprintf(f, v,"%5g");
    fclose(f);
}

 static void ke_vector_int_fprintf(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
	gsl_vector_int_fprintf(f, v, "%5g");
	fclose(f);
}

 static void ke_vector_fread(sml_t* sml) {
	gsl_vector * v = sml_pop_vector(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "r");
	gsl_vector_fread(f, v);
	fclose(f);
}

 static void ke_vector_int_fread(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "r");
	gsl_vector_int_fread(f, v);
	fclose(f);
}

 static void ke_vector_fwrite(sml_t* sml) { 
	gsl_vector * v = sml_pop_vector(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
    gsl_vector_fwrite(f, v);
    fclose(f);
}

 static void ke_vector_int_fwrite(sml_t* sml) { 
	gsl_vector_int * v = sml_pop_vector_int(sml);
	char * filename = sml_pop_str(sml);
	FILE * f = fopen(filename, "w");
	gsl_vector_int_fwrite(f, v);
	fclose(f);
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

void ke_vector_print(sml_t* sml, token_t *k) {
    printf("Vector: %s\n", k->name);
    for(size_t i = 0; i < k->obj.vector->size; i++) {
        printf("%d : v:%g\n", (int)i, gsl_vector_get(k->obj.vector, i));
    }
}

void ke_vector_freemem(sml_t* sml,token_t *e) {
    if (e->obj.vector && e->vtype == KEV_VEC) {
        gsl_vector_free(e->obj.vector); ke_dec_memory(sml);
        e->obj.vector = NULL;
    }
}

void ke_vector_int_print(sml_t* sml,token_t *k) {
	printf("Vector: %s\n", k->name);
	for (size_t i = 0; i < k->obj.vector_int->size; i++) {
		printf("%d : v:%d\n", (int)i, gsl_vector_int_get(k->obj.vector_int, i));
	}
}

void ke_vector_int_freemem(sml_t* sml,token_t *e) {
	if (e->obj.vector_int && e->vtype == KEV_VEC_INT) {
		gsl_vector_int_free(e->obj.vector_int); ke_dec_memory(sml);
		e->obj.vector_int = NULL;
	}
}
