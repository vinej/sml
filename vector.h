#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

void ke_vector_print(ke1_t *k);
void ke_vector_hash();
void ke_vector_freemem(ke1_t *e);

void ke_vector_int_print(ke1_t *k);
void ke_vector_int_freemem(ke1_t *e);

int ke_vector_get(ke1_t *stack, int top);
int ke_vector_int_get(ke1_t *stack, int top);
int ke_vector_set(ke1_t *stack, int top);
int ke_vector_int_set(ke1_t *stack, int top);
int ke_vector_put(ke1_t *stack, int top);
int ke_vector_prop_set(ke1_t *stack, int top);



#define VECTOR "vector"
#define VECTOR_ALLOC "valloc"
#define VECTOR_SGET "vg"
#define VECTOR_SSET "vs"
#define VECTOR_GET "vget"
#define VECTOR_SET "vset"
#define VECTOR_PUT "vput"
#define VECTOR_FREE "vfree"
#define VECTOR_SET_ALL "vset_all"
#define VECTOR_SET_ZERO "vset_zero"
#define VECTOR_SET_BASIS "vset_basis"
#define VECTOR_ADD "vadd"
#define VECTOR_SUB "vsub"
#define VECTOR_MUL "vmul"
#define VECTOR_DIV "vdiv"
#define VECTOR_SCALE "vscale"
#define VECTOR_ADD_CONSTANT "vadd_constant"
#define VECTOR_REVERSE "vreverse"
#define VECTOR_SWAP_ELEMENTS "vswap_elements"
#define VECTOR_MEMCPY "vmemcpy"
#define VECTOR_SWAP "vswap"
#define VECTOR_MIN "vmin"
#define VECTOR_MAX "vmax"
#define VECTOR_ISNULL "visnull"
#define VECTOR_ISPOS "vispos"
#define VECTOR_ISNEG "visneg"
#define VECTOR_ISNONNEG "visnonneg"
#define VECTOR_EQUAL "vequal"
#define VECTOR_FSCANF "vfscanf"
#define VECTOR_SCAN "vscan"
#define VECTOR_FPRINTF "vfprintf"
#define VECTOR_PRINT "vprint"
#define VECTOR_FREAD "vfread"
#define VECTOR_READ "vread"
#define VECTOR_FWRITE "vfwrite"
#define VECTOR_WRITE "vwrite"

#define VECTOR_INT "vector_int"
#define VECTOR_INT_ALLOC "vi_alloc"
#define VECTOR_INT_GET "vi_get"
#define VECTOR_INT_SET "vi_set"
#define VECTOR_INT_PUT "vi_put"
#define VECTOR_INT_FREE "vi_free"
#define VECTOR_INT_SET_ALL "vi_set_all"
#define VECTOR_INT_SET_ZERO "vi_set_zero"
#define VECTOR_INT_SET_BASIS "vi_set_basis"
#define VECTOR_INT_ADD "vi_add"
#define VECTOR_INT_SUB "vi_sub"
#define VECTOR_INT_MUL "vi_mul"
#define VECTOR_INT_DIV "vi_div"
#define VECTOR_INT_SCALE "vi_scale"
#define VECTOR_INT_ADD_CONSTANT "vi_add_constant"
#define VECTOR_INT_REVERSE "vi_reverse"
#define VECTOR_INT_SWAP_ELEMENTS "vi_swap_elements"
#define VECTOR_INT_MEMCPY "vi_memcpy"
#define VECTOR_INT_SWAP "vi_swap"
#define VECTOR_INT_MIN "vi_min"
#define VECTOR_INT_MAX "vi_max"
#define VECTOR_INT_ISNULL "vi_isnull"
#define VECTOR_INT_ISPOS "vi_ispos"
#define VECTOR_INT_ISNEG "vi_isneg"
#define VECTOR_INT_ISNONNEG "vi_isnonneg"
#define VECTOR_INT_EQUAL "vi_equal"
#define VECTOR_INT_FSCANF "vi_fscanf"
#define VECTOR_INT_SCAN "vi_scan"
#define VECTOR_INT_FPRINTF "vi_fprintf"
#define VECTOR_INT_PRINT "vi_print"
#define VECTOR_INT_FREAD "vi_fread"
#define VECTOR_INT_READ "vi_read"
#define VECTOR_INT_FWRITE "vi_fwrite"
#define VECTOR_INT_WRITE "vi_write"

#endif // VECTOR_H_INCLUDED
