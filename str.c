#include "stdafx.h"
#include <string.h>
#include "kexpr.h"
#include "str.h"

static int ke_strcpy(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    char * tmp = (char*)ke_calloc_memory(strlen(q->obj.s)+1, 1);
    strcpy(tmp, q->obj.s);
    ke_set_str_direct(p->ifield, tmp);
    --top;
    return top;
}

static int ke_strcat(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    size_t plen = (p->obj.s == NULL ? 0 : strlen(p->obj.s));
	size_t qlen = strlen(q->obj.s);
   	char * tmp = (char*)ke_calloc_memory(plen + qlen + 1,1);
   	if (plen) {
        strcpy(tmp, p->obj.s);
   	}
    strcat(tmp, q->obj.s);
    ke_set_str_direct(p->ifield, tmp);
    --top;
    return top;
}

static int ke_strlen(ke1_t *stack, int top) {
   	ke1_t *p;
    p = &stack[top-1];
    p->i = strlen(p->obj.s);
    p->r = (double)p->i;
    p->obj.s = NULL;
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    return top;
}

static int ke_strcmp(ke1_t *stack, int top) {
   	ke1_t *p, *q;
    q = &stack[--top];
    p = &stack[top-1];
    p->i = strcmp(p->obj.s, q->obj.s);
    p->obj.s = NULL;
    p->ttype = KET_VAL;
    p->vtype = KEV_INT;
    return top;
}

void ke_str_hash() {
    ke_hash_add(&ke_strcpy, STR_STRCPY);
    ke_hash_add(&ke_strcat, STR_STRCAT);
    ke_hash_add(&ke_strlen, STR_STRLEN);
    ke_hash_add(&ke_strcmp, STR_STRCMP);
    //ke_hash_add(&ke_strstr, STR_STRCHR);
    //ke_hash_add(&ke_strfmt, STR_STRFMT);
}

