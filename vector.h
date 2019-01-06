#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

void ke_vector_print(ke1_t *k);
void ke_vector_hash();
void ke_vector_freemem(ke1_t *e);

#define VECTOR "vector"
#define VECTOR_ALLOC "vector_alloc"
#define VECTOR_GET "vector_get"
#define VECTOR_SET "vector_set"
#define VECTOR_PUT "vector_put"
#define VECTOR_FREE "vector_free"
#define VECTOR_SET_ALL "vector_set_all"
#define VECTOR_SET_ZERO "vector_set_zero"
#define VECTOR_SET_BASIS "vector_set_basis"
#define VECTOR_ADD "vector_add"
#define VECTOR_SUB "vector_sub"
#define VECTOR_MUL "vector_mul"
#define VECTOR_DIV "vector_div"
#define VECTOR_SCALE "vector_scale"
#define VECTOR_ADD_CONSTANT "vector_add_constant"
#define VECTOR_REVERSE "vector_reverse"
#define VECTOR_SWAP_ELEMENTS "vector_swap_elements"
#define VECTOR_MEMCPY "vector_memcpy"
#define VECTOR_SWAP "vector_swap"
#define VECTOR_MIN "vector_min"
#define VECTOR_MAX "vector_max"
#define VECTOR_ISNULL "vector_isnull"
#define VECTOR_ISPOS "vector_ispos"
#define VECTOR_ISNEG "vector_isneg"
#define VECTOR_ISNONNEG "vector_isnonneg"
#define VECTOR_EQUAL "vector_equal"
#define VECTOR_FSCANF "vector_fscanf"
#define VECTOR_SCAN "vector_scan"
#define VECTOR_FPRINTF "vector_fprintf"
#define VECTOR_PRINT "vector_print"
#define VECTOR_FREAD "vector_fread"
#define VECTOR_READ "vector_read"
#define VECTOR_FWRITE "vector_fwrite"
#define VECTOR_WRITE "vector_write"

#endif // VECTOR_H_INCLUDED
