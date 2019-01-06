#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

void ke_matrix_print(ke1_t *e);
void ke_matrix_hash();
void ke_matrix_freemem(ke1_t*e);

#define MATRIX "matrix"
#define MATRIX_ALLOC "matrix_alloc"
#define MATRIX_GET "matrix_get"
#define MATRIX_SET "matrix_set"
#define MATRIX_PUT_ROW "matrix_put_row"
#define MATRIX_PUT_COL "matrix_put_col"
#define MATRIX_FREE "matrix_free"
#define MATRIX_SET_ALL "matrix_set_all"
#define MATRIX_SET_ZERO "matrix_set_zero"
#define MATRIX_SET_IDENTITY "matrix_set_identity"
#define MATRIX_SWAP_ROWS "matrix_swap_rows"
#define MATRIX_SWAP_COLUMNS "matrix_swap_columns"
#define MATRIX_SWAP_ROWCOL "matrix_rowcol"
#define MATRIX_TRANSPOSE_MEMCPY "matrix_transpose_memcpy"
#define MATRIX_TRANSPOSE "matrix_transpose"
#define MATRIX_ADD "matrix_add"
#define MATRIX_SUB "matrix_sub"
#define MATRIX_MUL "matrix_mul"
#define MATRIX_MUL_ELEMENTS "matrix_mul_elements"
#define MATRIX_DIV "matrix_div"
#define MATRIX_DIV_ELEMENTS "matrix_div_elements"
#define MATRIX_SCALE "matrix_scale"
#define MATRIX_ADD_CONSTANT "matrix_add_constant"
#define MATRIX_MEMCPY "matrix_memcpy"
#define MATRIX_SWAP "matrix_swap"
#define MATRIX_MIN "matrix_min"
#define MATRIX_MAX "matrix_max"
#define MATRIX_ISNULL "matrix_isnull"
#define MATRIX_ISPOS "matrix_ispos"
#define MATRIX_ISNEG "matrix_isneg"
#define MATRIX_ISNONNEG "matrix_isnonneg"
#define MATRIX_EQUAL "matrix_equal"
#define MATRIX_FSCANF "matrix_fscanf"
#define MATRIX_SCAN "matrix_scan"
#define MATRIX_FPRINTF "matrix_fprintf"
#define MATRIX_PRINT "matrix_print"
#define MATRIX_FREAD "matrix_fread"
#define MATRIX_READ "matrix_read"
#define MATRIX_FWRITE "matrix_fwrite"
#define MATRIX_WRITE "matrix_write"

#endif // MATRIX_H_INCLUDED
