#include <string.h>
#include "kexpr.h"
#include "str.h"
#include "utf8.h"

int ke_str_prop_get_0par(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	// b = a[1]  =>  1 a(1) =
	ke1_t *prop;
	prop = &stack[top - 1];
	prop->i = utf8len(prop->obj.s);
	prop->r = (double)prop->i;
	prop->ttype = KET_VAL;
	prop->vtype = KEV_INT;
	prop->obj.s = NULL;
	return top;
}

int ke_str_prop_get_1par(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	// b = a[1]  =>  1 a(1) =
	ke1_t *prop, *indice;
	prop = &stack[--top];
	indice = &stack[top - 1];

	if (indice->vtype == KEV_STR) {
		indice->i = utf8strstr(prop->obj.s, indice->obj.s);
		indice->ttype = KET_VAL;
		indice->vtype = KEV_INT;
		indice->r = (double)indice->i;
	}
	else {
		indice->obj.s = utf8pos(prop->obj.s, indice->i);
		indice->tofree = 1;
		indice->ttype = KET_VAL;
		indice->vtype = KEV_STR;
	}
	return top;
}

int ke_str_prop_get_2par(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	// b = a[1,2]  =>  1 2 a(2) =

	// 0,2
	// -1 -4  == len(-4) to len(-1)
	// 3,-1  , 3 to end
	// -1,-3     8+-3 = 5   8+-1 = 7
	// 
	ke1_t *prop, *from, *to;
	prop = &stack[--top];
	to = &stack[--top];
	from = &stack[top - 1];
	int len = (int)utf8len(prop->obj.s);
	if (from->i < 0 && to->i < 0) {
		int64_t t = from->i;
		from->i = to->i;
		to->i = t;
	}
	if (from->i < 0) {
		from->i = len + from->i;
	}
	if (to->i < 0) {
		to->i = len + to->i;
	}

	from->obj.s = utf8mid(prop->obj.s, (size_t)from->i, (size_t)to->i);
	from->tofree = 1;
	from->ttype = KET_VAL;
	from->vtype = KEV_STR;
	return top;
}

int ke_str_prop_set_1par(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	// a[2] = 'z' =>  2 a(1) 'z' =
	ke1_t *p, *q, *v;
	v = &stack[--top];
	p = &stack[--top];
	q = &stack[--top];

	p->obj.s[q->i] = v->obj.s[0];
	return top;
}

int ke_str_prop_set_2par(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	// a{1,2} = 'as'    1 2 a(2) 'sd' =
	ke1_t *prop, *from, *to, *v;
	v = &stack[--top];
	prop = &stack[--top];
	to = &stack[--top];
	from = &stack[--top];
	memcpy(&(prop->obj.s[from->i]), v->obj.s, (size_t)(to->i - from->i + 1));
	return top;
}

static int ke_strcpy(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    char * tmp = (char*)ke_calloc_memory(sml,strlen(q->obj.s)+1, 1);
    strcpy(tmp, q->obj.s);
    ke_set_str_direct(sml, p->ifield, tmp);
    --top;
    return top;
}

static int ke_strcat(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    size_t plen = (p->obj.s == NULL ? 0 : strlen(p->obj.s));
	size_t qlen = (q->obj.s == NULL ? 0 : strlen(q->obj.s));
   	char * tmp = (char*)ke_calloc_memory(sml,plen + qlen + 1,1);
   	if (plen) {
        strcpy(tmp, p->obj.s);
   	}
	if (q->obj.s != NULL) {
		strcat(tmp, q->obj.s);
	}
    ke_set_str_direct(sml, p->ifield, tmp);
    --top;
    return top;
}

static int ke_strbuf(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	ke1_t *p, *q;
	q = &stack[--top];
	p = &stack[--top];
	sml->fields[p->ifield]->obj.s = ke_calloc_memory(sml,(size_t)q->i,1);
	sml->fields[p->ifield]->ttype = KET_VAL;
	sml->fields[p->ifield]->vtype = KEV_STR;
	return top;
}

static int ke_strfree(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
	ke1_t *p;
	p = &stack[--top];
	ke_free_memory(sml, sml->fields[p->ifield]->obj.s);
	sml->fields[p->ifield]->obj.s = NULL;
	return top;
}

static int ke_strlen(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
   	ke1_t *p;
    p = &stack[top-1];
    p->i = strlen(p->obj.s);
    p->r = (double)p->i;
    p->obj.s = NULL;
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    return top;
}

static int ke_strcmp(sml_t* sml, ke1_t *tokp, int top) { ke1_t *stack = sml->stack;
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->i = strcmp(p->obj.s, q->obj.s);
    p->obj.s = NULL;
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    return top;
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

