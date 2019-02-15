#include <math.h>
#include "khash.h"
#include "kexpr.h"
#include "function.h"
#include "time.h"

#define RADIAN 1
#define DEGREE 2
#define GRADIAN  3
static int anglem = RADIAN;


static double convert_anglem(double value) {
	switch (anglem)
	{
	case RADIAN:
		return value;
	case DEGREE:
		return value * 180 / M_PI;
	case GRADIAN:
		return value * 200 / M_PI;
	default:
		return value;
	}
}

static int ke_function_anglem(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	out = stack[--top];
	anglem = (int)p->i;
	return top;
}

static int ke_function_exp(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top-1];
	out->r = exp(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
    return top;
}

static int ke_function_pow(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top],
    p = stack[--top];
	out = stack[top - 1];
	out->r = pow(p->r, q->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
    return top;
}

static int ke_function_log(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out,*p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = log(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_log10(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out,*p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = log10(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_sqrt(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out,*p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = sqrt(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_sin(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
    out->r = convert_anglem(sin(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_cos(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(cos(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_tan(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(tan(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_floor(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = floor(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_ceil(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
    out->r = ceil(p->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acos(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(acos(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_asin(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(asin(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_atan(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(atan(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_cosh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(cosh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_sinh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(sinh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
    return top;
}

static int ke_function_tanh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(tanh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_atan2(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top],
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(atan2(p->r, q->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_fmod(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p, *q;
    q = stack[--top],
    p = stack[--top];
	out = stack[top - 1];
	out->r = fmod(p->r, q->r);
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_csc(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(1/sin(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_sec(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
    out->r = convert_anglem(1/cos(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_cot(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(1/tan(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_csch(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
    p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(1/sinh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_sech(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(1/cosh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_coth(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(1/tanh(p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acosh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(log(p->r+sqrt(p->r*p->r-1)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_asinh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(log(p->r + sqrt(p->r*p->r+1)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_atanh(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(0.5*log((1+p->r)/(1-p->r)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acsc(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(asin(1/p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_asec(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(acos(1/p->r));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acot(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(-((2*atan(p->r)-M_PI)/2));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acsch(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(log(1/p->r + sqrt(1/p->r*1/p->r + 1)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_asech(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(log(1/p->r + sqrt(1/p->r*1/p->r - 1)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_acoth(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
   	ke1_t *out, *p;
	p = stack[--top];
	out = stack[top - 1];
	out->r = convert_anglem(0.5*log((1 + 1/p->r) / (1 - 1/p->r)));
	out->ttype = KET_VAL;
	out->vtype = KEV_REAL;
	out->assigned = 1;
	return top;
}

static int ke_function_timeyear(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = (int)localtime(&my_time)->tm_year + (int)1900;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_timemonth(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = (int)localtime(&my_time)->tm_mon + (int)1;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_timeday(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = localtime(&my_time)->tm_mday;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_timehour(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = localtime(&my_time)->tm_hour;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_timemin(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = localtime(&my_time)->tm_min;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_timesec(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	out = stack[top - 1];
	time_t my_time;
	time(&my_time);
	out->i = localtime(&my_time)->tm_sec;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_gcd(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	int64_t gcd = 0;
	q = stack[--top];
	p = stack[--top];
	out = stack[top - 1];
	int p_i = (p->i > 0) ? p->i : -p->i;
	int q_i = (q->i > 0) ? q->i : -q->i;

	for (int64_t i = 1; i <= p_i && i <= q_i; ++i)
	{
		// Checks if i is factor of both integers
		if (p_i%i == 0 && q_i%i == 0) {
			gcd = i;
		}
	}
	out->i = gcd;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

static int ke_function_lcm(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	ke1_t *out, *p, *q;
	int64_t lcm = 0;
	q = stack[--top];
	p = stack[--top];
	out = stack[top - 1];
	int p_i = (p->i > 0) ? p->i : -p->i;
	int q_i = (q->i > 0) ? q->i : -q->i;

	lcm = (p_i > q_i) ? p_i : q_i;

	// Always true
	while (1)
	{
		if (lcm%p->i == 0 && lcm%q->i == 0)
		{
			break;
		}
		++lcm;
	}

	out->i = lcm;
	out->vtype = KEV_INT;
	out->ttype = KET_VAL;
	return top;
}

void ke_function_hash(sml_t* sml) {
	ke_hash_add(sml, (fncp)&ke_function_anglem, FUNCTION_ANGLEM);
	ke_hash_add(sml, (fncp)&ke_function_exp, FUNCTION_EXP);
    ke_hash_add(sml, (fncp)&ke_function_pow, FUNCTION_POW);
	ke_hash_add(sml, (fncp)&ke_function_log, FUNCTION_LN);
	ke_hash_add(sml, (fncp)&ke_function_log, FUNCTION_LOG);
    ke_hash_add(sml, (fncp)&ke_function_log10, FUNCTION_LOG10);
    ke_hash_add(sml, (fncp)&ke_function_sqrt, FUNCTION_SQRT);
    ke_hash_add(sml, (fncp)&ke_function_sin, FUNCTION_SIN);
    ke_hash_add(sml, (fncp)&ke_function_cos, FUNCTION_COS);
    ke_hash_add(sml, (fncp)&ke_function_tan, FUNCTION_TAN);
    ke_hash_add(sml, (fncp)&ke_function_floor, FUNCTION_FLOOR);
    ke_hash_add(sml, (fncp)&ke_function_ceil, FUNCTION_CEIL);
    ke_hash_add(sml, (fncp)&ke_function_acos, FUNCTION_ACOS);
    ke_hash_add(sml, (fncp)&ke_function_asin, FUNCTION_ASIN);
    ke_hash_add(sml, (fncp)&ke_function_atan, FUNCTION_ATAN);
    ke_hash_add(sml, (fncp)&ke_function_cosh, FUNCTION_COSH);
    ke_hash_add(sml, (fncp)&ke_function_sinh, FUNCTION_SINH);
    ke_hash_add(sml, (fncp)&ke_function_tanh, FUNCTION_TANH);
    ke_hash_add(sml, (fncp)&ke_function_acosh, FUNCTION_ACOSH);
    ke_hash_add(sml, (fncp)&ke_function_asinh, FUNCTION_ASINH);
    ke_hash_add(sml, (fncp)&ke_function_atanh, FUNCTION_ATANH);
    ke_hash_add(sml, (fncp)&ke_function_atan2, FUNCTION_ATAN2);
    ke_hash_add(sml, (fncp)&ke_function_fmod, FUNCTION_FMOD);
    ke_hash_add(sml, (fncp)&ke_function_csc, FUNCTION_CSC);
    ke_hash_add(sml, (fncp)&ke_function_sec, FUNCTION_SEC);
    ke_hash_add(sml, (fncp)&ke_function_cot, FUNCTION_COT);
    ke_hash_add(sml, (fncp)&ke_function_csch, FUNCTION_CSCH);
    ke_hash_add(sml, (fncp)&ke_function_sech, FUNCTION_SECH);
    ke_hash_add(sml, (fncp)&ke_function_coth, FUNCTION_COTH);
    ke_hash_add(sml, (fncp)&ke_function_acsc, FUNCTION_ACSC);
    ke_hash_add(sml, (fncp)&ke_function_asec, FUNCTION_ASEC);
    ke_hash_add(sml, (fncp)&ke_function_acot, FUNCTION_ACOT);
    ke_hash_add(sml, (fncp)&ke_function_acsch, FUNCTION_ACSCH);
    ke_hash_add(sml, (fncp)&ke_function_asech, FUNCTION_ASECH);
    ke_hash_add(sml, (fncp)&ke_function_acoth, FUNCTION_ACOTH);
	ke_hash_add(sml, (fncp)&ke_function_timeyear, FUNCTION_TIMEYEAR);
	ke_hash_add(sml, (fncp)&ke_function_timemonth, FUNCTION_TIMEMONTH);
	ke_hash_add(sml, (fncp)&ke_function_timeday, FUNCTION_TIMEDAY);
	ke_hash_add(sml, (fncp)&ke_function_timehour, FUNCTION_TIMEHOUR);
	ke_hash_add(sml, (fncp)&ke_function_timemin, FUNCTION_TIMEMIN);
	ke_hash_add(sml, (fncp)&ke_function_timesec, FUNCTION_TIMESEC);
	ke_hash_add(sml, (fncp)&ke_function_gcd, FUNCTION_GCD);
	ke_hash_add(sml, (fncp)&ke_function_lcm, FUNCTION_LCM);
}

void ke_function_print(sml_t* sml,ke1_t *k) {
    printf("Function: %d\n", k->icmd);
    printf("%g\n", k->r);
}


