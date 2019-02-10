#include "kexpr.h"
#include "gdate.h"
#include "date.h"

static int ke_date_dmy(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t *stack = sml->stack;
	ke1_t *d, *m, *y;
	y = &stack[--top],
	m = &stack[--top];
	d = &stack[top - 1];
	d->obj.date = g_date_new_dmy( (GDateDay)d->i, (GDateMonth)m->i, (GDateYear)y->i);
	d->ttype = KET_VAL;
	d->vtype = KEV_DATE;
	return top;
}

static int ke_date_now(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t *stack = sml->stack;
	ke1_t *p;
	p = &stack[top - 1];
	time_t now = time(NULL);
	p->obj.date = g_date_new();
	g_date_set_time_t(p->obj.date, now);
	p->ttype = KET_VAL;
	p->vtype = KEV_DATE;
	return top;
}

void ke_date_hash(sml_t* sml) {
	ke_hash_add(sml, (fncp)&ke_date_dmy, DATE_NEW);
	ke_hash_add(sml, (fncp)&ke_date_dmy, DATE_DMY);
	ke_hash_add(sml, (fncp)&ke_date_now, DATE_NOW);
}