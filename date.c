#include "kexpr.h"
#include "gdate.h"
#include "date.h"
#include "api.c"

static void ke_date_dmy(sml_t* sml) {
	GDateYear y = sml_pop_year(sml);
	GDateMonth m = sml_pop_month(sml);
	GDateDay d = sml_pop_day(sml);
	GDate_t * date = g_date_new_dmy(sml, d, m, y);
	sml_push_date(sml, date);
}

static void ke_date_now(sml_t* sml) {
	GDate_t * date = g_date_new(sml);
	sml_push_date(sml, date);
	time_t now = time(NULL);
	g_date_set_time_t(sml_out_date, now);
}

static void ke_date_add_years(sml_t* sml) {
	guint years = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_add_years(date, years);
}

static void ke_date_add_months(sml_t* sml) {
	guint months = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_add_months(date, months);
}

static void ke_date_add_days(sml_t* sml) {
	guint days = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_add_days(date, days);
}

static void ke_date_sub_years(sml_t* sml) {
	guint years = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_subtract_years(date, years);
}

static void ke_date_sub_months(sml_t* sml) {
	guint months = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_subtract_months(date, months);
}

static void ke_date_sub_days(sml_t* sml) {
	guint days = sml_pop_int(sml);
	GDate_t * date = sml_pop_date(sml);
	g_date_subtract_days(date, days);
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