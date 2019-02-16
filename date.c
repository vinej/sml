#include "kexpr.h"
#include "gdate.h"
#include "date.h"

static int ke_date_dmy(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *d, *m, *y;
	y = stack[--top],
	m = stack[--top];
	d = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	out->obj.date = g_date_new_dmy(sml, (GDateDay)d->i, (GDateMonth)m->i, (GDateYear)y->i);
	out->ttype = KET_VAL;
	out->vtype = KEV_DATE;
	return top;
}

static int ke_date_now(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *out, *p;
	p = stack[--top];
	stack[top++] = sml->out; out = sml->out;
	time_t now = time(NULL);
	out->obj.date = g_date_new(sml);
	g_date_set_time_t(out->obj.date, now);
	out->ttype = KET_VAL;
	out->vtype = KEV_DATE;
	return top;
}

static int ke_date_add_years(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *year;
	year = stack[--top];
	date = stack[--top];
	g_date_add_years(sml->fields[date->ifield]->obj.date, (guint)year->i);
	return top;
}

static int ke_date_add_months(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *months;
	months = stack[--top];
	date = stack[--top];
	g_date_add_months(sml->fields[date->ifield]->obj.date, (guint)months->i);
	return top;
}

static int ke_date_add_days(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *days;
	days = stack[--top];
	date = stack[--top];
	g_date_add_days(sml->fields[date->ifield]->obj.date, (guint)days->i);
	return top;
}

static int ke_date_sub_years(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *year;
	year = stack[--top];
	date = stack[--top];
	g_date_subtract_years(sml->fields[date->ifield]->obj.date, (guint)year->i);
	return top;
}

static int ke_date_sub_months(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *months;
	months = stack[--top];
	date = stack[--top];
	g_date_subtract_months(sml->fields[date->ifield]->obj.date, (guint)months->i);
	return top;
}

static int ke_date_sub_days(sml_t* sml, ke1_t *tokp, int top) {
	ke1_t **stack = sml->stack;
	ke1_t *date, *days;
	days = stack[--top];
	date = stack[--top];
	g_date_subtract_days(sml->fields[date->ifield]->obj.date, (guint)days->i);
	return top;
}

void ke_date_hash(sml_t* sml) {
	ke_hash_add(sml, (fncp)&ke_date_dmy, DATE_NEW);
	ke_hash_add(sml, (fncp)&ke_date_dmy, DATE_DMY);
	ke_hash_add(sml, (fncp)&ke_date_now, DATE_NOW);
	ke_hash_add(sml, (fncp)&ke_date_add_years, DATE_ADD_YEARS);
	ke_hash_add(sml, (fncp)&ke_date_add_months, DATE_ADD_MONTHS);
	ke_hash_add(sml, (fncp)&ke_date_add_days, DATE_ADD_DAYS);
	ke_hash_add(sml, (fncp)&ke_date_sub_years, DATE_SUB_YEARS);
	ke_hash_add(sml, (fncp)&ke_date_sub_months, DATE_SUB_MONTHS);
	ke_hash_add(sml, (fncp)&ke_date_sub_days, DATE_SUB_DAYS);
}