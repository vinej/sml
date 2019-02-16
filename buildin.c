#include "buildin.h"

void ke_op_KEO_LT(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR)	{
		p->i = (strcmp(p->obj.s, q->obj.s) < 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r < q->r) : (p->i < q->i);
	}
	p->r = (double)p->i; 
	p->vtype = KEV_INT; 
}

void ke_op_KEO_LE(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR) {
		p->i = (strcmp(p->obj.s, q->obj.s) <= 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r <= q->r) : (p->i <= q->i);
	}
	p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_GT(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR) {
		p->i = (strcmp(p->obj.s, q->obj.s) > 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r > q->r) : (p->i > q->i);
	}
	p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_GE(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR) {
		p->i = (strcmp(p->obj.s, q->obj.s) >= 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r >= q->r) : (p->i >= q->i);
	}
	p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_EQ(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR) {
		p->i = (strcmp(p->obj.s, q->obj.s) == 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r == q->r) : (p->i == q->i);
	}
	p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_NE(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_STR && q->vtype == KEV_STR) {
		p->i = (strcmp(p->obj.s, q->obj.s) != 0);
	}
	else {
		p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? (p->r != q->r) : (p->i != q->i);
	}
	p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_BAND(ke1_t *p, ke1_t *q) { 
	p->i &= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_BOR(ke1_t *p, ke1_t *q) {
	p->i |= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_BXOR(ke1_t *p, ke1_t *q) {
	p->i ^= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_LSH(ke1_t *p, ke1_t *q) {
	p->i <<= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_RSH(ke1_t *p, ke1_t *q) {
	p->i >>= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_MOD(ke1_t *p, ke1_t *q) {
	p->i %= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_IDIV(ke1_t *p, ke1_t *q) {
	p->i /= q->i; p->r = (double)p->i;
	p->vtype = KEV_INT;
}

void ke_op_KEO_ADD(ke1_t *p, ke1_t *q) { 
	p->i += q->i; p->r += q->r; 
	p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL? KEV_REAL : KEV_INT; 
}

void ke_op_KEO_SUB(ke1_t *p, ke1_t *q) {
	p->i -= q->i; p->r -= q->r;
	p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT;
}

void ke_op_KEO_MUL(ke1_t *p, ke1_t *q) {
	p->i *= q->i; p->r *= q->r;
	p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT;
}

void ke_op_KEO_DIV(ke1_t *p, ke1_t *q) { 
	p->r /= q->r;
	p->i = (int64_t)(p->r + .5); 
	p->vtype = KEV_REAL;
}

void ke_op_KEO_LAND(ke1_t *p, ke1_t *q) { 
	p->i = (p->i && q->i);
	p->r = (double)p->i;
	p->vtype = KEV_INT; 
}

void ke_op_KEO_LOR(ke1_t *p, ke1_t *q) { 
	p->i = (p->i || q->i); 
	p->r = (double)p->i; 
	p->vtype = KEV_INT; 
}


void ke_op_KEO_POW(ke1_t *p, ke1_t *q) { 
	p->r = pow(p->r, q->r);
	p->i = (int64_t)(p->r + .5); 
	p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT; 
}

void ke_op_KEO_BNOT(ke1_t *p, ke1_t *q) 
{ 
	p->i = ~p->i; 
	p->r = (double)p->i; 
	p->vtype = KEV_INT; 
}

void ke_op_KEO_LNOT(ke1_t *p, ke1_t *q) { 
	p->i = !p->i;
	p->r = (double)p->i;
	p->vtype = KEV_INT; 
}

void ke_op_KEO_POS(ke1_t *p, ke1_t *q) 
{ } // do nothing

void ke_op_KEO_NOP(ke1_t *p, ke1_t *q) 
{ } // do nothing

void ke_op_KEO_NEG(ke1_t *p, ke1_t *q) 
{ 
	p->i = -p->i;
	p->r = -p->r;
}

void ke_func1_abs(ke1_t *p, ke1_t *q) {
	if (p->vtype == KEV_INT) {
		p->i = (int64_t)abs((int)p->i);
		p->r = (double)p->i;
	}
	else {
		p->r = fabs(p->r);
		p->i = (int64_t)(p->r + .5);
	}
}
