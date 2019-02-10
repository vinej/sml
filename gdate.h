#ifndef __G_DATE_H__
#define __G_DATE_H__

#include <time.h>

#define G_LOG_DOMAIN    ((gchar*) 0)
typedef long glong;
typedef size_t gsize;
typedef unsigned int guint;
typedef unsigned char guchar;
typedef char gchar;
typedef __int32 guint32;
typedef int gboolean;
typedef unsigned __int8  guint8;
typedef int gint;
typedef unsigned __int16 guint16;
typedef __int32 GTime;
typedef __int16 GDateYear;
typedef __int8  GDateDay;   /* day of the month */

typedef struct _GTimeVal GTimeVal;

struct _GTimeVal
{
	glong tv_sec;
	glong tv_usec;
};
/* enum used to specify order of appearance in parsed date strings */
typedef enum
{
	G_DATE_DAY = 0,
	G_DATE_MONTH = 1,
	G_DATE_YEAR = 2
} GDateDMY;

typedef struct _GError GError;

struct _GError
{
	gint         code;
	gchar       *message;
};

/* actual week and month values */
typedef enum
{
	G_DATE_BAD_WEEKDAY = 0,
	G_DATE_MONDAY = 1,
	G_DATE_TUESDAY = 2,
	G_DATE_WEDNESDAY = 3,
	G_DATE_THURSDAY = 4,
	G_DATE_FRIDAY = 5,
	G_DATE_SATURDAY = 6,
	G_DATE_SUNDAY = 7
} GDateWeekday;
typedef enum
{
	G_DATE_BAD_MONTH = 0,
	G_DATE_JANUARY = 1,
	G_DATE_FEBRUARY = 2,
	G_DATE_MARCH = 3,
	G_DATE_APRIL = 4,
	G_DATE_MAY = 5,
	G_DATE_JUNE = 6,
	G_DATE_JULY = 7,
	G_DATE_AUGUST = 8,
	G_DATE_SEPTEMBER = 9,
	G_DATE_OCTOBER = 10,
	G_DATE_NOVEMBER = 11,
	G_DATE_DECEMBER = 12
} GDateMonth;

#define G_DATE_BAD_JULIAN 0U
#define G_DATE_BAD_DAY    0U
#define G_DATE_BAD_YEAR   0U

/* Note: directly manipulating structs is generally a bad idea, but
* in this case it's an *incredibly* bad idea, because all or part
* of this struct can be invalid at any given time. Use the functions,
* or you will get hosed, I promise.
*/
struct _GDate
{
	guint julian_days : 32; /* julian days representation - we use a
							*  bitfield hoping that 64 bit platforms
							*  will pack this whole struct in one big
							*  int
							*/

	guint julian : 1;    /* julian is valid */
	guint dmy : 1;    /* dmy is valid */

					  /* DMY representation */
	guint day : 6;
	guint month : 4;
	guint year : 16;
};

typedef struct _GDate GDate_t;

/* g_date_new() returns an invalid date, you then have to _set() stuff
* to get a usable object. You can also allocate a GDate statically,
* then call g_date_clear() to initialize.
*/
GDate_t* g_date_new(void);
GDate_t* g_date_new_dmy(GDateDay day, GDateMonth   month, GDateYear  year);
GDate_t* g_date_new_julian(guint32 julian_day);
void g_date_free(GDate_t  *date);
GDate_t*       g_date_copy(const GDate_t *date);

/* check g_date_valid() after doing an operation that might fail, like
* _parse.  Almost all g_date operations are undefined on invalid
* dates (the exceptions are the mutators, since you need those to
* return to validity).
*/
gboolean     g_date_valid(const GDate_t *date);
gboolean     g_date_valid_day(GDateDay     day);
gboolean     g_date_valid_month(GDateMonth month);
gboolean     g_date_valid_year(GDateYear  year);
gboolean     g_date_valid_weekday(GDateWeekday weekday);
gboolean     g_date_valid_julian(guint32 julian_date);
gboolean     g_date_valid_dmy(GDateDay     day,
	GDateMonth   month,
	GDateYear    year);

GDateWeekday g_date_get_weekday(const GDate_t *date);
GDateMonth   g_date_get_month(const GDate_t *date);
GDateYear    g_date_get_year(const GDate_t *date);
GDateDay     g_date_get_day(const GDate_t *date);
guint32      g_date_get_julian(const GDate_t *date);
guint        g_date_get_day_of_year(const GDate_t *date);
/* First monday/sunday is the start of week 1; if we haven't reached
* that day, return 0. These are not ISO weeks of the year; that
* routine needs to be added.
* these functions return the number of weeks, starting on the
* corrsponding day
*/
guint        g_date_get_monday_week_of_year(const GDate_t *date);
guint        g_date_get_sunday_week_of_year(const GDate_t *date);
guint        g_date_get_iso8601_week_of_year(const GDate_t *date);

/* If you create a static date struct you need to clear it to get it
* in a sane state before use. You can clear a whole array at
* once with the ndates argument.
*/
void         g_date_clear(GDate_t       *date,
	guint        n_dates);

/* The parse routine is meant for dates typed in by a user, so it
* permits many formats but tries to catch common typos. If your data
* needs to be strictly validated, it is not an appropriate function.
*/
void         g_date_set_parse(GDate_t       *date,
	const gchar *str);
void         g_date_set_time_t(GDate_t       *date,
	time_t       timet);
void         g_date_set_month(GDate_t       *date,
	GDateMonth   month);
void         g_date_set_day(GDate_t       *date,
	GDateDay     day);
void         g_date_set_year(GDate_t       *date,
	GDateYear    year);
void         g_date_set_dmy(GDate_t       *date,
	GDateDay     day,
	GDateMonth   month,
	GDateYear    y);
void         g_date_set_julian(GDate_t       *date,
	guint32      julian_date);
gboolean     g_date_is_first_of_month(const GDate_t *date);
gboolean     g_date_is_last_of_month(const GDate_t *date);

/* To go forward by some number of weeks just go forward weeks*7 days */
void         g_date_add_days(GDate_t       *date,
	guint        n_days);
void         g_date_subtract_days(GDate_t       *date,
	guint        n_days);

/* If you add/sub months while day > 28, the day might change */
void         g_date_add_months(GDate_t       *date,
	guint        n_months);
void         g_date_subtract_months(GDate_t       *date,
	guint        n_months);

/* If it's feb 29, changing years can move you to the 28th */
void         g_date_add_years(GDate_t       *date,
	guint        n_years);
void         g_date_subtract_years(GDate_t       *date,
	guint        n_years);
gboolean     g_date_is_leap_year(GDateYear    year);
guint8       g_date_get_days_in_month(GDateMonth   month,
	GDateYear    year) ;
guint8       g_date_get_monday_weeks_in_year(GDateYear    year);
guint8       g_date_get_sunday_weeks_in_year(GDateYear    year);

/* Returns the number of days between the two dates.  If date2 comes
before date1, a negative value is return. */
gint         g_date_days_between(const GDate_t *date1,
	const GDate_t *date2);

/* qsort-friendly (with a cast...) */
gint         g_date_compare(const GDate_t *lhs,
	const GDate_t *rhs);
void         g_date_to_struct_tm(const GDate_t *date,
	struct tm   *tm);

void         g_date_clamp(GDate_t *date,
	const GDate_t *min_date,
	const GDate_t *max_date);

/* Swap date1 and date2's values if date1 > date2. */
void         g_date_order(GDate_t *date1, GDate_t *date2);

/* Just like strftime() except you can only use date-related formats.
*   Using a time format is undefined.
*/
size_t  g_date_strftime(gchar       *s,
	size_t        slen,
	const gchar *format,
	const GDate_t *date);

#endif /* __G_DATE_H__ */
