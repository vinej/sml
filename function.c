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

static int ke_function_anglem(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[--top];
	anglem = (int)p->i;
	return top;
}

static int ke_function_exp(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = exp(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_pow(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = pow(p->r, q->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_log(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = log(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_log10(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = log10(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sqrt(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = sqrt(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sin(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(sin(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cos(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(cos(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_tan(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(tan(p->r));
	p->ttype = KET_VAL;
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_floor(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = floor(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_ceil(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = ceil(p->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acos(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
	p->r = convert_anglem(acos(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asin(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(asin(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atan(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(atan(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cosh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(cosh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sinh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(sinh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_tanh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(tanh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atan2(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = convert_anglem(atan2(p->r, q->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_fmod(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p, *q;
    q = &stack[--top],
    p = &stack[top-1];
    p->r = fmod(p->r, q->r);
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_csc(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/sin(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sec(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/cos(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_cot(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/tan(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_csch(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/sinh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_sech(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/cosh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_coth(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(1/tanh(p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acosh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(log(p->r+sqrt(p->r*p->r-1)));
    p->vtype = KEV_REAL;
    p->ttype = KET_VAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asinh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(log(p->r + sqrt(p->r*p->r+1)));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_atanh(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(0.5*log((1+p->r)/(1-p->r)));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acsc(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(asin(1/p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asec(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(acos(1/p->r));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acot(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(-((2*atan(p->r)-M_PI)/2));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acsch(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(log(1/p->r + sqrt(1/p->r*1/p->r + 1)));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_asech(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(log(1/p->r + sqrt(1/p->r*1/p->r - 1)));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_acoth(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->r = convert_anglem(0.5*log((1 + 1/p->r) / (1 - 1/p->r)));
    p->vtype = KEV_REAL;
    p->assigned = 1;
    return top;
}

static int ke_function_timeyear(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	// push into the stack a value, becase this function does not have parameters
	p = &stack[top - 1];
	stack[top++] = *p;
	// because 
	p = &stack[top - 1];
	time_t my_time;
	//struct tm * timeinfo;
	time(&my_time);
	//timeinfo = localtime(&my_time);
	p->i = (int)localtime(&my_time)->tm_year + (int)1900;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_timemonth(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	// because 
	p = &stack[top - 1];
	time_t my_time;
	time(&my_time);
	p->i = (int)localtime(&my_time)->tm_mon + (int)1;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_timeday(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	p = &stack[top - 1];
	time_t my_time;
	time(&my_time);
	p->i = localtime(&my_time)->tm_mday;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_timehour(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	p = &stack[top - 1];
	time_t my_time;
	time(&my_time);
	p->i = localtime(&my_time)->tm_hour;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_timemin(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	p = &stack[top - 1];
	time_t my_time;
	time(&my_time);
	p->i = localtime(&my_time)->tm_min;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_timesec(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p;
	p = &stack[top - 1];
	stack[top++] = *p;
	p = &stack[top - 1];
	time_t my_time;
	time(&my_time);
	p->i = localtime(&my_time)->tm_sec;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_gcd(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	int64_t gcd = 0;
	q = &stack[--top];
	p = &stack[top - 1];
	p->i = (p->i > 0) ? p->i : -p->i;
	q->i = (q->i > 0) ? q->i : -q->i;

	for (int64_t i = 1; i <= p->i && i <= q->i; ++i)
	{
		// Checks if i is factor of both integers
		if (p->i%i == 0 && q->i%i == 0) {
			gcd = i;
		}
	}
	p->i = gcd;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
	return top;
}

static int ke_function_lcm(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->g_stack;
	ke1_t *p, *q;
	int64_t lcm = 0;
	q = &stack[--top];
	p = &stack[top - 1];
	p->i = (p->i > 0) ? p->i : -p->i;
	q->i = (q->i > 0) ? q->i : -q->i;

	lcm = (p->i>q->i) ? p->i : q->i;

	// Always true
	while (1)
	{
		if (lcm%p->i == 0 && lcm%q->i == 0)
		{
			break;
		}
		++lcm;
	}

	p->i = lcm;
	p->vtype = KEV_INT;
	p->ttype = KET_VAL;
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


