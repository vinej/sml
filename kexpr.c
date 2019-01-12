#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include "kexpr.h"
#include "kdq.h"
#include "khash.h"
#include "vector.h"
#include "matrix.h"
#include "complex.h"
#include "plot.h"
#include "constants.h"
#include "function.h"
#include "str.h"
#include "command.h"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_vector.h>
#include <stdio.h>

// GLOBAL VARIABLE USED BY ALL FUNCTIONS
ke1_t ** g_gbl_fields = NULL; // array of all global fields of the program to exectue
int g_gbl_field_qte = 1;      // number of global fields  
int g_tok_idx = 0;            // current program token index
ke1_t *g_stack = NULL;        // stack for the evaluation of the program
ke1_t ** g_tokens = NULL;     // array of pointers of all program tokens
int g_mem_count = 0;          // current count of memory allocation

// parser variables
int g_isNextDefName = 0;    // flag to indicate that the next token is the def name
char g_currentDefName[100]; // current name of the current function
int g_sourceCodeLine = 1;  // use to keep the line number to show better error trapping
int g_isFirstToken = 1;    // use to remove separators at the beginning of the program
int g_isLastTokenNop = 0;  // use to manage the command seperator, the rule is to have one separator between each comand
char g_lastErrorMessage[256]; // 
							//---------------------


// KHASH MAP STR DEFINITION
const int KH_FNC = 5;
const int KH_FIELD = 6;
KHASH_MAP_INIT_STR(KH_FNC, ke1_p)
KHASH_MAP_INIT_STR(KH_FIELD, int)
khash_t(KH_FNC) *hfunction;
khash_t(KH_FIELD) *hname;

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

// MEMORY ALLOCATION FUNCTIONS
void ke_init_memory_count() {
    g_mem_count = 0;
}

void inline ke_inc_memory() {
    ++g_mem_count;
}

void inline ke_dec_memory() {
    --g_mem_count;
}

inline void * ke_calloc_memory(size_t i, size_t x) {
    void * tmp = calloc(i,x);
    ++g_mem_count;
    return tmp;
}

inline void * ke_malloc_memory(size_t i) {
    void * tmp = malloc(i);
    ++g_mem_count;
    return tmp;
}

void ke_free_memory(void * m) {
	free(m);
	--g_mem_count;
}

inline char *ke_mystrndup(char *src, size_t n)
{
	char *dst;
	dst = (char*)calloc(n + 1, 1);
	++g_mem_count;
	strncpy(dst, src, n);
	return dst;
}

//****************************


// CURENT TOKEN FUNCTION
ke1_t * ke_get_tok() {
	return g_tokens[g_tok_idx];
}


ke1_t * ke_get_tokidx(int idx) {
	return g_tokens[idx];
}
//*********************************


fncp ke_function(char * name) {
	khint_t iter = kh_get(KH_FNC, hfunction, name);
	if (kh_end(hfunction) != iter) {
		return (fncp)kh_val(hfunction, iter);
	}
	else {
		return NULL;
	}
}

// parse a token except "(", ")" and ","
ke1_t ke_read_token(char *p, char **r, int *err, int last_is_val) // it doesn't parse parentheses
{
	ke1_t tok;
	memset(&tok, 0, sizeof(ke1_t));
	tok.realToken = 1;
	while (1) { // not a real token
		// comment
		if (*p == 0) {
			// fin du code pas d'autre tolen
			tok.realToken = 0;
			return tok;
		}

		if (*p == '#') {
			++p;
			while (*p != 0) {
				if (*p == '\n') {
					++g_sourceCodeLine;
					break;
				}
				++p;
			}
			if (*p == 0) {
				// fin du code pas d'autre tolen
				tok.realToken = 0;
				return tok;
			}
			continue;
		}

		if (*p == '\n') {
			++g_sourceCodeLine;
			if (g_isLastTokenNop || g_isFirstToken) {
				++p;
				continue;
			}
			*p = ';';
			break;
		}

		if (*p == ';') {
			if (g_isLastTokenNop || g_isFirstToken) {
				++p;
				continue;
			}
			break;
		}

		// do not accept control caracter and space
		if (*p <= 32) {
			++p;
			continue;
		}

		// line continuation
		if (*p == '\\') {
			++p;
			while (*p != 0) {
				if (*p == '\n') {
					++g_sourceCodeLine;
					++p;
					break;
				}
				++p;
			}
			if (*p == 0) {
				// fin du code pas d'autre tolen
				tok.realToken = 0;
				return tok;
			}
			continue;
		}

		// ok we can continue with a real token
		break;
	}

	if (*p == 0) {
		// fin du code pas d'autre tolen
		tok.realToken = 0;
		return tok;
	}

	char *q = p;
	g_isLastTokenNop = 0;
	tok.sourceLine = g_sourceCodeLine;
	if (isalpha(*p) || *p == '_') { // a variable or a function
		for (; *p && (*p == '_' || *p == '.' || isalnum(*p)); ++p);
		tok.name = ke_mystrndup(q, p - q);
		while (*p) { if (*p == ' ') ++p; else break; }  // pass over the spaces
		if (*p == '(') {
			// find 
			tok.n_args = 1;
			tok.sourceLine = g_sourceCodeLine;
			tok.f.defcmd = (cmdp)ke_command(tok.name);
			if (tok.f.defcmd != NULL) {
				tok.icmd = ke_command_icmd(tok.name);
				tok.ttype = KET_CMD;
				if (strcmp(tok.name, CMD_DEF) == 0) {
					g_isNextDefName = 1;
				}
				tok.ijmp = 0;
			} else {
				tok.f.deffunc = (fncp)ke_function(tok.name);
				if (tok.f.deffunc != NULL) {
					tok.ttype = KET_FUNC;
				}
				else {
					strcpy(g_lastErrorMessage, tok.name);
					*err |= KEE_UNFUNC;
				}
			}
		}
		else {
			tok.r = ke_constants(tok.name);
			if (tok.r != 0) {
				tok.ttype = KET_VAL;
			}
			else {
				tok.f.defvcmd = (vcmdp)ke_command_val(tok.name);
				if (tok.f.defvcmd != NULL) {
					if (strcmp(tok.name, CMD_RTN) == 0) {
						*g_currentDefName = 0;
					}
					tok.ttype = KET_VCMD;
					tok.icmd = ke_command_icmd(tok.name);
				}
				else {
					tok.ttype = KET_VNAME;
					if (g_isNextDefName) {
						strcpy(g_currentDefName, tok.name);
						g_isNextDefName = 0;
						tok.islocal = 1;
					}
					else {
						if (*g_currentDefName != 0) {
							char * localName = ke_calloc_memory(strlen(tok.name) + strlen(g_currentDefName) + 3, 1);
							strcpy(localName, g_currentDefName);
							strcat(localName, "__");
							strcat(localName, tok.name);
							ke_free_memory(tok.name);
							tok.name = localName;
							tok.islocal = 1;
						}
						else {
							tok.islocal = 0;
						}
					}
					khint_t iter = kh_get(KH_FIELD, hname, tok.name);
					if (kh_end(hname) != iter) {
						tok.ifield = kh_val(hname, iter);
					}
					else {
						int absent;
						khint_t iter = kh_put(KH_FIELD, hname, tok.name, &absent);
						kh_val(hname, iter) = g_gbl_field_qte;
						tok.ifield = g_gbl_field_qte;
						g_gbl_field_qte++;
					}
				}
				tok.i = 0, tok.r = 0.;
			}
			tok.vtype = KEV_REAL;

		}
		*r = p;
	} else if (isdigit(*p) || *p == '.') { // a number
		long x;
		double y;
		char *pp;
		tok.ttype = KET_VAL;
		y = strtod(q, &p);
		x = strtol(q, &pp, 0); // FIXME: check int/double parsing errors
		if (q == p && q == pp) { // parse error
			*err |= KEE_NUM;
		} else if (p > pp) { // has "." or "[eE]"; then it is a real number
			tok.vtype = KEV_REAL;
			tok.i = (int64_t)(y + .5), tok.r = y;
			*r = p;
		} else {
			tok.vtype = KEV_INT;
			tok.i = x, tok.r = y;
			*r = pp;
		}

	} else if (*p == '"' || *p == '\'') { // a string value
		int c = *p;
		for (++p; *p && *p != c; ++p)
			if (*p == '\\') ++p; // escaping
		if (*p == c) {
			tok.ttype = KET_VAL, tok.vtype = KEV_STR;
			tok.obj.s = ke_mystrndup(q + 1, p - q - 1);
			*r = p + 1;
		}
		else
		{
			*err |= KEE_UNQU, *r = p;
		}
	} else { // an operator
		tok.ttype = KET_OP;
		if (*p == '*' && p[1] == '*') tok.op = KEO_POW, tok.f.builtin = ke_op_KEO_POW, tok.n_args = 2, *r = q + 2;
		else if (*p == '*') tok.op = KEO_MUL, tok.f.builtin = ke_op_KEO_MUL, tok.n_args = 2, *r = q + 1; // FIXME: NOT working for unary operators
		else if (*p == '/' && p[1] == '/') tok.op = KEO_IDIV, tok.f.builtin = ke_op_KEO_IDIV, tok.n_args = 2, *r = q + 2;
		else if (*p == '/') tok.op = KEO_DIV, tok.f.builtin = ke_op_KEO_DIV, tok.n_args = 2, *r = q + 1;
		else if (*p == '%') tok.op = KEO_MOD, tok.f.builtin = ke_op_KEO_MOD, tok.n_args = 2, *r = q + 1;
		else if (*p == '+') {
			if (last_is_val) tok.op = KEO_ADD, tok.f.builtin = ke_op_KEO_ADD, tok.n_args = 2;
			else tok.op = KEO_POS, tok.f.builtin = ke_op_KEO_POS, tok.n_args = 1;
			*r = q + 1;
		} else if (*p == '-') {
			if (last_is_val) tok.op = KEO_SUB, tok.f.builtin = ke_op_KEO_SUB, tok.n_args = 2;
			else tok.op = KEO_NEG, tok.f.builtin = ke_op_KEO_NEG, tok.n_args = 1;

			*r = q + 1;
		}
		else if (*p == '=' && p[1] != '=') tok.op = KEO_LET, tok.f.builtin = ke_op_KEO_EQ, tok.n_args = 2, *r = q + 1;
		else if (*p == '=' && p[1] == '=') tok.op = KEO_EQ, tok.f.builtin = ke_op_KEO_EQ, tok.n_args = 2, *r = q + 2;
		else if (*p == '!' && p[1] == '=') tok.op = KEO_NE, tok.f.builtin = ke_op_KEO_NE, tok.n_args = 2, *r = q + 2;
		else if (*p == '<' && p[1] == '>') tok.op = KEO_NE, tok.f.builtin = ke_op_KEO_NE, tok.n_args = 2, *r = q + 2;
		else if (*p == '>' && p[1] == '=') tok.op = KEO_GE, tok.f.builtin = ke_op_KEO_GE, tok.n_args = 2, *r = q + 2;
		else if (*p == '<' && p[1] == '=') tok.op = KEO_LE, tok.f.builtin = ke_op_KEO_LE, tok.n_args = 2, *r = q + 2;
		else if (*p == '>' && p[1] == '>') tok.op = KEO_RSH, tok.f.builtin = ke_op_KEO_RSH, tok.n_args = 2, *r = q + 2;
		else if (*p == '<' && p[1] == '<') tok.op = KEO_LSH, tok.f.builtin = ke_op_KEO_LSH, tok.n_args = 2, *r = q + 2;
		else if (*p == '>') tok.op = KEO_GT, tok.f.builtin = ke_op_KEO_GT, tok.n_args = 2, *r = q + 1;
		else if (*p == '<') tok.op = KEO_LT, tok.f.builtin = ke_op_KEO_LT, tok.n_args = 2, *r = q + 1;
		else if (*p == '|' && p[1] == '|') tok.op = KEO_LOR, tok.f.builtin = ke_op_KEO_LOR, tok.n_args = 2, *r = q + 2;
		else if (*p == '&' && p[1] == '&') tok.op = KEO_LAND, tok.f.builtin = ke_op_KEO_LAND, tok.n_args = 2, *r = q + 2;
		else if (*p == '|') tok.op = KEO_BOR, tok.f.builtin = ke_op_KEO_BOR, tok.n_args = 2, *r = q + 1;
		else if (*p == '&') tok.op = KEO_BAND, tok.f.builtin = ke_op_KEO_BAND, tok.n_args = 2, *r = q + 1;
		else if (*p == '^') tok.op = KEO_BXOR, tok.f.builtin = ke_op_KEO_BXOR, tok.n_args = 2, *r = q + 1;
		else if (*p == '~') tok.op = KEO_BNOT, tok.f.builtin = ke_op_KEO_BNOT, tok.n_args = 1, *r = q + 1;
		else if (*p == '!') tok.op = KEO_LNOT, tok.f.builtin = ke_op_KEO_LNOT, tok.n_args = 1, *r = q + 1;
		else if (*p == ';') tok.op = KEO_NOP, g_isLastTokenNop = 1, tok.f.builtin = ke_op_KEO_NOP, tok.n_args = 2, *r = q + 1;
		else {
			tok.ttype = KET_NULL, *err |= KEE_UNOP;
		}

		g_isFirstToken = 0;
	}
	return tok;
}

static inline ke1_t *push_back(ke1_t **a, int *n, int *m)
{
	if (*n == *m) {
		int old_m = *m;
		*m = *m? *m<<1 : 8;
		// first timae allocation
		if (*a == 0) ke_inc_memory();
		*a = (ke1_t*)realloc(*a, *m * sizeof(ke1_t));
		memset(*a + old_m, 0, (*m - old_m) * sizeof(ke1_t));
	}
	return &(*a)[(*n)++];
}

ke1_t *ke_parse_core(char *_s, int *_n, int *err)
{
    char * p;
	int n_out, m_out, n_op, m_op, last_is_val = 0;
	ke1_t *out, *op, *t, *u;

	// remove remark, and normalize line seperator
	char *s;
	s = ke_mystrndup(_s, strlen(_s)); // make a copy
	//ke_normalize_code(s);
	*err = 0;
 	*_n = 0;
	out = op = 0;
	n_out = m_out = n_op = m_op = 0;
	p = s;
	while (*p) {
		while (1) {
			if (*p == ' ') ++p; else break;
		}

		if (*p == '(') {
			t = push_back(&op, &n_op, &m_op); // push to the operator g_stack
			t->op = -1, t->ttype = KET_NULL; // ->op < 0 for a left parenthsis
			++p;
		} else if (*p == ')') {
			while (n_op > 0 && op[n_op-1].op >= 0) { // move operators to the output until we see a left parenthesis
				u = push_back(&out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op == 0) { // error: extra right parenthesis
				*err |= KEE_UNRP;
				break;
			} else --n_op; // pop out '('
			if (n_op > 0 && (op[n_op-1].ttype == KET_FUNC || op[n_op - 1].ttype == KET_CMD)) { // the top of the operator g_stack is a function
				u = push_back(&out, &n_out, &m_out); // move it to the output
				*u = op[--n_op];
				if (u->n_args == 1 && strcmp(u->name, "abs") == 0) u->f.builtin = ke_func1_abs;
			}
			++p;
		} else if (*p == ',') { // function arguments separator
			while (n_op > 0 && op[n_op-1].op >= 0) {
				u = push_back(&out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op < 2 || (op[n_op-2].ttype != KET_FUNC && op[n_op- 2].ttype != KET_CMD)) { // we should at least see a function and a left parenthesis
				*err |= KEE_FUNC;
				break;
			}
			++op[n_op-2].n_args;
			++p;
		} else { // output-able token
			ke1_t v = ke_read_token(p, &p, err, last_is_val);
			if (!v.realToken) {
				break;
			}
			if (*err) {
				break;
			}
			if (v.ttype == KET_VAL || v.ttype == KET_VCMD || v.ttype == KET_VNAME) {
				u = push_back(&out, &n_out, &m_out);
				*u = v;
				last_is_val = 1;
			} else if (v.ttype == KET_FUNC || v.ttype == KET_CMD) {
				t = push_back(&op, &n_op, &m_op);
				*t = v;
				last_is_val = 0;
			} else if (v.ttype == KET_OP) {
				int oi = ke_op[v.op];
				while (n_op > 0 && op[n_op-1].ttype == KET_OP) {
					int pre = ke_op[op[n_op-1].op]>>1;
					if (((oi&1) && oi>>1 <= pre) || (!(oi&1) && oi>>1 < pre)) break;
					u = push_back(&out, &n_out, &m_out);
					*u = op[--n_op];
				}
				t = push_back(&op, &n_op, &m_op);
				*t = v;
				last_is_val = 0;
			}
		}
	}

	if (*err == 0) {
		while (n_op > 0 && op[n_op-1].op >= 0) {
			u = push_back(&out, &n_out, &m_out);
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
	ke_free_memory(s);
	if (op) {
        ke_free_memory(op);
	}
	*_n = n_out;
	return out;
}

void ke_free_val() {
	for(int i = 1; i < g_gbl_field_qte; ++i) {
		ke1_t*fieldp = g_gbl_fields[i];
        ke_free_memory(fieldp->name);
		if (fieldp->vtype == KEV_MAT && fieldp->obj.matrix) {
			ke_matrix_freemem(fieldp);
		} else if (fieldp->vtype == KEV_VEC && fieldp->obj.vector) {
			ke_vector_freemem(fieldp);
		} else if (fieldp->vtype == KEV_STR && fieldp->obj.s) {
            ke_free_memory(fieldp->obj.s);
		}
        ke_free_memory(fieldp);
    }
	ke_free_memory(g_gbl_fields);
	g_gbl_fields = NULL;
}

void ke_free_tokens() {
	ke_free_memory(g_tokens);
}

int ke_fill_list(kexpr_t *ke)
{
    g_tokens = ke_calloc_memory(ke->n * sizeof(ke1_t *),1);
	ke1_t * ne = NULL;
	for (int i = 0; i < ke->n; ++i) {
        ke1_t *tokp = &ke->e[i];
        if (tokp->ttype == KET_VNAME) {
			ne = g_gbl_fields[tokp->ifield];
			if (ne == NULL) {
                char * newname = ke_mystrndup(tokp->name, strlen(tokp->name));
                ne = ke_malloc_memory(sizeof(ke1_t));
                memcpy(ne, tokp, sizeof(ke1_t));
                ne->name = newname;
				g_gbl_fields[tokp->ifield] = ne;
            }
			tokp = ne;
		}
        g_tokens[i] = tokp;
	}
	return ke_set_ijmp(ke, g_tokens);
}

void ke_set_int(ke1_t *tokp, int64_t y)
{
	double yy = (double)y;
	tokp->i = y, tokp->r = yy, tokp->vtype = KEV_INT, tokp->assigned = 1;
}

void ke_set_real(ke1_t *tokp, double x)
{
    tokp->r = x, tokp->vtype = KEV_REAL;
}

void ke_set_null_vector(int ifield)
{
	ke1_t * ftokp = g_gbl_fields[ifield];
    ke_vector_freemem(ftokp);
    ftokp->obj.vector = NULL, ftokp->vtype = KEV_REAL, ftokp->ttype = KET_VAL, ftokp->assigned = 0;
}

void ke_set_vector(ke1_t *tokp, gsl_vector * vecp)
{
    ke_vector_freemem(tokp);
    tokp->obj.vector = vecp, tokp->vtype = KEV_VEC, tokp->ttype = KET_VAL, tokp->assigned = 1;
}

void ke_set_null_matrix(int ifield)
{
	ke1_t * f = g_gbl_fields[ifield];

    ke_matrix_freemem(f);
    f->obj.matrix = NULL, f->vtype = KEV_REAL, f->ttype = KET_VAL, f->assigned = 0;
}

void ke_set_matrix(ke1_t *tokp, gsl_matrix * matp)
{
    ke_matrix_freemem(tokp);
    tokp->obj.matrix = matp, tokp->vtype = KEV_MAT, tokp->ttype = KET_VAL, tokp->assigned = 1;
}


void ke_set_complex(ke1_t *tokp, gsl_complex complex)
{
	tokp->obj.complex = complex, tokp->vtype = KEV_COMPLEX, tokp->assigned = 1;
}

static void ke_set_str_internal(ke1_t *tokp, char * tmp) {
    if (tokp->vtype == KEV_STR && tokp->obj.s) {
        ke_free_memory(tokp->obj.s);
    }
    tokp->obj.s = tmp;
    tokp->i = 0, tokp->r = 0, tokp->assigned = 1;
    tokp->vtype = KEV_STR;
}

void ke_set_str(ke1_t *tokp, char *x)
{
   char * tmp = ke_mystrndup(x, strlen(x));
   ke_set_str_internal(tokp,tmp);
}

void ke_set_str_direct(int ifield, char *x)
{
	ke1_t * e = g_gbl_fields[ifield];
	ke_set_str_internal(e,x);
}

kexpr_t *ke_parse(char *_s, int *err)
{
	int n;
	ke1_t *e;
	kexpr_t *ke;
	e = ke_parse_core(_s, &n, err);
	if (*err == 0) {
		ke = (kexpr_t*)ke_calloc_memory(1, sizeof(kexpr_t));
		ke->n = n, ke->e = e;
		//#ifdef DEBUG
		ke_print(ke);
		//#endif // DEBUG
		if (g_gbl_fields == NULL) {
			g_gbl_fields = ke_calloc_memory(sizeof(ke1_t *) * 1000, 1);
		}
		//else {} {
		//	g_gbl_fields = realloc(g_gbl_fields, sizeof(ke1_t *) * g_gbl_field_qte);
		//	memset(g_gbl_fields + old_m, 0, (*m - old_m) * sizeof(ke1_t));
		//
		//}
        return ke;
	} else {
		if (*err & KEE_ARG) printf("SML: ERROR: Wrong number of arguments at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_UNFUNC) printf("SML: ERROR: Function name <%s> not found at line <%d>\n", g_lastErrorMessage, g_sourceCodeLine);
		if (*err & KEE_UNOP) printf("SML: ERROR: Undefiend operator at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_UNLP) printf("SML: ERROR: Unmatched left parentheses at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_UNRP) printf("SML: ERROR: Unmatched right parentheses at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_UNQU) printf("SML: ERROR: Unmatched quotation marks at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_FUNC) printf("SML: ERROR: Wrong function syntax at line <%d>\n", g_sourceCodeLine);
		if (*err & KEE_NUM) printf("SML: ERROR: Failed to parse number at line <%d>\n", g_sourceCodeLine);
		ke = (kexpr_t*)ke_calloc_memory(1, sizeof(kexpr_t));
        ke->n = n, ke->e = e;
        ke_free(ke);
	    return NULL;
	}
}

void ke_print_one_stack(ke1_t * tokp)
{
    if (tokp->ttype == KET_VNAME || tokp->ttype == KET_VCMD || tokp->ttype == KET_VAL) {
        if (tokp->name != NULL) printf("%s", tokp->name);
        else {
            if (tokp->vtype == KEV_STR) printf("'%s'", tokp->obj.s);
            if (tokp->vtype == KEV_REAL) printf("%g", tokp->r);
            if (tokp->vtype == KEV_INT) printf("%lld", (long long)tokp->i);
        }
    } else if (tokp->ttype == KET_OP) {
        printf("%s", ke_opstr[tokp->op]);
    } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_CMD) {
        printf("%s(%d)", tokp->name, tokp->n_args);
    }
}

void ke_print_one(ke1_t * tokp)
{
	if (tokp->vtype == KEV_VEC ) {
		ke_vector_print(tokp);
	}
	else if (tokp->vtype == KEV_MAT) {
		ke_matrix_print(tokp);
	} else if (tokp->vtype == KEV_COMPLEX) {
		ke_complex_print(tokp);
	} else if(tokp->ttype == KET_VNAME || tokp->ttype == KET_VCMD || tokp->ttype == KET_VAL) {
		#ifdef DEBUG
			if (tokp->name != NULL) printf("%s=", tokp->name);
		#endif // DEBUG
        if (tokp->obj.s == NULL) {
            if (tokp->vtype == KEV_REAL) printf("%g", tokp->r);
            if (tokp->vtype == KEV_INT) printf("%lld", (long long)tokp->i);
        } else {
            printf("%s", tokp->obj.s);
        }
    }

    #ifdef DEBUG
        if (tokp->ttype == KET_OP) {
            printf("%s ", ke_opstr[tokp->op]);
        } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_CMD) {
            printf("%s(%d) ", tokp->name, tokp->n_args);
        }
    #endif // DEBUG
}

void ke_print_stack(ke1_t *stack, int top) {
    if (top > 1) {
        printf("\n%s", "*****************************");
        printf("\n%s\n", "Stack has more than one value");
        while(top) {
            ke_print_one(stack);
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

void ke_print_error_one(kexpr_t *kexpr, char * name, ke1_t * e, int itok) {
    printf("\n*** ERROR for function <%s> at %d with the following value\n", name, itok);
    ke1_t *tokp = &kexpr->e[itok];
    printf("    ");
    ke_print_one(tokp);
    if (tokp != tokp) {
        printf("    ");
        ke_print_one(tokp);
    }
    printf("\n    ");
    ke_print_range(kexpr, itok - 3, itok + 3);
    printf("****\n");
}

void ke_print(kexpr_t *kexpr)
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

void ke_hash_add(fncp key, char * name) {
    int absent;
    khint_t iter = kh_put(KH_FNC, hfunction, name, &absent);
    kh_val(hfunction, iter) = (ke1_p)key;
}

void ke_fill_hash() {
    hfunction = kh_init(KH_FNC); ke_inc_memory();
	hname = kh_init(KH_FIELD); ke_inc_memory();
	ke_command_hash();
    ke_vector_hash();
	ke_plot_hash();
	ke_complex_hash();
    ke_matrix_hash();
    ke_function_hash();
    ke_str_hash();
    //ke_constants_hash();
}

ke1_t* ke_get_val_index(int i) {
	return g_tokens[i];
}

void ke_set_val_index(int i, ke1_t *tokp) {
	ke_set_val(g_tokens[i], tokp);
}

void inline ke_set_val(ke1_t* e, ke1_t *q) {
	 e->vtype = q->vtype;
     if (q->vtype == KEV_INT)  e->i = q->i; //, e->assigned = 1; //double yy = (double)q->i; e->r = yy,
	 else if (q->vtype == KEV_REAL)	e->r = q->r;  //e->i = (int64_t)e->r; e->assigned = 1;
	 else if (q->vtype == KEV_STR) 	ke_set_str(e, q->obj.s);
	 else if (q->vtype == KEV_VEC)	ke_set_vector(e, q->obj.vector);
	 else if (q->vtype == KEV_MAT) 	ke_set_matrix(e, q->obj.matrix);
	 else if (q->vtype == KEV_COMPLEX) ke_set_complex(e, q->obj.complex);
	 else {
        printf("\n->*** ERROR: %s:%s\n\n", "Error: Invalid type ", e->name);
     }
}

int ke_eval(kexpr_t *kexpr, int64_t *_i, double *_r, char **_p, int *ret_type)
{
	ke1_t *p, *q;
	int top = 0, err = 0;
	*_i = 0, *_r = 0., *ret_type = 0;
	g_stack = (ke1_t*)ke_malloc_memory(kexpr->n * sizeof(ke1_t));
	ke1_t *tokp = NULL;
	int n = kexpr->n;
	for (int itok = 0; itok < n; ++itok) {
		g_tok_idx = itok;
		//ke1_t *e = &ke->e[i];
		tokp = g_tokens[itok];

		switch (tokp->ttype) {
		case KET_CMD:
			top = (tokp->f.defcmd)(kexpr, tokp, g_stack, top, &itok);
			break;
		case KET_VCMD:
			itok = (tokp->f.defvcmd)(kexpr, tokp, itok);
			break;
		case KET_OP:
			if (tokp->op == KEO_NOP) continue;
			if (tokp->op == KEO_LET && tokp->n_args == 2) {
				q = &g_stack[--top];
				p = &g_stack[--top];
				ke_set_val(g_gbl_fields[p->ifield], q);
			}
			else {
				if (tokp->n_args == 2) {
					q = &g_stack[--top];
					p = &g_stack[top-1];
					tokp->f.builtin(p, q);

				}
				else {
					p = &g_stack[top - 1];
					tokp->f.builtin(&g_stack[top - 1], 0);
				}
			}
			break;
		case KET_FUNC:
			top = (tokp->f.deffunc)(g_stack, top);
			break;
		default:
			g_stack[top++] = *tokp;
		}
	}
	#ifdef DEBUG
        ke_print_stack(g_stack, top);
    #endif // DEBUG

	*ret_type = g_stack->vtype;
	*_i = g_stack->i, *_r = g_stack->r, *_p = g_stack->obj.s;

	#ifdef DEBUG
        printf("= ");
        ke_print_one(g_stack);
        printf("\n");
    #endif // DEBUG

	ke_free_memory(g_stack);
	return err;
}

void ke_push_stack(ke1_t * tokp, int *top) {
    g_stack[*top] = *tokp;
    (*top)++;
}

void ke_destroy(kexpr_t *kexpr)
{
    ke_free(kexpr);
	ke_free_hash();
}

void ke_free_hash()
{
	kh_destroy(KH_FNC, hfunction);	ke_dec_memory();
	kh_destroy(KH_FIELD, hname); ke_dec_memory();
	//ke_constants_destroy();
	ke_command_destroy();

	if (g_mem_count != 0) {
        printf("%s:%d", "Not all memory has been deallocated", g_mem_count);
	}
}

void ke_free(kexpr_t *kexpr)
{
    int i;
	for (i = 0; i < kexpr->n; ++i) {
		ke1_t *e = &kexpr->e[i];
		if (e->vtype == KEV_STR && e->obj.s) {
            ke_free_memory(e->obj.s);
            e->obj.s = NULL;
		}
        if (e->name) {
            ke_free_memory(e->name);
            e->name = NULL;
        }
	}
	ke_free_memory(kexpr->e);
	ke_free_memory(kexpr);
}

