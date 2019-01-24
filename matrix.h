#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

void ke_matrix_print(ke1_t *e);
void ke_matrix_hash();
void ke_matrix_freemem(ke1_t*e);
int ke_matrix_get(ke1_t *stack, int top);
int ke_matrix_prop_set(ke1_t *stack, int top);


#define MATRIX "matrix"
#define MATRIX_ALLOC "malloc"
#define MATRIX_GET "mget"
#define MATRIX_SET "mset"
#define MATRIX_PUT_ROW "mput_row"
#define MATRIX_PUT_COL "mput_col"
#define MATRIX_FREE "mfree"
#define MATRIX_SET_ALL "mset_all"
#define MATRIX_SET_ZERO "mset_zero"
#define MATRIX_SET_IDENTITY "mset_identity"
#define MATRIX_SWAP_ROWS "mswap_rows"
#define MATRIX_SWAP_COLUMNS "mswap_columns"
#define MATRIX_SWAP_ROWCOL "mrowcol"
#define MATRIX_TRANSPOSE_MEMCPY "mtranspose_memcpy"
#define MATRIX_TRANSPOSE "mtranspose"
#define MATRIX_ADD "madd"
#define MATRIX_SUB "msub"
#define MATRIX_MUL "mmul"
#define MATRIX_MUL_ELEMENTS "mmul_elements"
#define MATRIX_DIV "mdiv"
#define MATRIX_DIV_ELEMENTS "mdiv_elements"
#define MATRIX_SCALE "mscale"
#define MATRIX_ADD_CONSTANT "madd_constant"
#define MATRIX_MEMCPY "mmemcpy"
#define MATRIX_SWAP "mswap"
#define MATRIX_MIN "mmin"
#define MATRIX_MAX "mmax"
#define MATRIX_ISNULL "misnull"
#define MATRIX_ISPOS "mispos"
#define MATRIX_ISNEG "misneg"
#define MATRIX_ISNONNEG "misnonneg"
#define MATRIX_EQUAL "mequal"
#define MATRIX_FSCANF "mfscanf"
#define MATRIX_SCAN "mscan"
#define MATRIX_FPRINTF "mfprintf"
#define MATRIX_PRINT "mprint"
#define MATRIX_FREAD "mfread"
#define MATRIX_READ "mread"
#define MATRIX_FWRITE "mfwrite"
#define MATRIX_WRITE "mwrite"

#endif // MATRIX_H_INCLUDED
