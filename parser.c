#include "parser.h"
#include "kexpr.h"
#include "command.h"
#include "constants.h"
#include <math.h>

#if defined(_MSC_VER) || defined(_WIN32)
#include <Windows.h>
#endif

/**********************
* Operator functions *
**********************/

#define KE_GEN_CMP(_type, _op) \
	static void ke_op_##_type(token_t *p, token_t *q, token_t *out) { \
		if (p->vtype == KEV_STR && q->vtype == KEV_STR) out->i = (strcmp(p->obj.s, q->obj.s) _op 0); \
		else out->i = p->vtype == KEV_REAL || q->vtype == KEV_REAL? (p->r _op q->r) : (p->i _op q->i); \
		out->r = (double)out->i; \
		out->vtype = KEV_INT; \
		out->ttype = KET_VAL; \
	}

KE_GEN_CMP(KEO_LT, <)
	KE_GEN_CMP(KEO_LE, <= )
	KE_GEN_CMP(KEO_GT, >)
	KE_GEN_CMP(KEO_GE, >= )
	KE_GEN_CMP(KEO_EQ, == )
	KE_GEN_CMP(KEO_NE, != )

#define KE_GEN_BIN_INT(_type, _op) \
	static void ke_op_##_type(token_t *p, token_t *q, token_t *out) { \
		out->i = p->i; \
		out->i _op q->i; out->r = (double)out->i; \
		out->vtype = KEV_INT; \
		out->ttype = KET_VAL; \
	}

	KE_GEN_BIN_INT(KEO_BAND, &=)
	KE_GEN_BIN_INT(KEO_BOR, |=)
	KE_GEN_BIN_INT(KEO_BXOR, ^=)
	KE_GEN_BIN_INT(KEO_LSH, <<=)
	KE_GEN_BIN_INT(KEO_RSH, >>=)
	KE_GEN_BIN_INT(KEO_MOD, %=)
	KE_GEN_BIN_INT(KEO_IDIV, /=)

	static void ke_op_add_eq(token_t *p, token_t *q, token_t *out) {
	out->i = p->i; out->r = p->r;
	out->i += q->i; out->r += q->r;
	out->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT;
	out->ttype = KET_VAL;
}

static void ke_op_sub_eq(token_t *p, token_t *q, token_t *out) {
	out->i = p->i; out->r = p->r;
	out->i -= q->i; out->r -= q->r;
	out->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT;
	out->ttype = KET_VAL;
}

static void ke_op_mul_eq(token_t *p, token_t *q, token_t *out) {
	out->i = p->i; out->r = p->r;
	out->i *= q->i; out->r *= q->r;
	out->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT;
	out->ttype = KET_VAL;
}

static void ke_op_KEO_DIV(token_t *p, token_t *q, token_t *out)
{
	out->r = p->r;
	out->r /= q->r;
	out->i = (int64_t)(out->r + .5);
	out->vtype = KEV_REAL;
	out->ttype = KET_VAL;
}


static void ke_op_KEO_LAND(token_t *p, token_t *q, token_t *out) { out->i = (p->i && q->i); out->r = (double)out->i; out->vtype = KEV_INT, out->ttype = KET_VAL; }
static void ke_op_KEO_LOR(token_t *p, token_t *q, token_t *out) { out->i = (p->i || q->i); out->r = (double)out->i; out->vtype = KEV_INT, out->ttype = KET_VAL; }
static void ke_op_KEO_POW(token_t *p, token_t *q, token_t *out) { out->r = pow(p->r, q->r), out->i = (int64_t)(out->r + .5); out->vtype = p->vtype == KEV_REAL || q->vtype == KEV_REAL ? KEV_REAL : KEV_INT, out->ttype = KET_VAL; }
static void ke_op_KEO_BNOT(token_t *p, token_t *q, token_t *out) { out->i = ~p->i; out->r = (double)out->i; out->vtype = KEV_INT, out->ttype = KET_VAL; }
static void ke_op_KEO_LNOT(token_t *p, token_t *q, token_t *out) { out->i = !p->i; out->r = (double)out->i; out->vtype = KEV_INT, out->ttype = KET_VAL; }
static void ke_op_KEO_POS(token_t *p, token_t *q, token_t *out) { } // do nothing
static void ke_op_KEO_NOP(token_t *p, token_t *q, token_t *out) { } // do nothing
static void ke_op_KEO_NEG(token_t *p, token_t *q, token_t *out) { out->i = -p->i, out->r = -p->r; out->vtype = KEV_REAL, out->ttype = KET_VAL; }

static void ke_func1_abs(token_t *p, token_t *q, token_t *out) { if (p->vtype == KEV_INT) out->i = (int64_t)abs((int)p->i), out->r = (double)p->i, out->vtype = KEV_INT, out->ttype = KET_VAL; else out->r = fabs(p->r), out->i = (int64_t)(out->r + .5), out->vtype = KEV_REAL, out->ttype = KET_VAL; }


int ke_command_icmd(sml_t* sml, char * name) {
	if (strcmp(name, CMD_FOR) == 0) {
		return CMD_IFOR;
	}
	else if (strcmp(name, CMD_NEXT) == 0) {
		return CMD_INEXT;
	}
	else if (strcmp(name, CMD_BRK) == 0) {
		return CMD_IBRK;
	}
	else if (strcmp(name, CMD_CNT) == 0) {
		return CMD_ICNT;
	}
	else if (strcmp(name, CMD_IF) == 0) {
		return CMD_IIF;
	}
	else if (strcmp(name, CMD_ELSE) == 0) {
		return CMD_IELSE;
	}
	else if (strcmp(name, CMD_END) == 0) {
		return CMD_IEND;
	}
	else if (strcmp(name, CMD_DEF) == 0) {
		return CMD_IDEF;
	}
	else if (strcmp(name, CMD_RTN) == 0) {
		return CMD_IRTN;
	}
	else if (strcmp(name, CMD_EXE) == 0) {
		return CMD_IEXE;
	}
	else {
		return -1;
		printf("Command not found : %s", name);
	}
}

int ifind_backward_defname(token_t ** tokens, int starti, token_t * tokp) {
	for (int i = starti + 1; i > -1; --i) {
		token_t *f = tokens[i];
		if (f->vtype == KEV_DEF) {
			return i;
		}
	}
	printf("SML: ERROR: Command <defname> not found at line <%d>\n", tokp->sourceLine);
	return -1;
}

int ifind_forward_rtn(kexpr_t *kexpr, int starti, token_t * tokp) {
	int i = -1;
	for (int i = starti + 1; i < kexpr->n; i++) {
		token_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IRTN) {
			return i;
		}
	}
	printf("SML: ERROR: Command <enddef> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_forward_next(kexpr_t *kexpr, int starti, token_t *tokp) {
	int qte_for = 0;
	for (int i = starti + 1; i < kexpr->n; i++) {
		token_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IFOR) {
			++qte_for;
		}
		else if (f->ttype == KET_VCMD) {
			if (qte_for && f->icmd == CMD_INEXT)
				--qte_for;
			else if (!qte_for && f->icmd == CMD_INEXT)
				return i;
		}
	}
	printf("SML: ERROR: Command <next> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_forward_end(kexpr_t *kexpr, int starti, token_t *tokp) {
	int qte_if = 0;
	for (int i = starti + 1; i < kexpr->n; i++) {
		token_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IIF) {
			++qte_if;
		}
		else if (f->ttype == KET_VCMD) {
			if (qte_if && f->icmd == CMD_IEND)
				--qte_if;
			else if (!qte_if && (f->icmd == CMD_IEND))
				return i;
		}
	}
	printf("SML: ERROR: Command <end> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ifind_else_or_end(kexpr_t *kexpr, int starti, token_t *tokp) {
	int i = -1;
	int qte_if = 0;
	for (int i = starti + 1; i < kexpr->n; i++) {
		token_t *f = &kexpr->e[i];
		if (f->icmd == CMD_IIF) {
			++qte_if;
		}
		else if (f->ttype == KET_VCMD) {
			if (qte_if && f->icmd == CMD_IEND)
				--qte_if;
			else if (!qte_if && (f->icmd == CMD_IEND || f->icmd == CMD_IELSE))
				return i;
		}
	}
	printf("SML: ERROR: Command <end or else> not found for token <%s> at line <%d>\n", tokp->name, tokp->sourceLine);
	return -1;
}

int ke_set_ijmp(sml_t* sml, kexpr_t *kexpr, token_t ** tokens) {
	token_t *tokp = NULL;
	int n = kexpr->n;
	int lastForCmd = 0;
	int lastNop = 0;
	for (int itok = 0; itok < n; ++itok) {
		tokp = tokens[itok];
		if (tokp->ttype == KET_OP && tokp->op == KEO_NOP) {
			lastNop = itok;
		}
		else 	if (tokp->ttype == KET_CMD || tokp->ttype == KET_VCMD) {
			register int icmd = tokp->icmd;
			if (icmd == CMD_IBRK) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			}
			else if (icmd == CMD_ICNT) {
				if (!tokp->ijmp) {
					if (sml->g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <continue> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = peekfor(sml);
				}
			}
			else if (icmd == CMD_IEXE) {
				if (!tokp->ijmp) {
					int idefname = ifind_backward_defname(tokens, itok, tokp);
					if (idefname != -1) {
						token_t *def_name = tokens[idefname];
						khint_t iter = kh_get(3, sml->hidefcommand, def_name->ifield);
						if (kh_end(sml->hidefcommand) == iter) {
							printf("SML: ERROR: Command <def> not found for token <exe>(%s) at line <%d>\n", def_name->name, tokp->sourceLine);
							return -1;
						}
						tokp->ijmp = (int)kh_val(sml->hidefcommand, iter);
					}
				}
			}
			else if (icmd == CMD_IDEF) {
				sml->lastDef = itok;
				int absent;
				token_t *def_name = tokens[itok - tokp->n_args];
				def_name->vtype = KEV_DEF;
				khint_t iter = kh_put(3, sml->hidefcommand, def_name->ifield, &absent);
				kh_val(sml->hidefcommand, iter) = ((itok)-tokp->n_args - 1);

				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_rtn(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			}
			else if (icmd == CMD_IIF) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_else_or_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			}
			else if (icmd == CMD_IELSE) {
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_end(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			}
			else if (icmd == CMD_IRTN) {
				if (sml->lastDef == -1) {
					printf("SML: ERROR: Command <def> not found for token <enddef> at line <%d>\n", tokp->sourceLine);
					return -1;
				}
				if (!tokp->ijmp) {
					tokp->ijmp = sml->lastDef;
				}
			}
			else if (icmd == CMD_IFOR) {
				if (tokp->ttype == KET_CMD) {
					pushfor(sml, lastNop);
				}
				else
				{
					pushfor(sml, itok - 1);
				}
				if (!tokp->ijmp) {
					tokp->ijmp = ifind_forward_next(kexpr, itok, tokp);
					if (tokp->ijmp == -1) return -1;
				}
			}
			else if (icmd == CMD_INEXT) {
				if (!tokp->ijmp) {
					if (sml->g_fortop < 0) {
						printf("SML: ERROR: Command <for> not found for token <next> at line <%d>\n", tokp->sourceLine);
						return -1;
					}
					tokp->ijmp = popfor(sml);
				}
			}
			// nothing to do for CMD_IEND
		}
	}
	return 0;
}
int ke_manage_function(sml_t *sml, token_t * tok, int err) {
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

int ke_manage_property(sml_t *sml, token_t *tok, int err) {
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

int ke_manage_variable(sml_t *sml, token_t *tok, int err) {
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

	token_t * recp = NULL;
	khint_t iter = kh_get(6, sml->hname, tok->name);
	if (kh_end(sml->hname) != iter) {
		tok->ifield = kh_val(sml->hname, iter);
	}
	else {
		// if the name contains '~', we must create a variable for the record and
		// add the current ifield to the record list
		char * dotp = strchr(tok->name, '~');
		if (dotp) {
			// split at the dot. tok.name will contain only the rec name  p~test => p
			*dotp = 0;
			int ifield;
			// check if the record is alreeady there
			khint_t iter = kh_get(6, sml->hname, tok->name);
			if (kh_end(sml->hname) != iter) {
				ifield = kh_val(sml->hname, iter);
				for (int k = 0; k < sml->rec_qte; ++k) {
					if (sml->recp[k]->ifield == ifield) {
						recp = sml->recp[k];
						break;
					}
				}
			}
			else {
				int absent;
				char * recName = ke_calloc_memory(sml, strlen(tok->name) + 1, 1);
				strcpy(recName, tok->name);
				khint_t iter = kh_put(6, sml->hname, recName, &absent);
				kh_val(sml->hname, iter) = sml->rec_qte;
				recp = (token_t *)ke_calloc_memory(sml, sizeof(token_t), 1);
				recp->ifield = sml->field_qte++;
				recp->name = recName;
				recp->ttype = KET_REC;
				recp->vtype = KEV_REC;
				sml->recp[sml->rec_qte] = recp;
				sml->rec_qte++;
			}
			*dotp = '~';
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

int ke_manage_command(sml_t *sml, token_t *tok, int err) {
	if (strcmp(tok->name, CMD_RTN) == 0) {
		*sml->currentDefName = 0;
	}
	tok->ttype = KET_VCMD;
	tok->icmd = ke_command_icmd(sml, tok->name);
	return err;
}

int ke_manage_digit(sml_t* sml, token_t *tok, char **r, char **p, char **q, int err) {
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

int ke_manage_string(sml_t *sml, token_t *tok, char **r, char **p, char **q, int err) {
	int c = **p;
	for (++(*p); **p && **p != c; ++(*p))
		if (**p == '\\') {
			++(*p); // escaping
		}
	if (**p == c) {
		tok->ttype = KET_VAL;
		tok->vtype = KEV_STR;
		tok->obj.s = ke_mystrndup(sml, *q + 1, *p - *q - 1);
		tok->i = strlen(tok->obj.s);
		tok->r = (double)tok->i;
		*r = *p + 1;
	}
	else
	{
		err |= KEE_UNQU;
		*r = *p;
	}
	return err;
}

int ke_manager_operator(sml_t *sml, token_t *tok, char **r, char **p, char **q, int last_is_val, int err) {
	tok->ttype = KET_OP;
	if (**p == '*' && (*p)[1] == '*') tok->op = KEO_POW, tok->f.builtin = ke_op_KEO_POW, tok->n_args = 2, *r = *q + 2;
	else if (**p == '*') tok->op = KEO_MUL, tok->f.builtin = ke_op_mul_eq, tok->n_args = 2, *r = *q + 1; // FIXME: NOT working for unary operators
	else if (**p == '/' && (*p)[1] == '/') tok->op = KEO_IDIV, tok->f.builtin = ke_op_KEO_IDIV, tok->n_args = 2, *r = *q + 2;
	else if (**p == '/') tok->op = KEO_DIV, tok->f.builtin = ke_op_KEO_DIV, tok->n_args = 2, *r = *q + 1;
	else if (**p == '%') tok->op = KEO_MOD, tok->f.builtin = ke_op_KEO_MOD, tok->n_args = 2, *r = *q + 1;
	else if (**p == '+') {
		if (last_is_val) tok->op = KEO_ADD, tok->f.builtin = ke_op_add_eq, tok->n_args = 2;
		else tok->op = KEO_POS, tok->f.builtin = ke_op_KEO_POS, tok->n_args = 1;
		*r = *q + 1;
	}
	else if (**p == '-') {
		if (last_is_val) tok->op = KEO_SUB, tok->f.builtin = ke_op_sub_eq, tok->n_args = 2;
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
int ke_manage_comment(sml_t *sml, token_t * tok, char **p) {
	++(*p);
	while (**p != 0) {
		if (**p == '\n') {
			//++sml->sourceCodeLine;
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

int ke_manage_line_continuation(sml_t *sml, token_t * tok, char**p) {
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
token_t ke_read_token(sml_t *sml, char *p, char **r, int *err, int last_is_val) // it doesn't parse parentheses
{
	token_t tok;
	memset(&tok, 0, sizeof(token_t));
	tok.realToken = 1;

	// loop until we find the start of a real token
	while (1) { // not a real token
				// eofile
		if (*p == 0) { tok.realToken = 0;	return tok; }

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
			if (sml->isLastTokenNop || sml->isFirstToken) { ++p;	continue; }
			break;
		}

		// no control character and space
		if (utf8iscontrol((void**)&p)) { ++p; continue; }

		// line continuation
		if (*p == '\\') {
			if (!ke_manage_line_continuation(sml, &tok, &p)) { return tok; }
			continue;
		}

		// ok we can continue with a real token
		break;
	}

	if (*p == 0) { tok.realToken = 0;	return tok; }

	char *q = p;
	sml->isLastTokenNop = 0;
	tok.sourceLine = sml->sourceCodeLine;
	if (*p == '_' || utf8isalpha((void**)&p)) { // a variable or a function
		for (; *p && (*p == '_' || *p == '.' || *p == '~' || utf8isalnum((void**)&p)); ++p);
		if (*p == 0) { tok.realToken = 0;	return tok; }

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
	}
	else if (isdigit(*p) || *p == '.') { // a number
		*err = ke_manage_digit(sml, &tok, r, &p, &q, *err);
	}
	else if (*p == '"' || *p == '\'') { // a string value
		*err = ke_manage_string(sml, &tok, r, &p, &q, *err);
	}
	else { // an operator
		*err = ke_manager_operator(sml, &tok, r, &p, &q, last_is_val, *err);
	}
	return tok;
}

static inline token_t *push_back(sml_t *sml, token_t **a, int *n, int *m)
{
	if (*n == *m) {
		int old_m = *m;
		*m = *m ? *m << 1 : 8;
		// first timae allocation
		if (*a == 0) ke_inc_memory(sml);
		*a = (token_t*)realloc(*a, *m * sizeof(token_t));
		if (*a == NULL) {
			printf("out of memory at push_back\n");
			printf("TODO clean up the memory\n");
			abort();
		}

		memset(*a + old_m, 0, (*m - old_m) * sizeof(token_t));
	}
	return &(*a)[(*n)++];
}

token_t *ke_parse_core(sml_t* sml, utf8 *_s, int *_n, int *err)
{
	char * p;
	int n_out, m_out, n_op, m_op, last_is_val = 0;
	token_t *out, *op, *t, *u;

	t = NULL;

	token_t * last = NULL;

	// remove remark, and normalize line seperator
	utf8 *s;
	s = ke_mystr(sml, _s, utf8size(_s) + 1); // make a copy
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
		}
		else if (*p == ')' || *p == ']') {
			if (*p == ']') isLastLeftBraket = 1; else isLastLeftBraket = 0;
			while (n_op > 0 && op[n_op - 1].op >= 0) { // move operators to the output until we see a left parenthesis
				u = push_back(sml, &out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op == 0) { // error: extra right parenthesis
				*err |= KEE_UNRP;
				break;
			}
			else --n_op; // pop out '('
			if (n_op > 0 && (op[n_op - 1].ttype == KET_FUNC || op[n_op - 1].ttype == KET_PROP || op[n_op - 1].ttype == KET_CMD)) { // the top of the operator g_stack is a function
				u = push_back(sml, &out, &n_out, &m_out); // move it to the output
				*u = op[--n_op];

				if ((u->n_args == 1) && isPreviousLeftParenthese) --u->n_args;  // no parameters

				if (u->n_args == 1 && strcmp(u->name, "abs") == 0) u->f.builtin = ke_func1_abs;
			}
			++p;
			isPreviousLeftParenthese = 0;
			last_is_val = 1;
		}
		else if (*p == ',') { // function arguments separator
			isLastLeftBraket = 0;
			isPreviousLeftParenthese = 0;
			while (n_op > 0 && op[n_op - 1].op >= 0) {
				u = push_back(sml, &out, &n_out, &m_out);
				*u = op[--n_op];
			}
			if (n_op < 2 || (op[n_op - 2].ttype != KET_FUNC && op[n_op - 2].ttype != KET_PROP && op[n_op - 2].ttype != KET_CMD)) { // we should at least see a function and a left parenthesis
				*err |= KEE_FUNC;
				break;
			}
			++op[n_op - 2].n_args;
			++p;
			last_is_val = 0;
		}
		else { // output-able token
			isPreviousLeftParenthese = 0;
			token_t v = ke_read_token(sml, p, &p, err, last_is_val);

			// SET PROPERTY FOR EQUAL
			if (v.ttype == KET_OP && v.op == KEO_LET && isLastLeftBraket) {
				// must set
				for (int i = n_out - 1; i > -1; --i) {
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
			}
			else if (v.ttype == KET_FUNC || v.ttype == KET_PROP || v.ttype == KET_CMD) {
				t = push_back(sml, &op, &n_op, &m_op);
				*t = v;
				last_is_val = 0;
			}
			else if (v.ttype == KET_OP) {
				int oi = ke_op[v.op];
				while (n_op > 0 && op[n_op - 1].ttype == KET_OP) {
					int pre = ke_op[op[n_op - 1].op] >> 1;
					if (((oi & 1) && oi >> 1 <= pre) || (!(oi & 1) && oi >> 1 < pre)) break;
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
		while (n_op > 0 && op[n_op - 1].op >= 0) {
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
	token_t *e = &out[i];
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

kexpr_t *ke_parse(sml_t *sml, utf8 *_s, int *err)
{
	int n;
	token_t *e;
	kexpr_t *ke;
	if (sml->fields == NULL) {
		sml->fields = ke_calloc_memory(sml, sizeof(token_t *) * 1000, 1);
	}
	e = ke_parse_core(sml, _s, &n, err);
	if (*err == 0) {
		ke = (kexpr_t*)ke_calloc_memory(sml, 1, sizeof(kexpr_t));
		ke->n = n, ke->e = e;
		//#ifdef DEBUG
		ke_print(sml, ke);
		//#endif // DEBUG
		//else {} {
		//	g_gbl_fields = realloc(g_gbl_fields, sizeof(token_t *) * g_gbl_field_qte);
		//	memset(g_gbl_fields + old_m, 0, (*m - old_m) * sizeof(token_t));
		//
		//}
		return ke;
	}
	else {
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

