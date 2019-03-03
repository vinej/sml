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

char * kev_to_str[17] = {
	"NONE",
	"REAL",
	"INT",
	"STR",
	"VEC",
	"MAT",
	"COMPLEX",
	"FNC",
	"REC",
	"VEC_INT",
	"DEF",
	"IMAGE",
	"FILE",
	"BUFFER",
	"DATE",
	"PTR",
	"FPOS"
};
// VALIDATION

#ifdef _DEBUG
void ke_validate_parameter_qte(sml_t* sml, token_t *p, int nb_param, char * function_name) {
	if (p->n_args != nb_param) {
		printf("\nSML: Invalid number of parameters at line %d (got %d expected %d) for function %s", p->sourceLine, p->n_args, nb_param, function_name);
		longjmp(sml->env_buffer, 1);
	}
}

void ke_validate_parameter_vtype(sml_t* sml, token_t * p, int vtype, char * param_name, char * function_name) {
	if (p->vtype != vtype) {
		printf("\nSML: Invalid type of parameter <%s> type at line <%d> (got %d expected %d) for function <%s>", param_name, p->sourceLine, p->vtype, vtype, function_name);
		longjmp(sml->env_buffer, 2);
	}
}

void ke_validate_parameter_ttype(sml_t* sml, token_t * p, int ttype, char * function_name) {
	if (p->ttype != ttype) {
		printf("\nSML: Invalid type of parameter at line <%d< (got %d expected %d) for function <%s>", p->sourceLine, p->ttype, ttype, function_name);
		longjmp(sml->env_buffer, 3);
	}
}

void ke_validate_parameter_not_null(sml_t* sml, token_t * p, void * ptr, char * param_name, char * function_name) {
	if (ptr == NULL) {
		printf("\nSML: Parameter <%s> is null at line <%d> for function <%s>", param_name, p->sourceLine, function_name);
		longjmp(sml->env_buffer,4);
	}
}

void ke_validate_parameter_int_gt_zero(sml_t* sml, token_t * p, char * param_name, char * function_name) {
	if (p->i <= 0) {
		printf("\nSML: Parameter <%s> must be greater than zero at line <%d> for function <%s>", param_name, p->sourceLine, function_name);
		longjmp(sml->env_buffer, 5);
	}
}
#endif // _DEBUG

void ke_set_jump(sml_t *sml, int info) {
	longjmp(sml->env_buffer, info);
}

sml_t * ke_create_sml() {
	sml_t * sml = calloc(1,sizeof(sml_t));
	sml->sourceCodeLine = 1;
	sml->lastDef = -1;
	sml->g_fortop = 0;
	sml->dllke_hash_add = (dllke_hash_add_t)ke_hash_add;
	sml->dllke_get_out = (dllke_get_out_t)ke_get_out;
	sml->out = calloc(100, sizeof(token_t));
	return sml;
}

void ke_free_sml(sml_t *sml) {
	free(sml->out);
	free(sml);
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
    void * tmp = calloc(x,i);
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
	dst = (char*)calloc(1,n+1);
	if (dst == NULL) {
		printf("out of memory at ke_mystrndup");
		printf("TODO clean up the memory");
		abort();
	}
	++sml->mem_count;
	memcpy(dst, src, n);
	return dst;
}

utf8 *ke_mystr(sml_t *sml, utf8 *src, size_t n)
{
	utf8*dst;
	char sstart[] = "1+1;1+1;";
	size_t start = utf8len(sstart);
	dst = (utf8*)calloc(1,start + n + 1);
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
token_t * ke_get_tok(sml_t *sml) {
	return sml->tokens[sml->tok_idx];
}


token_t * ke_get_tokidx(sml_t *sml, int idx) {
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

//typedef int(*DLLPROC)();
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

void ke_free_val(sml_t *sml) {
	for(int i = 0; i < sml->field_qte; ++i) {
		token_t*fieldp = sml->fields[i];
		if (!fieldp) continue;
        //ke_free_memory(sml, fieldp->name);
		if (fieldp->vtype == KEV_MAT && fieldp->obj.matrix) {
			ke_matrix_freemem(sml,fieldp);
		} else if (fieldp->vtype == KEV_VEC && fieldp->obj.vector) {
			ke_vector_freemem(sml, fieldp);
		} else if (fieldp->vtype == KEV_VEC_INT && fieldp->obj.vector_int) {
			ke_vector_int_freemem(sml, fieldp);
		} else if (fieldp->vtype == KEV_STR && fieldp->obj.s) {
            ke_free_memory(sml, fieldp->obj.s);
		} else if (fieldp->vtype == KEV_DATE && fieldp->obj.date) {
			ke_free_memory(sml, fieldp->obj.date);
		} else if (fieldp->vtype == KEV_BUFFER && fieldp->obj.buffer) {
			ke_free_memory(sml, fieldp->obj.buffer);
		}
        ke_free_memory(sml,fieldp);
    }
	if (sml->field_qte > 0) {
		ke_free_memory(sml, sml->fields);
	}
	sml->fields = NULL;

	for (int i = 0; i < sml->rec_qte; ++i) {
		token_t*fieldp = sml->recp[i];
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
    sml->tokens = ke_calloc_memory(sml, ke->n * sizeof(token_t *),1);
	token_t * ne = NULL;
	for (int i = 0; i < ke->n; ++i) {
        token_t *tokp = &ke->e[i];
        if (tokp->ttype == KET_VNAME) {
			ne = sml->fields[tokp->ifield];
			if (ne == NULL) {
				//char * newname = ke_mystrndup(sml, tokp->name, strlen(tokp->name));
				ne = ke_malloc_memory(sml, sizeof(token_t));
				if (ne == NULL) {
					printf("out of memory at ke_fill_list");
					printf("TODO clean up the memory");
					abort();
				}
				if ((tokp->ifield < sml->rec_qte) && (strcmp(sml->recp[tokp->ifield]->name, tokp->name) == 0)) {
					memcpy(ne, sml->recp[tokp->ifield], sizeof(token_t));
				}
				else {
					memcpy(ne, tokp, sizeof(token_t));
				}
				ne->name = tokp->name;
				sml->fields[tokp->ifield] = ne;
            }
			tokp = ne;
		}
        sml->tokens[i] = tokp;
	}
	return ke_set_ijmp(sml, ke, sml->tokens);
}

void ke_set_int(sml_t* sml, token_t *tokp, int64_t y)
{
	double yy = (double)y;
	tokp->i = y, tokp->r = yy, tokp->vtype = KEV_INT, tokp->assigned = 1;
}

void ke_set_real(sml_t* sml, token_t *tokp, double x)
{
    tokp->r = x, tokp->vtype = KEV_REAL;
}


void ke_set_vector(sml_t * sml, token_t *tokp, gsl_vector * vecp)
{
    ke_vector_freemem(sml, tokp);
    tokp->obj.vector = vecp, tokp->vtype = KEV_VEC, tokp->ttype = KET_VAL, tokp->assigned = 1;
}

void ke_set_vector_int(sml_t * sml, token_t *tokp, gsl_vector_int * vecp)
{
	ke_vector_int_freemem(sml,tokp);
	tokp->obj.vector_int = vecp, tokp->vtype = KEV_VEC_INT, tokp->ttype = KET_VAL, tokp->assigned = 1;
}

void ke_set_date(sml_t * sml, token_t *tokp, GDate_t * datep)
{
	if (tokp->obj.date && tokp->vtype == KEV_DATE) {
		free(tokp->obj.date);
	}
	tokp->obj.date = datep;
	tokp->vtype = KEV_DATE;
	tokp->ttype = KET_VAL;
	tokp->assigned = 1;
}

void ke_set_matrix(sml_t *sml, token_t *tokp, gsl_matrix * matp)
{
    ke_matrix_freemem(sml, tokp);
    tokp->obj.matrix = matp, tokp->vtype = KEV_MAT, tokp->ttype = KET_VAL, tokp->assigned = 1;
}


void ke_set_complex(sml_t *sml, token_t *tokp, gsl_complex complex)
{
	tokp->obj.tcomplex = complex, tokp->vtype = KEV_COMPLEX, tokp->assigned = 1;
}

static void ke_set_str_internal(sml_t* sml,token_t *tokp, char * tmp) {
    if (tokp->vtype == KEV_STR && tokp->obj.s) {
        ke_free_memory(sml,tokp->obj.s);
    }
    tokp->obj.s = tmp;
    tokp->i = strlen(tmp), tokp->r = (double)tokp->i, tokp->assigned = 1;
    tokp->vtype = KEV_STR;
}

void ke_set_str(sml_t* sml,token_t *tokp, char *x)
{
   char * tmp = ke_mystrndup(sml, x, strlen(x));
   ke_set_str_internal(sml, tokp,tmp);
}

void ke_set_str_direct(sml_t *sml, token_t * e, char *x)
{
	ke_set_str_internal(sml, e,x);
}

void ke_set_record(sml_t *sml, token_t *source, token_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.reclist = source->obj.reclist;
}

void ke_set_image(sml_t *sml, token_t *source, token_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.image = source->obj.image;
}

void ke_set_file(sml_t *sml, token_t *source, token_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.file = source->obj.file;
	dest->i = source->i;  // 1 = isopen
	dest->r = source->r;  // 1 = isopen
}

void ke_set_fpos(sml_t *sml, token_t *source, token_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.fpos = source->obj.fpos;
}

void ke_set_buffer(sml_t *sml, token_t *source, token_t *dest) {
	dest->ttype = source->ttype;
	dest->vtype = source->vtype;
	dest->obj.buffer = source->obj.buffer;
	dest->i = source->i;  // buffer len
	dest->r = source->r;  // buffer len
}

void ke_print_one_stack(token_t * tokp)
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
        printf("%d", tokp->op);
    } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_PROP || tokp->ttype == KET_CMD) {
        printf("%s(%d)", tokp->name, tokp->n_args);
    }
}

void ke_print_one(sml_t *sml, token_t * tokp)
{
	if (tokp->vtype == KEV_REC) {
		for (int i = 0; i < tokp->i; i++) {
			token_t * tmptokp = sml->fields[tokp->obj.reclist[i]];
			printf("%s = ", tmptokp->name);
			ke_print_one(sml, tmptokp);
			printf("\n");
		}
	} else if (tokp->vtype == KEV_BUFFER && tokp->i > 0) {
		printf("%.*s", tokp->i, tokp->obj.buffer);
	} else if (tokp->vtype == KEV_FPOS && tokp->i > 0) {
		printf("fpos");
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
			else if (tokp->vtype == KEV_BUFFER && tokp->i > 0)
				printf("%.*s", tokp->i, tokp->obj.buffer);
	} else if (tokp->tofree == 1) {
		if (tokp->vtype == KEV_STR) {
			ke_free_memory(sml, tokp->obj.s);
		}
	}
    #ifdef DEBUG
        if (tokp->ttype == KET_OP) {
            //printf("%s ", ke_opstr[tokp->op]);
        } else if (tokp->ttype == KET_FUNC || tokp->ttype == KET_CMD || tokp->ttype == KET_PROP) {
            printf("%s(%d) ", tokp->name, tokp->n_args);
        }
    #endif // DEBUG
}

void ke_print_stack(sml_t* sml, token_t *tokp, int top) { 
	token_t **stack = sml->stack;
    if (top > 3) {
        printf("\n%s", "*****************************");
        printf("\n%s\n", "Stack has more than one value");
        while(top) {
            ke_print_one(sml, stack[0]);
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
		token_t *tokp = &kexpr->e[i];
		if (i) putchar(' ');
		ke_print_one_stack(tokp);
	}
	putchar('\n');
}

void ke_print_error_one(sml_t * sml,kexpr_t *kexpr, char * name, token_t * e, int itok) {
    printf("\n*** ERROR for function <%s> at %d with the following value\n", name, itok);
    token_t *tokp = &kexpr->e[itok];
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
		token_t *tokp = &kexpr->e[i];
		if (i) putchar(' ');
		ke_print_one_stack(tokp);
	}
	putchar('\n');
	printf("\n");
}

void ke_hash_add(sml_t *sml, fncp key, char * name) {
    int absent;
    khint_t iter = kh_put(5, sml->hfunction, name, &absent);
    kh_val(sml->hfunction, iter) = (fncp)key;
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

token_t* ke_get_val_index(sml_t *sml, int i) {
	return sml->tokens[i];
}

void ke_set_val_index(sml_t *sml, int i, token_t *tokp) {
	ke_set_val(sml, sml->tokens[i], tokp);
}

void ke_set_val(sml_t* sml, token_t* e, token_t *q) {
	 e->vtype = q->vtype;
	 if (q->vtype == KEV_INT)  e->i = q->i, e->r = (double)e->i; 
	 else if (q->vtype == KEV_REAL)	e->r = q->r, e->i = (int64_t)e->r;
	 else if (q->vtype == KEV_STR) 	ke_set_str(sml, e, q->obj.s);
	 else if (q->vtype == KEV_VEC)	ke_set_vector(sml, e, q->obj.vector);
	 else if (q->vtype == KEV_VEC_INT)	ke_set_vector_int(sml, e, q->obj.vector_int);
	 else if (q->vtype == KEV_MAT) 	ke_set_matrix(sml, e, q->obj.matrix);
	 else if (q->vtype == KEV_COMPLEX) ke_set_complex(sml, e, q->obj.tcomplex);
	 else if (q->vtype == KEV_REC) ke_set_record(sml, q, e);
	 else if (q->vtype == KEV_IMAGE) ke_set_image(sml, q, e);
	 else if (q->vtype == KEV_FILE) ke_set_file(sml, q, e);
	 else if (q->vtype == KEV_DATE) ke_set_date(sml, e, q->obj.date);
	 else if (q->vtype == KEV_BUFFER) ke_set_buffer(sml, q, e);
	 else if (q->vtype == KEV_FPOS) ke_set_fpos(sml, q, e);
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

inline token_t * ke_get_out(sml_t *sml) {
	++sml->out_qte;
	if (sml->out_qte == 100) {
		sml->out_qte = 0;
	}
	return (token_t*)&sml->out[sml->out_qte];
}

void ke_push_stack(sml_t * sml, token_t * tokp, int *top) {
	sml->stack[*top] = tokp;
    (*top)++;
}


void ke_free_dll(sml_t * sml) {
	for (int i = 0; i < sml->libhandle_qte; ++i) {
		FreeLibrary(sml->libhandle[i]);
	}
}

void ke_destroy(sml_t *sml, kexpr_t *kexpr)
{
    ke_free(sml, kexpr);
	ke_free_hash(sml);
	ke_free_dll(sml);
}

void ke_free_hash(sml_t *sml)
{
	kh_destroy(5, sml->hfunction);	ke_dec_memory(sml);
	kh_destroy(6, sml->hname); ke_dec_memory(sml);
	//ke_constants_destroy();
	ke_command_destroy(sml);

	if (sml->mem_count != 0) {
        printf("%s:%d", "\n\nNot all memory has been deallocated\n", sml->mem_count);
	}
}

void ke_free(sml_t* sml,kexpr_t *kexpr)
{
    int i;
	for (i = 0; i < kexpr->n; ++i) {
		token_t *e = &kexpr->e[i];
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

int peekfor(sml_t* sml)
{
	//printf("\npeek %d\n", g_fortop);
	return sml->g_forstack[sml->g_fortop - 1];
}

int popfor(sml_t* sml) {
	//printf("\npop %d\n", g_fortop);
	//--g_fortop;
	return sml->g_forstack[--sml->g_fortop];
}

void pushfor(sml_t* sml, int val) {
	//printf("\npush %d\n", g_fortop);
	sml->g_forstack[sml->g_fortop++] = val;
	//++g_fortop;
}


