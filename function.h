#ifndef FUNCTION_H_INCLUDED
#define FUNCTION_H_INCLUDED

#include "api.h"

void ke_function_print(sml_t* sml, token_t *k);
void ke_function_hash(sml_t* sml);

#define FUNCTION_ANGLEM "anglem"
#define FUNCTION_EXP "exp"
#define FUNCTION_POW "pow"
#define FUNCTION_LN "ln"
#define FUNCTION_LOG "log"
#define FUNCTION_LOG10 "log10"
#define FUNCTION_SQRT "sqrt"
#define FUNCTION_SIN "sin"
#define FUNCTION_COS "cos"
#define FUNCTION_TAN "tan"
#define FUNCTION_FLOOR "floor"
#define FUNCTION_CEIL "ceil"
#define FUNCTION_ACOS "acos"
#define FUNCTION_ASIN "asin"
#define FUNCTION_ATAN "atan"
#define FUNCTION_COSH "cosh"
#define FUNCTION_SINH "sinh"
#define FUNCTION_TANH "tanh"
#define FUNCTION_ACOSH "acosh"
#define FUNCTION_ASINH "asinh"
#define FUNCTION_ATANH "atanh"
#define FUNCTION_ATAN2 "atan2"
#define FUNCTION_FMOD "fmod"
#define FUNCTION_CSC "csc"
#define FUNCTION_SEC "sec"
#define FUNCTION_COT "cot"
#define FUNCTION_CSCH "csch"
#define FUNCTION_SECH "sech"
#define FUNCTION_COTH "coth"
#define FUNCTION_ACSC "acsc"
#define FUNCTION_ASEC "asec"
#define FUNCTION_ACOT "acot"
#define FUNCTION_ACSCH "acsch"
#define FUNCTION_ASECH "asech"
#define FUNCTION_ACOTH "acoth"
#define FUNCTION_TIMEYEAR "time_year"
#define FUNCTION_TIMEMONTH "time_month"
#define FUNCTION_TIMEDAY "time_day"
#define FUNCTION_TIMEHOUR "time_hour"
#define FUNCTION_TIMEMIN "time_min"
#define FUNCTION_TIMESEC "time_sec"
#define FUNCTION_GCD "gcd"
#define FUNCTION_LCM "lcm"


#endif // FUNCTION_H_INCLUDED
