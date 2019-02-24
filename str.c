#include <string.h>
#include "kexpr.h"
#include "str.h"
#include "utf8.h"
#include "api.c"

int ke_str_prop_get_0par(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	// b = a[1]  =>  1 a(1) =
	ke1_t *out, *prop;
	prop = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	out->i = utf8len(prop->obj.s);
	out->r = (double)out->i;
	out->ttype = KET_VAL;
	out->vtype = KEV_INT;
	out->obj.s = NULL;
	return top;
}

int ke_str_prop_get_1par(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	// b = a[1]  =>  1 a(1) =
	ke1_t *out, *prop, *indice;
	prop = stack[--top];
	indice = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];

	if (indice->vtype == KEV_STR) {
		out->i = utf8strstr(prop->obj.s, indice->obj.s);
		out->ttype = KET_VAL;
		out->vtype = KEV_INT;
		out->r = (double)out->i;
	}
	else {
		out->obj.s = utf8pos(prop->obj.s, (size_t)indice->i);
		out->tofree = 1;
		out->ttype = KET_VAL;
		out->vtype = KEV_STR;
	}
	return top;
}

int ke_str_prop_get_2par(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	// b = a[1,2]  =>  1 2 a(2) =

	// 0,2
	// -1 -4  == len(-4) to len(-1)
	// 3,-1  , 3 to end
	// -1,-3     8+-3 = 5   8+-1 = 7
	// 
	ke1_t *out, *prop, *from, *to;
	prop = stack[--top];
	to = stack[--top];
	from = stack[--top];
	stack[top] = ke_get_out(sml); out = stack[top++];
	int64_t to_i = to->i;
	int64_t from_i = from->i;

	int len = (int)utf8len(prop->obj.s);
	if (from_i < 0 && to_i < 0) {
		int64_t t = from_i;
		from_i = to_i;
		to_i = t;
	}
	if (from_i < 0) {
		from_i = len + from_i;
	}
	if (to_i < 0) {
		to_i = len + to_i;
	}

	out->obj.s = utf8mid(prop->obj.s, (size_t)from_i, (size_t)to_i);
	out->tofree = 1;
	out->ttype = KET_VAL;
	out->vtype = KEV_STR;
	return top;
}

int ke_str_prop_set_1par(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	// a[2] = 'z' =>  2 a(1) 'z' =
	ke1_t *p, *q, *v;
	v = stack[--top];
	p = stack[--top];
	q = stack[--top];
	p->obj.s[q->i] = v->obj.s[0];
	return top;
}

int ke_str_prop_set_2par(sml_t* sml, ke1_t *tokp, int top) { ke1_t **stack = sml->stack;
	// a{1,2} = 'as'    1 2 a(2) 'sd' =
	ke1_t *prop, *from, *to, *v;
	v = stack[--top];
	prop = stack[--top];
	to = stack[--top];
	from = stack[--top];
	memcpy(&(prop->obj.s[from->i]), v->obj.s, (size_t)(to->i - from->i + 1));
	return top;
}

static void ke_strcpy(sml_t* sml) {
    char * src = sml_pop_str(sml);
	token_t * tokp = sml_pop_token(sml);
    char * tmp = sml_new_ptr(sml, utf8size(src)+1);
    utf8cpy(tmp, src);
    sml_save_str(sml, tokp, tmp);
}

static void ke_strcat(sml_t* sml) {
	char * src = sml_pop_str(sml);
	token_t * tokp = sml_pop_token(sml);
	char * dest = sml_get_str(tokp);

	size_t destlen = (dest == NULL ? 0 : utf8size(dest));
	size_t srclen = (src == NULL ? 0 : utf8size(src));
   	char * tmp = sml_new_ptr(sml, destlen + srclen + 1);
   	if (destlen) {
        utf8cat(tmp, dest);
   	}
	if (src != NULL) {
		utf8cat(tmp, src);
	}
    sml_save_str(sml, tokp, tmp);
}

static void ke_strbuf(sml_t* sml) {
	int i = sml_pop_int(sml);
	token_t * tokp = sml_pop_token(sml);
	char ** str = sml_adr_str(tokp);
	*str = sml_new_ptr(sml,i);
	tokp->ttype = KET_VAL;
	tokp->vtype = KEV_STR;
}

static void ke_strfree(sml_t* sml) {
	token_t * tokp = sml_pop_token(sml);
	char ** str = sml_adr_str(tokp);
	sml_free_ptr(sml, *str);
}

static void ke_strlen(sml_t* sml) { 
	char * str = sml_pop_str(sml);
	int i = utf8len(str);
	sml_push_int(sml, i);
}

static void ke_strcmp(sml_t* sml) {
	char * str2 = sml_pop_str(sml);
	char * str1 = sml_pop_str(sml);
    int i = utf8cmp(str1,str2);
	sml_push_int(sml, i);
}

void ke_str_hash(sml_t* sml) {
	ke_hash_add(sml,&ke_strfree, STR_STRFREE);
	ke_hash_add(sml, &ke_strbuf, STR_STRBUF);
	ke_hash_add(sml, &ke_strcpy, STR_STRCPY);
    ke_hash_add(sml, &ke_strcat, STR_STRCAT);
    ke_hash_add(sml, &ke_strlen, STR_STRLEN);
    ke_hash_add(sml, &ke_strcmp, STR_STRCMP);
    //ke_hash_add(sml,&ke_strstr, STR_STRCHR);
    //ke_hash_add(sml,&ke_strfmt, STR_STRFMT);
}

