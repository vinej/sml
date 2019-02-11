#include "kexpr.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include "vector.h"
#include "matrix.h"
#include "complex.h"
#include "constants.h"
#include "function.h"
#include "property.h"
#include "str.h"
#include "file.h"
#include "gdate.h"
#include "date.h"
#include "command.h"
#include "utf8.h"

#if defined(_MSC_VER) || defined(_WIN32)
#include <Windows.h>
#endif

static int ke_op[KEO_MAX] = {
	0,
	1<<1|1, 1<<1|1, 1<<1|1, 1<<1|1, // unary operators
	2<<1|1, // pow()
	3<<1, 3<<1, 3<<1, 3<<1, // * / // %
	4<<1, 4<<1, // + and - and =
	5<<1, 5<<1, // << and >>
	6<<1, 6<<1, 6<<1, 6<<1, // < > <= >=
	7<<1, 7<<1, // == !=
	8<<1, // &
	9<<1, // ^	kdq_destroy(int, callstack);

	10<<1,// |
	11<<1,// &&
	12<<1, // ||
	13<<1, // =
	14<<1 //;
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

/**********************
 * Operator functions *
 **********************/

#define KE_GEN_CMP(_type, _op) \
	static void ke_op_##_type(ke1_t *p, ke1_t *q) { \
		if (p->vtype == KEV_STR && q->vtype == KEV_STR) p->i = (strcmp(p->obj.s, q->obj.s) _op 0); \
		else p->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL? (p->r _op q->r) : (p->i _op q->i); \
		p->r = (double)p->i; \
		p->vtype = KEV_INT; \
	}

KE_GEN_CMP(KEO_LT, <)
KE_GEN_CMP(KEO_LE, <=)
KE_GEN_CMP(KEO_GT, >)
KE_GEN_CMP(KEO_GE, >=)
KE_GEN_CMP(KEO_EQ, ==)
KE_GEN_CMP(KEO_NE, !=)

#define KE_GEN_BIN_INT(_type, _op) \
	static void ke_op_##_type(ke1_t *p, ke1_t *q) { \
		p->i _op q->i; p->r = (double)p->i; \
		p->vtype = KEV_INT; \
	}

KE_GEN_BIN_INT(KEO_BAND, &=)
KE_GEN_BIN_INT(KEO_BOR, |=)
KE_GEN_BIN_INT(KEO_BXOR, ^=)
KE_GEN_BIN_INT(KEO_LSH, <<=)
KE_GEN_BIN_INT(KEO_RSH, >>=)
KE_GEN_BIN_INT(KEO_MOD, %=)
KE_GEN_BIN_INT(KEO_IDIV, /=)

#define KE_GEN_BIN_BOTH(_type, _op) \
	static void ke_op_##_type(ke1_t *p, ke1_t *q) { \
		p->i _op q->i; p->r _op q->r; \
		p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL? KEV_REAL : KEV_INT; \
	}

KE_GEN_BIN_BOTH(KEO_ADD, +=)
KE_GEN_BIN_BOTH(KEO_SUB, -=)
KE_GEN_BIN_BOTH(KEO_MUL, *=)

static void ke_op_KEO_DIV(ke1_t *p, ke1_t *q)  { p->r /= q->r, p->i = (int64_t)(p->r + .5); p->vtype = KEV_REAL; }
static void ke_op_KEO_LAND(ke1_t *p, ke1_t *q) { p->i = (p->i && q->i); p->r = (double)p->i; p->vtype = KEV_INT; }
static void ke_op_KEO_LOR(ke1_t *p, ke1_t *q)  { p->i = (p->i || q->i); p->r = (double)p->i; p->vtype = KEV_INT; }
static void ke_op_KEO_POW(ke1_t *p, ke1_t *q)  { p->r = pow(p->r, q->r), p->i = (int64_t)(p->r + .5); p->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL? KEV_REAL : KEV_INT; }
static void ke_op_KEO_BNOT(ke1_t *p, ke1_t *q) { p->i = ~p->i; p->r = (double)p->i; p->vtype = KEV_INT; }
static void ke_op_KEO_LNOT(ke1_t *p, ke1_t *q) { p->i = !p->i; p->r = (double)p->i; p->vtype = KEV_INT; }
static void ke_op_KEO_POS(ke1_t *p, ke1_t *q)  { } // do nothing
static void ke_op_KEO_NOP(ke1_t *p, ke1_t *q)  { } // do nothing
static void ke_op_KEO_NEG(ke1_t *p, ke1_t *q)  { p->i = -p->i, p->r = -p->r; }

static void ke_func1_abs(ke1_t *p, ke1_t *q) { if (p->vtype == KEV_INT) p->i = (int64_t)abs((int)p->i), p->r = (double)p->i; else p->r = fabs(p->r), p->i = (int64_t)(p->r + .5); }

// VALIDATION

#ifdef _DEBUG
void ke_validate_parameter_qte(ke1_t *p, int nb_param, char * function_name) {
	if (p->n_args != nb_param) {
		printf("SML: Invalid number of parameters at line %d (got %d expected %d) for function %s", p->sourceLine, p->n_args, nb_param, function_name);
		exit(1);
	}
}

void ke_validate_parameter_vtype(ke1_t * p, int vtype, char * param_name, char * function_name) {
	if (p->vtype != vtype) {
		printf("SML: Invalid type of parameter <%s> type at line <%d> (got %d expected %d) for function <%s>", param_name, p->sourceLine, p->vtype, vtype, function_name);
		exit(1);
	}
}

void ke_validate_parameter_ttype(ke1_t * p, int ttype, char * function_name) {
	if (p->ttype != ttype) {
		printf("SML: Invalid type of parameter at line <%d< (got %d expected %d) for function <%s>", p->sourceLine, p->ttype, ttype, function_name);
		exit(1);
	}
}

void ke_validate_parameter_not_null(ke1_t * p, void * ptr, char * param_name, char * function_name) {
	if (ptr == NULL) {
		printf("SML: Parameter <%s> is null at line <%d> for function <%s>", param_name, p->sourceLine, function_name);
		exit(1);
	}
}

void ke_validate_parameter_int_gt_zero(ke1_t * p, char * param_name, char * function_name) {
	if (p->i <= 0) {
		printf("SML: Parameter <%s> must be greater than zero at line <%d> for function <%s>", param_name, p->sourceLine, function_name);
		exit(1);
	}
}
#endif // _DEBUG

sml_t * ke_create_sml() {
	sml_t * sml = calloc(sizeof(sml_t), 1);
	sml->lastDef = -1;
	sml->g_fortop = 0;
	sml->dllke_hash_add = ke_hash_add;
	return sml;
}

// MEMORY ALLOCATION FUNCTIONS
void ke_init_memory_count(sml_t *sml) {
	sml->mem_count = 0;
}

void ke_inc_memory(sml_t *sml) {
    ++sml->mem_count;
}

void ke_dec_memory(sml_t *sml) {
    --sml->mem_count;
}

void * ke_calloc_memory(sml_t *sml, size_t i, size_t x) {
    void * tmp = calloc(i,x);
    ++sml->mem_count;
    return tmp;
}

void * ke_malloc_memory(sml_t *sml, size_t i) {
    void * tmp = malloc(i);
    ++sml->mem_count;
    return tmp;
}

void ke_free_memory(sml_t *sml, void * m) {
	free(m);
	--sml->mem_count;
}

char *ke_mystrndup(sml_t *sml, char *src, size_t n)
{
	char *dst;
	dst = (char*)calloc(n + 1, 1);
	if (dst == NULL) {
		printf("out of memory at ke_mystrndup");
		printf("TODO clean up the memory");
		abort();
	}
	++sml->mem_count;
	strncpy(dst, src, n);
	return dst;
}

utf8 *ke_mystr(sml_t *sml, utf8 *src, size_t n)
{
	utf8*dst;
	char sstart[] = "1+1;1+1;";
	size_t start = utf8len(sstart);
	dst = (utf8*)calloc(start + n + 1, 1);
	if (dst == NULL) {
		printf("out of memory at ke_mystr");
		printf("TODO clean up the memory");
		abort();
	}
	++sml->mem_count;
	utf8cpy(dst, sstart);
	utf8cat(dst, src);
	return dst;
}
//****************************


// CURENT TOKEN FUNCTION
ke1_t * ke_get_tok(sml_t *sml) {
	return sml->tokens[sml->tok_idx];
}


ke1_t * ke_get_tokidx(sml_t *sml, int idx) {
	return sml->tokens[idx];
}
//*********************************


fncp ke_function(sml_t *sml, char * name) {
	khint_t iter = kh_get(5, sml->hfunction, name);
	if (kh_end(sml->hfunction) != iter) {
		return (fncp)kh_val(sml->hfunction, iter);
	}
	else {
		return NULL;
	}
}

typedef int(*DLLPROC)();
void ke_import(sml_t *sml, char * s) {
#if defined(_MSC_VER) || defined(_WIN32)
	// load the dll
	// call method ke_dll_hash(voir * hash_method, global_field)
	if ((sml->libhandle[sml->libhandle_qte] = LoadLibrary(TEXT(s))) == NULL)
	{
		printf("load failed\n");
		return;
	}
	DLLPROC fp = (DLLPROC)GetProcAddress(sml->libhandle[sml->libhandle_qte], "ke_dll_hash_add");
	if (fp == NULL)
	{
		printf("GetProcAddress failed\n");
		printf("%d\n", GetLastError());
		return;
	}

	fp(sml);
	++sml->libhandle_qte;
	//FREE HANDLE
#else
	return;
#endif
}

void ke_load_dll(sml_t *sml, char * p) {
	char *t = p;
	while (*t != '"' && *t != '\'') ++t;
	++t;
	char * end = t + 1;
	while (*end != '"' && *end != '\'') ++end;
	char old = *end;
	*end = 0;
	ke_import(sml, t);
	*end = old;
}

int ke_manage_function(sml_t *sml, ke1_t * tok, int err) {
	tok->n_args = 1;
	tok->sourceLine = sml->sourceCodeLine;
	tok->f.defcmd = (cmdp)ke_command(sml, tok->name);
	if (tok->f.defcmd != NULL) {
		tok->icmd = ke_command_icmd(sml, tok->name);
		tok->ttype = KET_CMD;
		if (strcmp(tok->name, CMD_DEF) == 0) {
			sml->isNextDefName = 1;
		}
		tok->ijmp = 0;
	}
	else {
		tok->f.deffunc = (fncp)ke_function(sml, tok->name);
		if (tok->f.deffunc != NULL) {
			tok->ttype = KET_FUNC;
		}
		else {
			strcpy(sml->lastErrorMessage, tok->name);
			err |= KEE_UNFUNC;
		}
	}
	return err;
}

int ke_manage_property(sml_t *sml, ke1_t *tok, int err) {
	// it's a propery
	tok->n_args = 1;
	tok->sourceLine = sml->sourceCodeLine;
	tok->ijmp = 0;
	tok->ttype = KET_PROP;
	tok->vtype = KEV_REAL;
	tok->propget = 1;
	tok->propset = 0;
	tok->i = 0, tok->r = 0.;
	khint_t iter = kh_get(6, sml->hname, tok->name);
	if (kh_end(sml->hname) != iter) {
		tok->ifield = kh_val(sml->hname, iter);
	}
	else {
		int absent;
		khint_t iter = kh_put(6, sml->hname, tok->name, &absent);
		kh_val(sml->hname, iter) = sml->field_qte;
		tok->ifield = sml->field_qte;
		sml->field_qte++;
		//*err |= KEE_UNVAR;
	}
	return err;
}

int ke_manage_variable(sml_t *sml, ke1_t *tok, int err) {
	tok->ttype = KET_VNAME;
	if (sml->isNextDefName) {
		strcpy(sml->currentDefName, tok->name);
		sml->isNextDefName = 0;
		tok->islocal = 1;
	}
	else {
		// inside a function, the variables are localized with the functon name, except
		// variables starting with "_g"
		if ((*sml->currentDefName != 0) && (strncmp(tok->name, __GLOBAL, 2) != 0)) {
			char * localName = ke_calloc_memory(sml, strlen(tok->name) + strlen(sml->currentDefName) + 4, 1);
			strcpy(localName, __GLOBAL_DSEP);
			strcat(localName, sml->currentDefName);
			strcat(localName, __GLOBAL_SEP);
			strcat(localName, tok->name);
			ke_free_memory(sml, tok->name);
			tok->name = localName;
			tok->islocal = 1;
		}
		else {
			tok->islocal = 0;
		}
	}

	ke1_t * recp = NULL;
	khint_t iter = kh_get(6, sml->hname, tok->name);
	if (kh_end(sml->hname) != iter) {
		tok->ifield = kh_val(sml->hname, iter);
	}
	else {
		// if the name contains '.', we must create a variable for the record and
		// add the current ifield to the record list
		char * dotp = strchr(tok->name, '.');
		if (dotp) {
			// split at the dot. tok.name will contain only the rec name  p.test => p
			*dotp = 0;
			int ifield;
			// check if the record is alreeady there
			khint_t iter = kh_get(6, sml->hname, tok->name);
			if (kh_end(sml->hname) != iter) {
				ifield = kh_val(sml->hname, iter);
				recp = sml->recp[ifield];
			}
			else {
				int absent;
				char * recName = ke_calloc_memory(sml, strlen(tok->name) + 1, 1);
				strcpy(recName, tok->name);
				khint_t iter = kh_put(6, sml->hname, recName, &absent);
				kh_val(sml->hname, iter) = sml->rec_qte;
				recp = (ke1_t *)ke_calloc_memory(sml,sizeof(ke1_t), 1);
				recp->ifield = sml->rec_qte;
				recp->name = recName;
				recp->ttype = KET_REC;
				recp->vtype = KEV_REC;
				sml->recp[sml->rec_qte] = recp;
				sml->rec_qte++;
			}
			*dotp = '.';
		}
		int absent;
		khint_t iter = kh_put(6, sml->hname, tok->name, &absent);
		kh_val(sml->hname, iter) = sml->field_qte;
		tok->ifield = sml->field_qte;
		// the field of a record contains a pointer to the record
		tok->f.recp = recp;
		sml->field_qte++;

		// create the list if not done
		if (recp) {
			if (!recp->obj.reclist) {
				recp->obj.reclist = ke_calloc_memory(sml, sizeof(int) * 30, 1);
				recp->i = 0;
			}
			// add the new field into the list
			recp->obj.reclist[recp->i] = tok->ifield;
			++recp->i;
		}
	}
	return err;
}

int ke_manage_command(sml_t *sml, ke1_t *tok, int err) {
	if (strcmp(tok->name, CMD_RTN) == 0) {
		*sml->currentDefName = 0;
	}
	tok->ttype = KET_VCMD;
	tok->icmd = ke_command_icmd(sml, tok->name);
	return err;
}

int ke_manage_digit(sml_t* sml,ke1_t *tok, char **r, char **p, char **q, int err) {
	long x;
	double y;
	char *pp;
	tok->ttype = KET_VAL;
	y = strtod(*q, &(*p));
	x = strtol(*q, &pp, 0); // FIXME: check int/double parsing errors
	if (*q == *p && *q == pp) { // parse error
		err |= KEE_NUM;
	}
	else if (*p > pp) { // has "." or "[eE]"; then it is a real number
		tok->vtype = KEV_REAL;
		tok->i = (int64_t)(y + .5), tok->r = y;
		*r = *p;
	}
	else {
		tok->vtype = KEV_INT;
		tok->i = x, tok->r = y;
		*r = pp;
	}
	return err;
}

int ke_manage_string(sml_t *sml, ke1_t *tok, char **r, char **p, char **q, int err) {
	int c = **p;
	for (++(*p); **p && **p != c; ++(*p))
	if (**p == '\\') {
		++(*p); // escaping
	}
	if (**p == c) {
		tok->ttype = KET_VAL;
		tok->vtype = KEV_STR;
		tok->obj.s = ke_mystrndup(sml,*q + 1, *p - *q - 1);
		*r = *p + 1;
	}
	else
	{
		err |= KEE_UNQU;
		*r = *p;
	}
	return err;
}

int ke_manager_operator(sml_t *sml, ke1_t *tok, char **r, char **p, char **q, int last_is_val, int err) {
	tok->ttype = KET_OP;
	if ( **p == '*' && (*p)[1] == '*') tok->op = KEO_POW, tok->f.builtin = ke_op_KEO_POW, tok->n_args = 2, *r = *q + 2;
	else if (**p == '*') tok->op = KEO_MUL, tok->f.builtin = ke_op_KEO_MUL, tok->n_args = 2, *r = *q + 1; // FIXME: NOT working for unary operators
	else if (**p == '/' && (*p)[1] == '/') tok->op = KEO_IDIV, tok->f.builtin = ke_op_KEO_IDIV, tok->n_args = 2, *r = *q + 2;
	else if (**p == '/') tok->op = KEO_DIV, tok->f.builtin = ke_op_KEO_DIV, tok->n_args = 2, *r = *q + 1;
	else if (**p == '%') tok->op = KEO_MOD, tok->f.builtin = ke_op_KEO_MOD, tok->n_args = 2, *r = *q + 1;
	else if (**p == '+') {
		if (last_is_val) tok->op = KEO_ADD, tok->f.builtin = ke_op_KEO_ADD, tok->n_args = 2;
		else tok->op = KEO_POS, tok->f.builtin = ke_op_KEO_POS, tok->n_args = 1;
		*r = *q + 1;
	}
	else if (**p == '-') {
		if (last_is_val) tok->op = KEO_SUB, tok->f.builtin = ke_op_KEO_SUB, tok->n_args = 2;
		else tok->op = KEO_NEG, tok->f.builtin = ke_op_KEO_NEG, tok->n_args = 1;

		*r = *q + 1;
	}
	else if (**p == '=' && (*p)[1] != '=') tok->op = KEO_LET, tok->f.builtin = ke_op_KEO_EQ, tok->n_args = 2, *r = *q + 1;
	else if (**p == '=' && (*p)[1] == '=') tok->op = KEO_EQ, tok->f.builtin = ke_op_KEO_EQ, tok->n_args = 2, *r = *q + 2;
	else if (**p == '!' && (*p)[1] == '=') tok->op = KEO_NE, tok->f.builtin = ke_op_KEO_NE, tok->n_args = 2, *r = *q + 2;
	else if (**p == '<' && (*p)[1] == '>') tok->op = KEO_NE, tok->f.builtin = ke_op_KEO_NE, tok->n_args = 2, *r = *q + 2;
	else if (**p == '>' && (*p)[1] == '=') tok->op = KEO_GE, tok->f.builtin = ke_op_KEO_GE, tok->n_args = 2, *r = *q + 2;
	else if (**p == '<' && (*p)[1] == '=') tok->op = KEO_LE, tok->f.builtin = ke_op_KEO_LE, tok->n_args = 2, *r = *q + 2;
	else if (**p == '>' && (*p)[1] == '>') tok->op = KEO_RSH, tok->f.builtin = ke_op_KEO_RSH, tok->n_args = 2, *r = *q + 2;
	else if (**p == '<' && (*p)[1] == '<') tok->op = KEO_LSH, tok->f.builtin = ke_op_KEO_LSH, tok->n_args = 2, *r = *q + 2;
	else if (**p == '>') tok->op = KEO_GT, tok->f.builtin = ke_op_KEO_GT, tok->n_args = 2, *r = *q + 1;
	else if (**p == '<') tok->op = KEO_LT, tok->f.builtin = ke_op_KEO_LT, tok->n_args = 2, *r = *q + 1;
	else if (**p == '|' && (*p)[1] == '|') tok->op = KEO_LOR, tok->f.builtin = ke_op_KEO_LOR, tok->n_args = 2, *r = *q + 2;
	else if (**p == '&' && (*p)[1] == '&') tok->op = KEO_LAND, tok->f.builtin = ke_op_KEO_LAND, tok->n_args = 2, *r = *q + 2;
	else if (**p == '|') tok->op = KEO_BOR, tok->f.builtin = ke_op_KEO_BOR, tok->n_args = 2, *r = *q + 1;
	else if (**p == '&') tok->op = KEO_BAND, tok->f.builtin = ke_op_KEO_BAND, tok->n_args = 2, *r = *q + 1;
	else if (**p == '^') tok->op = KEO_BXOR, tok->f.builtin = ke_op_KEO_BXOR, tok->n_args = 2, *r = *q + 1;
	else if (**p == '~') tok->op = KEO_BNOT, tok->f.builtin = ke_op_KEO_BNOT, tok->n_args = 1, *r = *q + 1;
	else if (**p == '!') tok->op = KEO_LNOT, tok->f.builtin = ke_op_KEO_LNOT, tok->n_args = 1, *r = *q + 1;
	else if (**p == ';') tok->op = KEO_NOP, sml->isLastTokenNop = 1, tok->f.builtin = ke_op_KEO_NOP, tok->n_args = 2, *r = *q + 1;
	else {
		tok->ttype = KET_NULL;
		err |= KEE_UNOP;
	}

	sml->isFirstToken = 0;
	return err;
}

// comments are not kept into the token list
int ke_manage_comment(sml_t *sml, ke1_t * tok, char **p) {
	++(*p);
	while (**p != 0) {
		if (**p == '\n') {
			++sml->sourceCodeLine;
			break;
		}
		++(*p);
	}
	if (**p == 0) {
		// fin du code pas d'autre tolen
		tok->realToken = 0;
		return 0;
	}
	else {
		return 1;
	}
}

int ke_manage_line_continuation(sml_t *sml, ke1_t * tok, char**p) {
	++(*p);
	while (**p != 0) {
		if (**p == '\n') {
			++sml->sourceCodeLine;
			++(*p);
			break;
		}
		++(*p);
	}
	if (**p == 0) {
		// fin du code pas d'autre tolen
		tok->realToken = 0;
		return 0;
	}
	else {
		return 1;
	}
}

// parse a token except "(", ")" and ","
ke1_t ke_read_token(sml_t *sml, char *p, char **r, int *err, int last_is_val) // it doesn't parse parentheses
{
	ke1_t tok;
	memset(&tok, 0, sizeof(ke1_t));
	tok.realToken = 1;

	// loop until we find the start of a real token
	while (1) { // not a real token
		// eofile
		if (*p == 0) {	tok.realToken = 0;	return tok;	}

		// comment
		if (*p == '#') {
			
			if (!ke_manage_comment(sml, &tok, &p)) return tok;
			continue;
		}

		// newline
		if (*p == '\n') {
			++sml->sourceCodeLine;
			if (sml->isLastTokenNop || sml->isFirstToken) { ++p;	continue; }
			*p = ';';
			break;
		}

		// command separator. We only keep one separator between commands
		if (*p == ';') {
			if (sml->isLastTokenNop || sml->isFirstToken) { ++p;	continue;	}
			break;
		}

		// no control character and space
		if (utf8iscontrol((void**)&p)) {	++p; continue; }

		// line continuation
		if (*p == '\\') {
			if (!ke_manage_line_continuation(sml, &tok, &p)) { return tok; }
			continue;
		}

		// ok we can continue with a real token
		break;
	}

	if (*p == 0) {	tok.realToken = 0;	return tok; }

	char *q = p;
	sml->isLastTokenNop = 0;
	tok.sourceLine = sml->sourceCodeLine;
	if (*p == '_' || utf8isalpha((void**)&p) ) { // a variable or a function
		for (; *p && (*p == '_' || *p == '.' || utf8isalnum((void**)&p)); ++p);
		if (*p == 0) {	tok.realToken = 0;	return tok;	}

		tok.name = ke_mystrndup(sml, q, p - q);
		while (*p) { if (*p == ' ') ++p; else break; }  // pass over the spaces
		if (*p == 0) { tok.realToken = 0; return tok; }

		if (*p == '(') {
			// manage import immeately, because DLL must be mounted and available functions of the DLL set
			if (strcmp(tok.name, "import") == 0) {
				ke_load_dll(sml, p);
			}
			*err = ke_manage_function(sml, &tok, *err);
		}
		else if (*p == '[') {
			*err = ke_manage_property(sml, &tok, *err);
		}
		else {
			tok.r = ke_constants(tok.name);
			if (tok.r != 0) {
				tok.ttype = KET_VAL; // a constant is only a real variable
			}
			else {
				tok.f.defvcmd = (vcmdp)ke_command_val(sml, tok.name);
				if (tok.f.defvcmd != NULL) {
					*err = ke_manage_command(sml, &tok, *err);
				}
				else {
					*err = ke_manage_variable(sml, &tok, *err);
				}
				tok.i = 0, tok.r = 0.;
			}
			// default type for a variable
			tok.vtype = KEV_REAL;
		}
		*r = p;
	} else if (isdigit(*p) || *p == '.') { // a number
		*err = ke_manage_digit(sml, &tok, r, &p, &q, *err);
	} else if (*p == '"' || *p == '\'') { // a string value
		*err = ke_manage_string(sml, &tok, r, &p, &q, *err);
	} else { // an operator
		*err = ke_manager_operator(sml, &tok, r, &p, &q, last_is_val, *err);
	}
	return tok;
}

static inline ke1_t *push_back(sml_t *sml, ke1_t **a, int *n, int *m)
{
	if (*n == *m) {
		int old_m = *m;
		*m = *m? *m<<1 : 8;
		// first timae allocation
		if (*a == 0) ke_inc_memory(sml);
		*a = (ke1_t*)realloc(*a, *m * sizeof(ke1_t));
		if (*a == NULL) {
			printf("out of memory at push_back");
			printf("TODO clean up the memory");
			abort();
		}

		memset(*a + old_m, 0, (*m - old_m) * sizeof(ke1_t));
	}
	return &(*a)[(*n)++];
}

ke1_t *ke_parse_core(sml_t* sml, utf8 *_s, int *_n, int *err)
{
    char * p;
	int n_out, m_out, n_op, m_op, last_is_val = 0;
	ke1_t *out, *op, *t, *u;

	t = NULL;
		
	ke1_t * last = NULL;

	// remove remark, and normalize line seperator
	utf8 *s;
	s = ke_mystr(sml,_s, utf8size(_s) + 1); // make a copy
	//ke_normalize_code(s);
	*err = 0;
 	*_n = 0;
	out = op = 0;
	n_out = m_out = n_op = m_op = 0;
	p = s;
	int isLastLeftBraket = 0;
	int isPreviousLeftParenthese = 0;
	int isPreviousLeftbraket = 0;
	while (*p) {
		while (1) {
			if (*p == ' ') ++p; else break;
		}
		if (*p == '(' || *p == '[') {
			isPreviousLeftParenthese = 1;
			t = push_back(sml, &op, &n_op, &m_op); // push to the operator g_stack
			t->op = -1, t->ttype = KET_NULL; // ->op < 0 for a left parenthsis
			++p;
			last_is_val = 0;
			isLastLeftBraket = 0;
		} else if (*p == ')' || *p == ']') {
			if (*p == ']') isLastLeftBraket = 1; else isLastLeftBraket = 0;
			while (n_op > 0 && op[n_op-1].op >= 0) { // move operators to the output until we see a left parenthesis
				u = push_back(sml, &out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op == 0) { // error: extra right parenthesis
				*err |= KEE_UNRP;
				break;
			} else --n_op; // pop out '('
			if (n_op > 0 && (op[n_op-1].ttype == KET_FUNC || op[n_op - 1].ttype == KET_PROP || op[n_op - 1].ttype == KET_CMD)) { // the top of the operator g_stack is a function
				u = push_back(sml, &out, &n_out, &m_out); // move it to the output
				*u = op[--n_op];

				if ((u->n_args == 1) && isPreviousLeftParenthese) --u->n_args;  // no parameters

				if (u->n_args == 1 && strcmp(u->name, "abs") == 0) u->f.builtin = ke_func1_abs;
			}
			++p;
			isPreviousLeftParenthese = 0;
			last_is_val = 1;
		} else if (*p == ',') { // function arguments separator
			isLastLeftBraket = 0;
			isPreviousLeftParenthese = 0;
			while (n_op > 0 && op[n_op-1].op >= 0) {
				u = push_back(sml, &out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op < 2 || (op[n_op-2].ttype != KET_FUNC && op[n_op - 2].ttype != KET_PROP && op[n_op- 2].ttype != KET_CMD)) { // we should at least see a function and a left parenthesis
				*err |= KEE_FUNC;
				break;
			}
			++op[n_op-2].n_args;
			++p;
			last_is_val = 0;
		} else { // output-able token
			isPreviousLeftParenthese = 0;
			ke1_t v = ke_read_token(sml, p, &p, err, last_is_val);

			// SET PROPERTY FOR EQUAL
			if (v.ttype == KET_OP && v.op == KEO_LET && isLastLeftBraket) {
				// must set
				for (int i = n_out-1; i > -1; --i) {
					if (out[i].ttype == KET_PROP) {
						out[i].propset = 1;
						out[i].propget = 0;
						break;
					}
				}
			}
			isLastLeftBraket = 0;
			if (!v.realToken) {
				break;
			}
			if (*err) {
				break;
			}

			if (v.ttype == KET_VAL || v.ttype == KET_VCMD || v.ttype == KET_VNAME) {
				u = push_back(sml, &out, &n_out, &m_out);
				*u = v;
				last_is_val = 1;
			} else if (v.ttype == KET_FUNC || v.ttype == KET_PROP || v.ttype == KET_CMD) {
				t = push_back(sml, &op, &n_op, &m_op);
				*t = v;
				last_is_val = 0;
			} else if (v.ttype == KET_OP) {
				int oi = ke_op[v.op];
				while (n_op > 0 && op[n_op-1].ttype == KET_OP) {
					int pre = ke_op[op[n_op-1].op]>>1;
					if (((oi&1) && oi>>1 <= pre) || (!(oi&1) && oi>>1 < pre)) break;
					u = push_back(sml, &out, &n_out, &m_out);
					*u = op[--n_op];
				}
				t = push_back(sml, &op, &n_op, &m_op);
				*t = v;
				last_is_val = 0;
			}
		}
	}

	if (*err == 0) {
		while (n_op > 0 && op[n_op-1].op >= 0) {
			u = push_back(sml, &out, &n_out, &m_out);
			*u = op[--n_op];
		}
		if (n_op > 0) {
			*err |= KEE_UNLP;
		}
	}
	/*
	if (*err == 0) { // then check if the number of args is correct
		int i, n;
		for (i = n = 0; i < n_out; ++i) {
			ke1_t *e = &out[i];
			if (e->ttype == KET_VAL || e->ttype == KET_VCMD || e->ttype == KET_VNAME) ++n;
			else n -= e->n_args - 1;
		}
		if (n != 1) {
			*err |= KEE_ARG;
		}
	}
	*/
	ke_free_memory(sml, s);
	if (op) {
        ke_free_memory(sml, op);
	}
	*_n = n_out;
	return out;
}

void ke_free_val(sml_t *sml) {
	for(int i = 1; i < sml->field_qte; ++i) {
		ke1_t*fieldp = sml->fields[i];
        ke_free_memory(sml, fieldp->name);
		if (fieldp->vtype == KEV_MAT && fieldp->obj.matrix) {
			ke_matrix_freemem(sml,fieldp);
		} else if (fieldp->vtype == KEV_VEC && fieldp->obj.vector) {
			ke_vector_freemem(sml, fieldp);
		} else if (fieldp->vtype == KEV_VEC_INT && fieldp->obj.vector_int) {
			ke_vector_int_freemem(sml, fieldp);
		} else if (fieldp->vtype == KEV_STR && fieldp->obj.s) {
            ke_free_memory(sml, fieldp->obj.s);
		} else if (fieldp->vtype == KEV_DATE && fieldp->obj.s) {
			ke_free_memory(sml, fieldp->obj.date);
		}
        ke_free_memory(sml,fieldp);
    }
	ke_free_memory(sml, sml->fields);
	sml->fields = NULL;

	for (int i = 0; i < sml->rec_qte; ++i) {
		ke1_t*fieldp = sml->recp[i];
		ke_free_memory(sml, fieldp->name);
		ke_free_memory(sml, fieldp->obj.reclist);
		ke_free_memory(sml, fieldp);
	}
}

void ke_free_tokens(sml_t *sml) {
	ke_free_memory(sml, sml->tokens);
}

int ke_fill_list(sml_t *sml, kexpr_t *ke)
{
    sml->tokens = ke_calloc_memory(sml, ke->n * sizeof(ke1_t *),1);
	ke1_t * ne = NULL;
	for (int i = 0; i < ke->n; ++i) {
        ke1_t *tokp = &ke->e[i];
        if (tokp->ttype == KET_VNAME) {
			ne = sml->fields[tokp->ifield];
			if (ne == NULL) {
				char * newname = ke_mystrndup(sml, tokp->name, strlen(tokp->name));
				ne = ke_malloc_memory(sml, sizeof(ke1_t));
				if (ne == NULL) {
					printf("out of memory at ke_fill_list");
					printf("TODO clean up the memory");
					abort();
				}
				if ((tokp->ifield < sml->rec_qte) && (strcmp(sml->recp[tokp->ifield]->name, tokp->name) == 0)) {
					memcpy(ne, sml->recp[tokp->ifield], sizeof(ke1_t));
				}
				else {
					memcpy(ne, tokp, sizeof(ke1_t));
				}
				ne->name = newname;
				sml->fields[tokp->ifield] = ne;
            }
			tokp = ne;
		}
        sml->tokens[i] = tokp;
	}
	return ke_set_ijmp(sml, ke, sml->tokens);
}

void ke_set_int(sml_t* sml, ke1_t *tokp, int64_t y)
{
	double yy = (double)y;
	tokp->i = y, tokp->r = yy, tokp->vtype = KEV_INT, tokp->assigned = 1;
}

void ke_set_real(sml_t* sml, ke1_t *tokp, double x)
{
    tokp->r = x, tokp->vtype = KEV_REAL;
}

void ke_set_null_vector(sml_t *sml, int ifield)
{
	ke1_t * ftokp = sml->fields[ifield];
    ke_vector_freemem(sml, ftokp);
    ftokp->obj.vector = NULL, ftokp->vtype = KEV_REAL, ftokp->ttype = KET_VAL, ftokp->assigned = 0;
}

void ke_set_null_vector_int(sml_t *sml, int ifield)
{
	ke1_t * ftokp = sml->fields[ifield];
	ke_vector_int_freemem(sml,ftokp);
	ftokp->obj.vector_int = NULL, ftokp->vtype = KEV_REAL, ftokp->ttype = KET_VAL, ftokp->assigned = 0;
}

void ke_set_vector(sml_t * sml, ke1_t *tokp, gsl_vector * vecp)
{
    ke_vector_freemem(sml, tokp);
    tokp->obj.vector = vecp, tokp->vtype = KEV_VEC, tokp->ttype = KET_VAL, tokp->assigned = 1;
}

void ke_set_vector_int(sml_t * sml, ke1_t *tokp, gsl_vector_int * vecp)
{
	ke_vector_int_freemem(sml,tokp);
	tokp->obj.vector_int = vecp, tokp->vtype = KEV_VEC_INT, tokp->ttype = KET_VAL, tokp->assigned = 1;
}

void ke_set_date(sml_t * sml, ke1_t *tokp, GDate_t * datep)
{
	if (tokp->obj.date && tokp->vtype == KEV_DATE) {
		free(tokp->obj.date);
	}
	tokp->obj.date = datep;
	tokp->vtype = KEV_DATE;
	tokp->ttype = KET_VAL;
	tokp->assigned = 1;
}

void ke_set_null_matrix(sml_t *sml, int ifield)
{
	ke1_t * f = sml->fields[ifield];

    ke_matrix_freemem(sml, f);
    f->obj.matrix = NULL, f->vtype = KEV_REAL, f->ttype = KET_VAL, f->assigned = 0;
}

void ke_set_matrix(sml_t *sml, ke1_t *tokp, gsl_matrix * matp)
{
    ke_matrix_freemem(sml, tokp);
    tokp->obj.matrix = matp, tokp->vtype = KEV_MAT, tokp->ttype = KET_VAL, tokp->assigned = 1;
}


void ke_set_complex(sml_t *sml, ke1_t *tokp, gsl_complex complex)
{
	tokp->obj.tcomplex = complex, tokp->vtype = KEV_COMPLEX, tokp->assigned = 1;
}

static void ke_set_str_internal(sml_t* sml,ke1_t *tokp, char * tmp) {
    if (tokp->vtype == KEV_STR && tokp->obj.s) {
        ke_free_memory(sml,tokp->obj.s);
    }
    tokp->obj.s = tmp;
    tokp->i = 0, tokp->r = 0, tokp->assigned = 1;
    tokp->vtype = KEV_STR;
}

void ke_set_str(sml_t* sml,ke1_t *tokp, char *x)
{
   char * tmp = ke_mystrndup(sml, x, strlen(x));
   ke_set_str_internal(sml, tokp,tmp);
}

void ke_set_str_direct(sml_t *sml, int ifield, char *x)
{
	ke1_t * e = sml->fields[ifield];
	ke_set_str_internal(sml, e,x);
}

void ke_set_record(sml_t *sml, ke1_t *source, ke1_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.reclist = source->obj.reclist;
}

void ke_set_image(sml_t *sml, ke1_t *source, ke1_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.image = source->obj.image;
}

void ke_set_file(sml_t *sml, ke1_t *source, ke1_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.file = source->obj.file;
}

kexpr_t *ke_parse(sml_t *sml, utf8 *_s, int *err)
{
	int n;
	ke1_t *e;
	kexpr_t *ke;
	if (sml->fields == NULL) {
		sml->fields = ke_calloc_memory(sml, sizeof(ke1_t *) * 1000, 1);
	}
	e = ke_parse_core(sml, _s, &n, err);
	if (*err == 0) {
		ke = (kexpr_t*)ke_calloc_memory(sml, 1, sizeof(kexpr_t));
		ke->n = n, ke->e = e;
		//#ifdef DEBUG
		ke_print(sml,ke);
		//#endif // DEBUG
		//else {} {
		//	g_gbl_fields = realloc(g_gbl_fields, sizeof(ke1_t *) * g_gbl_field_qte);
		//	memset(g_gbl_fields + old_m, 0, (*m - old_m) * sizeof(ke1_t));
		//
		//}
        return ke;
	} else {
		if (*err & KEE_ARG) printf("SML: ERROR: Wrong number of arguments at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_UNFUNC) printf("SML: ERROR: Function name <%s> not found at line <%d>\n", sml->lastErrorMessage, sml->sourceCodeLine);
		if (*err & KEE_UNOP) printf("SML: ERROR: Undefiend operator at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_UNLP) printf("SML: ERROR: Unmatched left parentheses at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_UNRP) printf("SML: ERROR: Unmatched right parentheses at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_UNQU) printf("SML: ERROR: Unmatched quotation marks at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_FUNC) printf("SML: ERROR: Wrong function syntax at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_NUM) printf("SML: ERROR: Failed to parse number at line <%d>\n", sml->sourceCodeLine);
		if (*err & KEE_UNVAR) printf("SML: ERROR: Variable in line <%d>\n", sml->sourceCodeLine);
		ke = (kexpr_t*)ke_calloc_memory(sml, 1, sizeof(kexpr_t));
        ke->n = n, ke->e = e;
        ke_free(sml, ke);
	    return NULL;
	}
}

void ke_print_one_stack(ke1_t * tokp)
{
    if (tokp->ttype == KET_VNAME || tokp->ttype == KET_VCMD || tokp->ttype == KET_VAL ) {
        if (tokp->name != NULL) printf("%s", tokp->name);
        else {
            if (tokp->vtype == KEV_STR) printf("'%s'", tokp->obj.s);
            if (tokp->vtype == KEV_REAL) printf("%g", tokp->r);
            if (tokp->vtype == KEV_INT) printf("%lld", (long long)tokp->i);
			if (tokp->vtype == KEV_DATE) printf("Y:%d M:%d D:%d", g_date_get_year(tokp->obj.date), g_date_get_month(tokp->obj.date), g_date_get_day(tokp->obj.date));
		}
    } else if (tokp->ttype == KET_OP) {
        printf("%s", ke_opstr[tokp->op]);
    } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_PROP || tokp->ttype == KET_CMD) {
        printf("%s(%d)", tokp->name, tokp->n_args);
    }
}

void ke_print_one(sml_t *sml, ke1_t * tokp)
{
	if (tokp->vtype == KEV_REC) {
		for (int i = 0; i < tokp->i; i++) {
			ke1_t * tmptokp = sml->fields[tokp->obj.reclist[i]];
			printf("%s = ",tmptokp->name);
			ke_print_one(sml, tmptokp);
			printf("\n");
		}
	} else if (tokp->vtype == KEV_VEC ) {
		ke_vector_print(sml,tokp);
	}
	else if (tokp->vtype == KEV_VEC_INT) {
		ke_vector_int_print(sml,tokp);
	} else if (tokp->vtype == KEV_MAT) {
		ke_matrix_print(sml,tokp);
	} else if (tokp->vtype == KEV_COMPLEX) {
		ke_complex_print(sml,tokp);
	} else if(tokp->ttype == KET_VNAME || tokp->ttype == KET_VCMD || tokp->ttype == KET_VAL) {
		#ifdef DEBUG
			if (tokp->name != NULL) printf("%s=", tokp->name);
		#endif // DEBUG
        if (tokp->vtype == KEV_REAL) 
			printf("%g", tokp->r);
        else if (tokp->vtype == KEV_INT) 
			printf("%lld", (long long)tokp->i);
        else if (tokp->vtype == KEV_STR && tokp->obj.s != NULL) 
            printf("%s", tokp->obj.s);
        else if (tokp->vtype == KEV_DATE && tokp->obj.date != NULL) 
			 printf("dd-mm-yyyy: %d-%d-%d", g_date_get_day(tokp->obj.date), g_date_get_month(tokp->obj.date), g_date_get_year(tokp->obj.date));
	} else if (tokp->tofree == 1) {
		if (tokp->vtype == KEV_STR) {
			ke_free_memory(sml, tokp->obj.s);
		}
	}
    #ifdef DEBUG
        if (tokp->ttype == KET_OP) {
            printf("%s ", ke_opstr[tokp->op]);
        } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_CMD || tokp->ttype == KET_PROP) {
            printf("%s(%d) ", tokp->name, tokp->n_args);
        }
    #endif // DEBUG
}

void ke_print_stack(sml_t* sml, ke1_t *tokp, int top) { 
	ke1_t *stack = sml->stack;
    if (top > 3) {
        printf("\n%s", "*****************************");
        printf("\n%s\n", "Stack has more than one value");
        while(top) {
            ke_print_one(sml, stack);
            putchar(' ');
            stack++;
            top--;
        }
        printf("\n%s\n", "*****************************");
    }
}

void ke_print_range(kexpr_t *kexpr, int starti, int endi)
{
	int i;
	if (kexpr == 0) return;
	for (i = (int)fmax(0,starti); i < (int)fmin(kexpr->n,endi) ; ++i) {
		ke1_t *tokp = &kexpr->e[i];
		if (i) putchar(' ');
		ke_print_one_stack(tokp);
	}
	putchar('\n');
}

void ke_print_error_one(sml_t * sml,kexpr_t *kexpr, char * name, ke1_t * e, int itok) {
    printf("\n*** ERROR for function <%s> at %d with the following value\n", name, itok);
    ke1_t *tokp = &kexpr->e[itok];
    printf("    ");
    ke_print_one(sml, tokp);
    printf("\n    ");
    ke_print_range(kexpr, itok - 3, itok + 3);
    printf("****\n");
}

void ke_print(sml_t* sml, kexpr_t *kexpr)
{
	int i;
	if (kexpr == 0) return;
	for (i = 0; i < kexpr->n; ++i) {
		ke1_t *tokp = &kexpr->e[i];
		if (i) putchar(' ');
		ke_print_one_stack(tokp);
	}
	putchar('\n');
}

void ke_hash_add(sml_t *sml, fncp key, char * name) {
    int absent;
    khint_t iter = kh_put(5, sml->hfunction, name, &absent);
    kh_val(sml->hfunction, iter) = (ke1_p)key;
}

void ke_fill_hash(sml_t *sml) {
    sml->hfunction = kh_init(5); ke_inc_memory(sml);
	sml->hname = kh_init(6); ke_inc_memory(sml);
	ke_command_hash(sml);
    ke_vector_hash(sml);
	ke_file_hash(sml);
	ke_complex_hash(sml);
    ke_matrix_hash(sml);
    ke_function_hash(sml);
    ke_str_hash(sml);
	ke_date_hash(sml);
	//ke_constants_hash(sml);
}

ke1_t* ke_get_val_index(sml_t *sml, int i) {
	return sml->tokens[i];
}

void ke_set_val_index(sml_t *sml, int i, ke1_t *tokp) {
	ke_set_val(sml, sml->tokens[i], tokp);
}

void inline ke_set_val(sml_t* sml, ke1_t* e, ke1_t *q) {
	 e->vtype = q->vtype;
	 if (q->vtype == KEV_INT)  e->i = q->i; //, e->assigned = 1; //double yy = (double)q->i; e->r = yy,
	 else if (q->vtype == KEV_REAL)	e->r = q->r;  //e->i = (int64_t)e->r; e->assigned = 1;
	 else if (q->vtype == KEV_STR) 	ke_set_str(sml, e, q->obj.s);
	 else if (q->vtype == KEV_VEC)	ke_set_vector(sml, e, q->obj.vector);
	 else if (q->vtype == KEV_VEC_INT)	ke_set_vector_int(sml, e, q->obj.vector_int);
	 else if (q->vtype == KEV_MAT) 	ke_set_matrix(sml, e, q->obj.matrix);
	 else if (q->vtype == KEV_COMPLEX) ke_set_complex(sml, e, q->obj.tcomplex);
	 else if (q->vtype == KEV_REC) ke_set_record(sml, q, e);
	 else if (q->vtype == KEV_IMAGE) ke_set_image(sml, q, e);
	 else if (q->vtype == KEV_FILE) ke_set_file(sml, q, e);
	 else if (q->vtype == KEV_DATE) ke_set_date(sml, e, q->obj.date);
	 else {
        printf("\n->*** ERROR: %s:%s\n\n", "Error: Invalid type ", e->name);
     }
	 if (q->tofree == 1) {
		 if (q->vtype == KEV_STR) {
			 ke_free_memory(sml, q->obj.s);
			 q->tofree = 0;
		 }
	 }
}

int ke_eval(sml_t *sml, kexpr_t *kexpr, int64_t *_i, double *_r, char **_p, int *ret_type)
{
	ke1_t *p, *q, *e;
	int top = 0, err = 0;
	*_i = 0, *_r = 0., *ret_type = 0;
	sml->stack = (ke1_t*)ke_malloc_memory(sml, kexpr->n * sizeof(ke1_t));
	ke1_t *stack = sml->stack;
	struct ke1_s ** fields = sml->fields;
	struct ke1_s ** tokens = sml->tokens;

	ke1_t *tokp = NULL;
	int n = kexpr->n;
	for (int itok = 0; itok < n; ++itok) {
		sml->tok_idx = itok;
		//ke1_t *e = &ke->e[i];
		tokp = tokens[itok];

		switch (tokp->ttype) {
		case KET_CMD:
			top = (tokp->f.defcmd)(sml, kexpr, tokp, top, &itok);
			break;
		case KET_VCMD:
			itok = (tokp->f.defvcmd)(sml, kexpr, tokp, itok);
			break;
		case KET_OP:
			if (tokp->op == KEO_NOP) continue;
			if (tokp->op == KEO_LET && tokp->n_args == 2) {
				if (stack[top - 2].propset) {
					e = &stack[top - 2];
					fields[e->ifield]->n_args = e->n_args;
					stack[top-2] = *fields[e->ifield];
					top = ke_poperty_set(sml, stack, e, top);
				}
				else {
					q = &stack[--top];
					p = &stack[--top];
					ke_set_val(sml, fields[p->ifield], q);
				}
			}
			else {
				if (tokp->n_args == 2) {
					q = &stack[--top];
					p = &stack[top-1];
					tokp->f.builtin(p, q);

				}
				else {
					p = &stack[top - 1];
					tokp->f.builtin(&stack[top - 1], 0);
				}
			}
			break;
		case KET_FUNC:
			if (tokp->n_args == 0) {
				// always need something in the stack
				stack[top++] = *tokp;
			}
			top = (tokp->f.deffunc)(sml, tokp, top);
			break;
		case KET_PROP:
			if (tokp->propget) {
				stack[top++] = *fields[tokp->ifield];
				top = ke_poperty_get(sml, stack, tokp, top);
			}
			else {
				stack[top++] = *tokp;
			}
			break;
		default:
			stack[top++] = *tokp;
			break;
		}
	}

	#ifdef DEBUG
        ke_print_stack(sml, tokp, top);
    #endif // DEBUG

	*ret_type = stack->vtype;
	*_i = stack->i, *_r = stack->r, *_p = stack->obj.s;

	#ifdef DEBUG
        printf("= ");
        ke_print_one(sml, tokp);
        printf("\n");
    #endif // DEBUG

	ke_free_memory(sml, sml->stack);
	return err;
}

void ke_push_stack(sml_t * sml, ke1_t * tokp, int *top) {
	sml->stack[*top] = *tokp;
    (*top)++;
}

/*
void ke_free_dll() {
	for (int i = 0; i < g_libhandle_qte; ++i) {
		FreeLibrary(g_libhandle[i]);
	}
}
*/

/*
void ke_free_image() {
	for (int i = 0; i < g_libhandle_qte; ++i) {
		FreeLibrary(g_libhandle[i]);
	}
}
*/

void ke_destroy(sml_t *sml, kexpr_t *kexpr)
{
    ke_free(sml, kexpr);
	ke_free_hash(sml);
	//ke_free_dll();
}

void ke_free_hash(sml_t *sml)
{
	kh_destroy(5, sml->hfunction);	ke_dec_memory(sml);
	kh_destroy(6, sml->hname); ke_dec_memory(sml);
	//ke_constants_destroy();
	ke_command_destroy(sml);

	if (sml->mem_count != 0) {
        printf("%s:%d", "Not all memory has been deallocated", sml->mem_count);
	}
}

void ke_free(sml_t* sml,kexpr_t *kexpr)
{
    int i;
	for (i = 0; i < kexpr->n; ++i) {
		ke1_t *e = &kexpr->e[i];
		if (e->vtype == KEV_STR && e->obj.s) {
            ke_free_memory(sml, e->obj.s);
            e->obj.s = NULL;
		}

		if (e->name) {
            ke_free_memory(sml, e->name);
            e->name = NULL;
        }
	}
	ke_free_memory(sml ,kexpr->e);
	ke_free_memory(sml, kexpr);
}

