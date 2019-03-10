#include <string.h>
#include "api.h"
#include "str.h"
#include "utf8.h"

void ke_str_prop_get_0par(sml_t* sml) { 
	// b = a[1]  =>  1 a(1) =
	token_t * prop = sml_pop_token(sml);
	int i = utf8len(sml_get_str(prop));
	sml_push_int(sml, i);
}

void ke_str_prop_get_1par(sml_t* sml) { 
	// b = a[1]  =>  1 a(1) =
	token_t * prop = sml_pop_token(sml);
	token_t * indice = sml_pop_token(sml);
	if (indice->vtype == KEV_STR) {
		int i = utf8strstr(sml_get_str(prop), sml_get_str(indice));
		sml_push_int(sml,i)
	}
	else {
		char * s = utf8pos(sml_get_str(prop), sml_get_int(indice));
		sml_push_str(sml, s);
	}
}

void ke_str_prop_get_2par(sml_t* sml) { 
	// b = a[1,2]  =>  1 2 a(2) =

	// 0,2
	// -1 -4  == len(-4) to len(-1)
	// 3,-1  , 3 to end
	// -1,-3     8+-3 = 5   8+-1 = 7
	// 
	char * str = sml_pop_str(sml);
	int to = sml_pop_int(sml);
	int from = sml_pop_int(sml);

	int64_t to_i = to;
	int64_t from_i = from;

	int len = (int)utf8len(str);
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

	char * s = utf8mid(str, (size_t)from_i, (size_t)to_i);
	sml_push_str(sml, s);
}

void ke_str_prop_set_1par(sml_t* sml) {
	// a[2] = 'z' =>  2 a(1) 'z' =
	char * dest = sml_pop_str(sml);
	char * src = sml_pop_str(sml);
	int i = sml_pop_int(sml);
	src[i] = dest[0];
}

void ke_str_prop_set_2par(sml_t* sml) {
	// a{1,2} = 'as'    1 2 a(2) 'sd' =
	char * src = sml_pop_str(sml);
	char * dest = sml_pop_str(sml);
	int to = sml_pop_int(sml);
	int from = sml_pop_int(sml);
	memcpy(&(dest[from]), src, to - from + 1);
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
	int t = tokp->ifield;
	if (t < 0) {
		t += sml->localtop;
	}
	tokp = sml->fields[t];
	char ** str = sml_adr_str(tokp);
	*str = sml_new_ptr(sml,i);
	tokp->ttype = KET_VAL;
	tokp->vtype = KEV_STR;
}

static void ke_strfree(sml_t* sml) {
	token_t * tokp = sml_pop_token(sml);
	int t = tokp->ifield;
	if (t < 0) {
		t += sml->localtop;
	}
	tokp = sml->fields[t];
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

