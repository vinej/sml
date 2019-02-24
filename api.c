#include "kexpr.h"

#define token_t ke1_t

#define sml_get_top(sml) sml->top
#define sml_set_top(sml,top) sml->top=top
#define sml_get_stack(sml) sml->stack
#define sml_get_tokp(sml) sml->tokp
#define sml_get_args(sml) sml->tokp->n_args

#define sml_pop_token(sml) sml->stack[--sml->top]
#define sml_pop_int(sml) sml->stack[--sml->top]->i
#define sml_pop_real(sml) sml->stack[--sml->top]->r
#define sml_pop_ptr(sml) sml->stack[--sml->top]->obj.ptr
#define sml_pop_file(sml) sml->stack[--sml->top]->obj.file
#define sml_pop_str(sml) sml->stack[--sml->top]->obj.s
#define sml_pop_date(sml) sml->stack[--sml->top]->obj.date
#define sml_pop_complex(sml) sml->stack[--sml->top]->obj.tcomplex
#define sml_pop_complex_adr(sml) &sml->stack[--sml->top]->obj.tcomplex
#define sml_pop_day(sml) sml->stack[--sml->top]->i
#define sml_pop_month(sml) sml->stack[--sml->top]->i
#define sml_pop_year(sml) sml->stack[--sml->top]->i
#define sml_pop_matrix(sml) sml->stack[--sml->top]->obj.matrix
#define sml_pop_vector(sml) sml->stack[--sml->top]->obj.vector
#define sml_pop_vector_int(sml) sml->stack[--sml->top]->obj.vector_int

#define sml_peek_token(sml,i) sml->stack[i]
#define sml_peek_int(sml,ii) sml->stack[ii]->i
#define sml_peek_real(sml,i) sml->stack[i]->r
#define sml_peek_ptr(sml,i) sml->stack[i]->obj.ptr
#define sml_peek_file(sml,i) sml->stack[i]->obj.file
#define sml_peek_str(sml,i) sml->stack[i]->obj.s
#define sml_peek_date(sml,i) sml->stack[i]->obj.date
#define sml_peek_complex(sml,i) sml->stack[i]->obj.tcomplex
#define sml_peek_complex_adr(sml,i) &sml->stack[i]->obj.tcomplex
#define sml_peek_day(sml,i) (GDateDay)sml->stack[i]->i
#define sml_peek_month(sml,i) (GDateMonth)sml->stack[i]->i
#define sml_peek_year(sml,i) (GDateYear)sml->stack[i]->i
#define sml_peek_matrix(sml,i) sml->stack[i]->obj.matrix
#define sml_peek_vector(sml,i) sml->stack[i]->obj.vector
#define sml_peek_vector_int(sml,i) sml->stack[i]->obj.vector_int

#define sml_get_complex_adr  &out->obj.tcomplex

#define sml_get_str(t) t->obj.s
#define sml_get_real(t) t->r
#define sml_get_int(t) t->i
#define sml_get_ptr(t) t->obj.ptr
#define sml_set_ptr_null(t) t->obj.ptr = NULL

#define sml_adr_str(t) &t->obj.s
#define sml_adr_ptr(t) &t->obj



#define sml_push_buffer(sml, b) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.buffer = b; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_BUFFER; \
	out->assigned = 1;

#define sml_push_file(sml, file) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.file = file; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_FILE; \
	out->assigned = 1;

#define sml_push_new_complex(sml) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_COMPLEX; \
	out->assigned = 1;

#define sml_push_complex(sml, z) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.tcomplex = z; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_COMPLEX; \
	out->assigned = 1;

#define sml_push_real(sml, r) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->r = r; \
	out->i = (int64_t)r; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_REAL; \
	out->assigned = 1;

#define sml_push_date(sml, dt) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.date = dt; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_DATE; \
	out->assigned = 1;

#define  sml_push_int(sml, i) ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->i = i; \
	out->r = (double)i; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_INT; \
	out->assigned = 1;

#define  sml_push_ptr(sml, ptr) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.ptr = ptr; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_PTR; \
	out->assigned = 1; 

#define  sml_push_str(sml, str) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.s = str; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_STR; \
	out->assigned = 1; 

#define  sml_push_matrix(sml, m) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.matrix = m; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_MAT; \
	out->assigned = 1; 

#define  sml_push_vector(sml, v) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.vector = v; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_VEC; \
	out->assigned = 1; 

#define  sml_push_vector_int(sml, v) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.vector_int = v; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_VEC_INT; \
	out->assigned = 1; 

#define  sml_push_image(sml, image) \
	ke1_t* out; \
	sml->stack[sml->top] = ke_get_out(sml); \
	out = sml->stack[sml->top++]; \
	out->obj.image = image; \
	out->ttype = KET_VAL; \
	out->vtype = KEV_IMAGE; \
	out->assigned = 1; 

#define sml_out_date out->obj.date

#define sml_free_ptr(sml,ptr) ke_free_memory(sml,ptr); ke_dec_memory(sml); ptr = NULL

#define sml_new_ptr(sml,size) ke_calloc_memory(sml, size,1)

#define sml_mem_inc ke_inc_memory

#define sml_save_str(sml, tokp, tmp) \
	if (tokp->vtype == KEV_STR && tokp->obj.s) { \
		ke_free_memory(sml, tokp->obj.s); \
	} \
	tokp->obj.s = tmp; \
	tokp->i = 0, tokp->r = 0, tokp->assigned = 1; \
	tokp->vtype = KEV_STR;

