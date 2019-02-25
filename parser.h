#ifndef PARSER_H
#define PARSER_H
#include "kexpr.h"

static int ke_op[KEO_MAX] = {
	0,
	1 << 1 | 1, 1 << 1 | 1, 1 << 1 | 1, 1 << 1 | 1, // unary operators
	2 << 1 | 1, // pow()
	3 << 1, 3 << 1, 3 << 1, 3 << 1, // * / // %
	4 << 1, 4 << 1, // + and - and =
	5 << 1, 5 << 1, // << and >>
	6 << 1, 6 << 1, 6 << 1, 6 << 1, // < > <= >=
	7 << 1, 7 << 1, // == !=
	8 << 1, // &
	9 << 1, // ^	kdq_destroy(int, callstack);

	10 << 1,// |
	11 << 1,// &&
	12 << 1, // ||
	13 << 1, // =
	14 << 1 //;
};

static const char *ke_opstr[] = {
	"",
	"+(1)", "-(1)", "~", "!",
	"**",
	"*", "/", "//", "%",
	"+", "-",
	"<<", ">>",
	"<", "<=", ">", ">=",
	"==", "!=",
	"&",
	"^",
	"|",
	"&&",
	"||",
	"=" ,
	";"
};

#define CMD_IFOR 1
#define CMD_INEXT 2
#define CMD_IBRK 3
#define CMD_ICNT 4
#define CMD_IDEF 5
#define CMD_IRTN 6
#define CMD_IIF 7
#define CMD_IELSE 8
#define CMD_IEND 9
#define CMD_IEXE 10

kexpr_t *ke_parse(sml_t *sml, utf8 *_s, int *err);

#endif
