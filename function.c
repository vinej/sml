#include <math.h>
#include <time.h>

#include "api.h"
#include "function.h"

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

static void ke_function_anglem(sml_t* sml) { 
	anglem = sml_pop_int(sml);
}

static void ke_function_exp(sml_t* sml) {
	double r = exp(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_pow(sml_t* sml) {
	double q = sml_pop_real(sml);
	double p = sml_pop_real(sml);
	double r = pow(p, q);
	sml_push_real(sml, r);
}

static void ke_function_log(sml_t* sml) { 
	double r = log(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_log10(sml_t* sml) { 
	double r = log10(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_sqrt(sml_t* sml) {
	double r = sqrt(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_sin(sml_t* sml) { 
	double r = convert_anglem(sin(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_cos(sml_t* sml) { 
	double r = convert_anglem(cos(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_tan(sml_t* sml) {
	double r = convert_anglem(tan(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_floor(sml_t* sml) {
	double r = floor(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_ceil(sml_t* sml) {
	double r = ceil(sml_pop_real(sml));
	sml_push_real(sml, r);
}

static void ke_function_acos(sml_t* sml) { 
	double r = convert_anglem(acos(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_asin(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(asin(r));
	sml_push_real(sml, r);
}

static void ke_function_atan(sml_t* sml) { 
	double r = convert_anglem(atan(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_cosh(sml_t* sml) { 
	double r = convert_anglem(cosh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_sinh(sml_t* sml) { 
	double r = convert_anglem(sinh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_tanh(sml_t* sml) { 
	double r = convert_anglem(tanh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_atan2(sml_t* sml) {
	double q = sml_pop_real(sml);
	double p = sml_pop_real(sml);
	double r = convert_anglem(atan2(p, q));
	sml_push_real(sml, r);
}

static void ke_function_fmod(sml_t* sml) { 
	double q = sml_pop_real(sml);
	double p = sml_pop_real(sml);
	double r = fmod(p, q);
	sml_push_real(sml, r);
}

static void ke_function_csc(sml_t* sml) { 
	double r = convert_anglem(1/sin(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_sec(sml_t* sml) {
	double r = convert_anglem(1 / cos(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_cot(sml_t* sml) { 
	double r = convert_anglem(1 / tan(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_csch(sml_t* sml) { 
	double r = convert_anglem(1 / sinh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_sech(sml_t* sml) {
	double r = convert_anglem(1 / cosh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_coth(sml_t* sml) { 
	double r = convert_anglem(1 / tanh(sml_pop_real(sml)));
	sml_push_real(sml, r);
}

static void ke_function_acosh(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(log(r + sqrt(r*r-1)));
	sml_push_real(sml, r);
}

static void ke_function_asinh(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(log(r + sqrt(r*r + 1)));
	sml_push_real(sml, r);
}

static void ke_function_atanh(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(0.5*log((1 + r) / (1 - r)));
	sml_push_real(sml, r);
}

static void ke_function_acsc(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(asin(1/r));
	sml_push_real(sml, r);
}

static void ke_function_asec(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(acos(1/r));
	sml_push_real(sml, r);
}

static void ke_function_acot(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(-((2 * atan(r) - M_PI) / 2));
	sml_push_real(sml, r);
}

static void ke_function_acsch(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(log(1 / r + sqrt(1 / r * 1 / r + 1)));
	sml_push_real(sml, r);
}

static void ke_function_asech(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(log(1 / r + sqrt(1 / r * 1 / r - 1)));
	sml_push_real(sml, r);
}

static void ke_function_acoth(sml_t* sml) { 
	double r = sml_pop_real(sml);
	r = convert_anglem(0.5*log((1 + 1 /r) / (1 - 1 / r)));
	sml_push_real(sml, r);
}

static void ke_function_timeyear(sml_t* sml) { 
	time_t my_time;
	time(&my_time);
	int i = (int)localtime(&my_time)->tm_year + (int)1900;
	sml_push_int(sml, i);
}

static void ke_function_timemonth(sml_t* sml) {
	time_t my_time;
	time(&my_time);
	int i = (int)localtime(&my_time)->tm_mon + (int)1;
	sml_push_int(sml, i);
}

static void ke_function_timeday(sml_t* sml) { 
	time_t my_time;
	time(&my_time);
	int i = localtime(&my_time)->tm_mday;
	sml_push_int(sml, i);
}

static void ke_function_timehour(sml_t* sml) { 
	time_t my_time;
	time(&my_time);
	int i = localtime(&my_time)->tm_hour;
	sml_push_int(sml, i);
}

static void ke_function_timemin(sml_t* sml) {
	time_t my_time;
	time(&my_time);
	int i = localtime(&my_time)->tm_min;
	sml_push_int(sml, i);
}

static void ke_function_timesec(sml_t* sml) { 
	time_t my_time;
	time(&my_time);
	int i = localtime(&my_time)->tm_sec;
	sml_push_int(sml, i);
}

static void ke_function_gcd(sml_t* sml) {
	int64_t i = 0;
	int64_t q = sml_pop_int(sml);
	int64_t p = sml_pop_int(sml);
	int64_t p_i = (p > 0) ? p : -p;
	int64_t q_i = (q > 0) ? q : -q;

	for (int64_t j = 1; j <= p_i && i <= q_i; ++j)
	{
		// Checks if i is factor of both integers
		if (p_i%j == 0 && q_i%j == 0) {
			i = j;
		}
	}
	sml_push_int(sml, i);
}

static void ke_function_lcm(sml_t* sml) { 
	int64_t i = 0;
	int64_t q = sml_pop_int(sml);
	int64_t p = sml_pop_int(sml);
	int64_t p_i = (p > 0) ? p : -p;
	int64_t q_i = (q > 0) ? q : -q;

	i = (p_i > q_i) ? p_i : q_i;

	// Always true
	while (1)
	{
		if (i%p == 0 && i%q == 0)
		{
			break;
		}
		++i;
	}
	sml_push_int(sml, i);
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

void ke_function_print(sml_t* sml,token_t *k) {
    printf("Function: %d\n", k->icmd);
    printf("%g\n", k->r);
}


