#ifndef BUILDIN_H_INCLUDED
#define BUILDIN_H_INCLUDED
#include "kexpr.h"

void ke_op_KEO_LT(ke1_t *p, ke1_t *q);
void ke_op_KEO_LE(ke1_t *p, ke1_t *q);
void ke_op_KEO_GT(ke1_t *p, ke1_t *q);
void ke_op_KEO_GE(ke1_t *p, ke1_t *q);
void ke_op_KEO_EQ(ke1_t *p, ke1_t *q);
void ke_op_KEO_NE(ke1_t *p, ke1_t *q);
void ke_op_KEO_BAND(ke1_t *p, ke1_t *q);
void ke_op_KEO_BOR(ke1_t *p, ke1_t *q);
void ke_op_KEO_BXOR(ke1_t *p, ke1_t *q);
void ke_op_KEO_LSH(ke1_t *p, ke1_t *q);
void ke_op_KEO_RSH(ke1_t *p, ke1_t *q);
void ke_op_KEO_MOD(ke1_t *p, ke1_t *q);
void ke_op_KEO_IDIV(ke1_t *p, ke1_t *q);
void ke_op_KEO_ADD(ke1_t *p, ke1_t *q);
void ke_op_KEO_SUB(ke1_t *p, ke1_t *q);
void ke_op_KEO_MUL(ke1_t *p, ke1_t *q);
void ke_op_KEO_DIV(ke1_t *p, ke1_t *q);
void ke_op_KEO_LAND(ke1_t *p, ke1_t *q);
void ke_op_KEO_LOR(ke1_t *p, ke1_t *q);
void ke_op_KEO_POW(ke1_t *p, ke1_t *q);
void ke_op_KEO_BNOT(ke1_t *p, ke1_t *q);
void ke_op_KEO_LNOT(ke1_t *p, ke1_t *q);
void ke_op_KEO_POS(ke1_t *p, ke1_t *q);
void ke_op_KEO_NOP(ke1_t *p, ke1_t *q);
void ke_op_KEO_NEG(ke1_t *p, ke1_t *q);
void ke_func1_abs(ke1_t *p, ke1_t *q);

#endif // BUILDIN_H_INCLUDED
