/* GLIB - Library of useful routines for C programming
* Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

/*
* Modified by the GLib Team and others 1997-2000.  See the AUTHORS
* file for a list of people on the GLib Team.  See the ChangeLog
* files for a list of changes.  These files are distributed with
* GLib at ftp://ftp.gtk.org/pub/gtk/.
*/

/*
* MT safe
*/

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include "gdate.h"
#include "kexpr.h"

/**
* SECTION:date
* @title: Date and Time Functions
* @short_description: calendrical calculations and miscellaneous time stuff
*
* The #GDate_t data structure represents a day between January 1, Year 1,
* and sometime a few thousand years in the future (right now it will go
* to the year 65535 or so, but g_date_set_parse() only parses up to the
* year 8000 or so - just count on "a few thousand"). #GDate_t is meant to
* represent everyday dates, not astronomical dates or historical dates
* or ISO timestamps or the like. It extrapolates the current Gregorian
* calendar forward and backward in time; there is no attempt to change
* the calendar to match time periods or locations. #GDate_t does not store
* time information; it represents a day.
*
* The #GDate_t implementation has several nice features; it is only a
* 64-bit struct, so storing large numbers of dates is very efficient. It
* can keep both a Julian and day-month-year representation of the date,
* since some calculations are much easier with one representation or the
* other. A Julian representation is simply a count of days since some
* fixed day in the past; for #GDate_t the fixed day is January 1, 1 AD.
* ("Julian" dates in the #GDate_t API aren't really Julian dates in the
* technical sense; technically, Julian dates count from the start of the
* Julian period, Jan 1, 4713 BC).
*
* #GDate_t is simple to use. First you need a "blank" date; you can get a
* dynamically allocated date from g_date_new(), or you can declare an
* automatic variable or array and initialize it to a sane state by
* calling g_date_clear(). A cleared date is sane; it's safe to call
* g_date_set_dmy() and the other mutator functions to initialize the
* value of a cleared date. However, a cleared date is initially
* invalid, meaning that it doesn't represent a day that exists.
* It is undefined to call any of the date calculation routines on an
* invalid date. If you obtain a date from a user or other
* unpredictable source, you should check its validity with the
* g_date_valid() predicate. g_date_valid() is also used to check for
* errors with g_date_set_parse() and other functions that can
* fail. Dates can be invalidated by calling g_date_clear() again.
*
* It is very important to use the API to access the #GDate_t
* struct. Often only the day-month-year or only the Julian
* representation is valid. Sometimes neither is valid. Use the API.
*
* GLib also features #GDate_tTime which represents a precise time.
*/

/**
* G_USEC_PER_SEC:
*
* Number of microseconds in one second (1 million).
* This macro is provided for code readability.
*/

/**
* GTimeVal:
* @tv_sec: seconds
* @tv_usec: microseconds
*
* Represents a precise time, with seconds and microseconds.
* Similar to the struct timeval returned by the gettimeofday()
* UNIX system call.
*
* GLib is attempting to unify around the use of 64bit integers to
* represent microsecond-precision time. As such, this type will be
* removed from a future version of GLib. A consequence of using `glong` for
* `tv_sec` is that on 32-bit systems `GTimeVal` is subject to the year 2038
* problem.
*/

/**
* GDate_t:
* @julian_days: the Julian representation of the date
* @julian: this bit is set if @julian_days is valid
* @dmy: this is set if @day, @month and @year are valid
* @day: the day of the day-month-year representation of the date,
*     as a number between 1 and 31
* @month: the day of the day-month-year representation of the date,
*     as a number between 1 and 12
* @year: the day of the day-month-year representation of the date
*
* Represents a day between January 1, Year 1 and a few thousand years in
* the future. None of its members should be accessed directly.
*
* If the #GDate_t-struct is obtained from g_date_new(), it will be safe
* to mutate but invalid and thus not safe for calendrical computations.
*
* If it's declared on the stack, it will contain garbage so must be
* initialized with g_date_clear(). g_date_clear() makes the date invalid
* but sane. An invalid date doesn't represent a day, it's "empty." A date
* becomes valid after you set it to a Julian day or you set a day, month,
* and year.
*/

/**
* GTime:
*
* Simply a replacement for time_t. It has been deprecated
* since it is not equivalent to time_t on 64-bit platforms
* with a 64-bit time_t. Unrelated to #GTimer.
*
* Note that #GTime is defined to always be a 32-bit integer,
* unlike time_t which may be 64-bit on some systems. Therefore,
* #GTime will overflow in the year 2038, and you cannot use the
* address of a #GTime variable as argument to the UNIX time()
* function.
*
* Instead, do the following:
* |[<!-- language="C" -->
* time_t ttime;
* GTime gtime;
*
* time (&ttime);
* gtime = (GTime)ttime;
* ]|
*/

/**
* GDate_tDMY:
* @G_DATE_DAY: a day
* @G_DATE_MONTH: a month
* @G_DATE_YEAR: a year
*
* This enumeration isn't used in the API, but may be useful if you need
* to mark a number as a day, month, or year.
*/

/**
* GDate_tDay:
*
* Integer representing a day of the month; between 1 and 31.
* #G_DATE_BAD_DAY represents an invalid day of the month.
*/

/**
* GDate_tMonth:
* @G_DATE_BAD_MONTH: invalid value
* @G_DATE_JANUARY: January
* @G_DATE_FEBRUARY: February
* @G_DATE_MARCH: March
* @G_DATE_APRIL: April
* @G_DATE_MAY: May
* @G_DATE_JUNE: June
* @G_DATE_JULY: July
* @G_DATE_AUGUST: August
* @G_DATE_SEPTEMBER: September
* @G_DATE_OCTOBER: October
* @G_DATE_NOVEMBER: November
* @G_DATE_DECEMBER: December
*
* Enumeration representing a month; values are #G_DATE_JANUARY,
* #G_DATE_FEBRUARY, etc. #G_DATE_BAD_MONTH is the invalid value.
*/

/**
* GDate_tYear:
*
* Integer representing a year; #G_DATE_BAD_YEAR is the invalid
* value. The year must be 1 or higher; negative (BC) years are not
* allowed. The year is represented with four digits.
*/

/**
* GDate_tWeekday:
* @G_DATE_BAD_WEEKDAY: invalid value
* @G_DATE_MONDAY: Monday
* @G_DATE_TUESDAY: Tuesday
* @G_DATE_WEDNESDAY: Wednesday
* @G_DATE_THURSDAY: Thursday
* @G_DATE_FRIDAY: Friday
* @G_DATE_SATURDAY: Saturday
* @G_DATE_SUNDAY: Sunday
*
* Enumeration representing a day of the week; #G_DATE_MONDAY,
* #G_DATE_TUESDAY, etc. #G_DATE_BAD_WEEKDAY is an invalid weekday.
*/

/**
* G_DATE_BAD_DAY:
*
* Represents an invalid #GDate_tDay.
*/

/**
* G_DATE_BAD_JULIAN:
*
* Represents an invalid Julian day number.
*/

/**
* G_DATE_BAD_YEAR:
*
* Represents an invalid year.
*/

/**
* g_date_new:
*
* Allocates a #GDate_t and initializes
* it to a sane state. The new date will
* be cleared (as if you'd called g_date_clear()) but invalid (it won't
* represent an existing day). Free the return value with g_date_free().
*
* Returns: a newly-allocated #GDate_t
*/
GDate_t*
g_date_new(void)
{
	GDate_t *d = calloc(sizeof(GDate_t), 1); /* happily, 0 is the invalid flag for everything. */
	return d;
}

/**
* g_date_new_dmy:
* @day: day of the month
* @month: month of the year
* @year: year
*
* Like g_date_new(), but also sets the value of the date. Assuming the
* day-month-year triplet you pass in represents an existing day, the
* returned date will be valid.
*
* Returns: a newly-allocated #GDate_t initialized with @day, @month, and @year
*/
GDate_t* g_date_new_dmy(GDateDay   day,	GDateMonth m,	GDateYear  y)
{
	//g_return_val_if_fail(g_date_valid_dmy(day, m, y), NULL);

	GDate_t *d = calloc(sizeof(GDate_t), 1); /* happily, 0 is the invalid flag for everything. */

	d->julian = FALSE;
	d->dmy = TRUE;

	d->month = m;
	d->day = day;
	d->year = y;

	//g_assert(g_date_valid(d));

	return d;
}

/**
* g_date_new_julian:
* @julian_day: days since January 1, Year 1
*
* Like g_date_new(), but also sets the value of the date. Assuming the
* Julian day number you pass in is valid (greater than 0, less than an
* unreasonably large number), the returned date will be valid.
*
* Returns: a newly-allocated #GDate_t initialized with @julian_day
*/
GDate_t* g_date_new_julian(guint32 julian_day)
{
	//g_return_val_if_fail(g_date_valid_julian(julian_day), NULL);

	GDate_t *d = calloc(sizeof(GDate_t), 1); /* happily, 0 is the invalid flag for everything. */

	d->julian = TRUE;
	d->dmy = FALSE;

	d->julian_days = julian_day;

	//g_assert(g_date_valid(d));

	return d;
}

/**
* g_date_free:
* @date: a #GDate_t to free
*
* Frees a #GDate_t returned from g_date_new().
*/
void g_date_free(GDate_t *date)
{
	//g_return_if_fail(date != NULL);

	free(date);
}

/**
* g_date_copy:
* @date: a #GDate_t to copy
*
* Copies a GDate_t to a newly-allocated GDate_t. If the input was invalid
* (as determined by g_date_valid()), the invalid state will be copied
* as is into the new object.
*
* Returns: (transfer full): a newly-allocated #GDate_t initialized from @date
*
* Since: 2.56
*/
GDate_t * g_date_copy(const GDate_t *date)
{
	GDate_t *res;
	//g_return_val_if_fail(date != NULL, NULL);

	if (g_date_valid(date))
		res = g_date_new_julian(g_date_get_julian(date));
	else
	{
		res = g_date_new();
		*res = *date;
	}

	return res;
}

/**
* g_date_valid:
* @date: a #GDate_t to check
*
* Returns %TRUE if the #GDate_t represents an existing day. The date must not
* contain garbage; it should have been initialized with g_date_clear()
* if it wasn't allocated by one of the g_date_new() variants.
*
* Returns: Whether the date is valid
*/
gboolean g_date_valid(const GDate_t *d)
{
	//g_return_val_if_fail(d != NULL, FALSE);

	return (d->julian || d->dmy);
}

static const guint8 days_in_months[2][13] =
{  /* error, jan feb mar apr may jun jul aug sep oct nov dec */
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
{ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } /* leap year */
};

static const guint16 days_in_year[2][14] =
{  /* 0, jan feb mar apr may  jun  jul  aug  sep  oct  nov  dec */
	{ 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
{ 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/**
* g_date_valid_month:
* @month: month
*
* Returns %TRUE if the month value is valid. The 12 #GDate_tMonth
* enumeration values are the only valid months.
*
* Returns: %TRUE if the month is valid
*/
gboolean g_date_valid_month(GDateMonth m)
{
	return ((m > G_DATE_BAD_MONTH) && (m < 13));
}

/**
* g_date_valid_year:
* @year: year
*
* Returns %TRUE if the year is valid. Any year greater than 0 is valid,
* though there is a 16-bit limit to what #GDate_t will understand.
*
* Returns: %TRUE if the year is valid
*/
gboolean g_date_valid_year(GDateYear y)
{
	return (y > G_DATE_BAD_YEAR);
}

/**
* g_date_valid_day:
* @day: day to check
*
* Returns %TRUE if the day of the month is valid (a day is valid if it's
* between 1 and 31 inclusive).
*
* Returns: %TRUE if the day is valid
*/

gboolean g_date_valid_day(GDateDay d)
{
	return ((d > G_DATE_BAD_DAY) && (d < 32));
}

/**
* g_date_valid_weekday:
* @weekday: weekday
*
* Returns %TRUE if the weekday is valid. The seven #GDate_tWeekday enumeration
* values are the only valid weekdays.
*
* Returns: %TRUE if the weekday is valid
*/
gboolean g_date_valid_weekday(GDateWeekday w)
{
	return ((w > G_DATE_BAD_WEEKDAY) && (w < 8));
}

/**
* g_date_valid_julian:
* @julian_date: Julian day to check
*
* Returns %TRUE if the Julian day is valid. Anything greater than zero
* is basically a valid Julian, though there is a 32-bit limit.
*
* Returns: %TRUE if the Julian day is valid
*/
gboolean g_date_valid_julian(guint32 j)
{
	return (j > G_DATE_BAD_JULIAN);
}

/**
* g_date_valid_dmy:
* @day: day
* @month: month
* @year: year
*
* Returns %TRUE if the day-month-year triplet forms a valid, existing day
* in the range of days #GDate_t understands (Year 1 or later, no more than
* a few thousand years in the future).
*
* Returns: %TRUE if the date is a valid one
*/
gboolean g_date_valid_dmy(GDateDay   d,
	GDateMonth m,
	GDateYear  y)
{
	/* No need to check the upper bound of @y, because #GDate_tYear is 16 bits wide,
	* just like #GDate_t.year. */
	return ((m > G_DATE_BAD_MONTH) &&
		(m < 13) &&
		(d > G_DATE_BAD_DAY) &&
		(y > G_DATE_BAD_YEAR) &&   /* must check before using g_date_is_leap_year */
		(d <= (g_date_is_leap_year(y) ?
			days_in_months[1][m] : days_in_months[0][m])));
}


/* "Julian days" just means an absolute number of days, where Day 1 ==
*   Jan 1, Year 1
*/
static void g_date_update_julian(const GDate_t *const_d)
{
	GDate_t *d = (GDate_t *)const_d;
	GDateYear year;
	gint idx;

	//g_return_if_fail(d != NULL);
	//g_return_if_fail(d->dmy != 0);
	//g_return_if_fail(!d->julian);
	//g_return_if_fail(g_date_valid_dmy(d->day, d->month, d->year));

	/* What we actually do is: multiply years * 365 days in the year,
	* add the number of years divided by 4, subtract the number of
	* years divided by 100 and add the number of years divided by 400,
	* which accounts for leap year stuff. Code from Steffen Beyer's
	* DateCalc.
	*/

	year = d->year - 1; /* we know d->year > 0 since it's valid */

	d->julian_days = year * 365U;
	d->julian_days += (year >>= 2); /* divide by 4 and add */
	d->julian_days -= (year /= 25); /* divides original # years by 100 */
	d->julian_days += year >> 2;    /* divides by 4, which divides original by 400 */

	idx = g_date_is_leap_year(d->year) ? 1 : 0;

	d->julian_days += days_in_year[idx][d->month] + d->day;

	//g_return_if_fail(g_date_valid_julian(d->julian_days));

	d->julian = TRUE;
}

static void g_date_update_dmy(const GDate_t *const_d)
{
	GDate_t *d = (GDate_t *)const_d;
	GDateYear y;
	GDateMonth m;
	GDateDay day;

	guint32 A, B, C, D, E, M;

	//g_return_if_fail(d != NULL);
	//g_return_if_fail(d->julian);
	//g_return_if_fail(!d->dmy);
	//g_return_if_fail(g_date_valid_julian(d->julian_days));

	/* Formula taken from the Calendar FAQ; the formula was for the
	*  Julian Period which starts on 1 January 4713 BC, so we add
	*  1,721,425 to the number of days before doing the formula.
	*
	* I'm sure this can be simplified for our 1 January 1 AD period
	* start, but I can't figure out how to unpack the formula.
	*/

	A = d->julian_days + 1721425 + 32045;
	B = (4 * (A + 36524)) / 146097 - 1;
	C = A - (146097 * B) / 4;
	D = (4 * (C + 365)) / 1461 - 1;
	E = C - ((1461 * D) / 4);
	M = (5 * (E - 1) + 2) / 153;

	m = M + 3 - (12 * (M / 10));
	day = E - (153 * M + 2) / 5;
	y = 100 * B + D - 4800 + (M / 10);

#ifdef G_ENABLE_DEBUG
	if (!g_date_valid_dmy(day, m, y))
		g_warning("OOPS julian: %u  computed dmy: %u %u %u",
			d->julian_days, day, m, y);
#endif

	d->month = m;
	d->day = day;
	d->year = y;

	d->dmy = TRUE;
}

/**
* g_date_get_weekday:
* @date: a #GDate_t
*
* Returns the day of the week for a #GDate_t. The date must be valid.
*
* Returns: day of the week as a #GDate_tWeekday.
*/
GDateWeekday g_date_get_weekday(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), G_DATE_BAD_WEEKDAY);

	if (!d->julian)
		g_date_update_julian(d);

	//g_return_val_if_fail(d->julian, G_DATE_BAD_WEEKDAY);

	return ((d->julian_days - 1) % 7) + 1;
}

/**
* g_date_get_month:
* @date: a #GDate_t to get the month from
*
* Returns the month of the year. The date must be valid.
*
* Returns: month of the year as a #GDate_tMonth
*/
GDateMonth g_date_get_month(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), G_DATE_BAD_MONTH);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, G_DATE_BAD_MONTH);

	return d->month;
}

/**
* g_date_get_year:
* @date: a #GDate_t
*
* Returns the year of a #GDate_t. The date must be valid.
*
* Returns: year in which the date falls
*/
GDateYear g_date_get_year(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), G_DATE_BAD_YEAR);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, G_DATE_BAD_YEAR);

	return d->year;
}

/**
* g_date_get_day:
* @date: a #GDate_t to extract the day of the month from
*
* Returns the day of the month. The date must be valid.
*
* Returns: day of the month
*/
GDateDay g_date_get_day(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), G_DATE_BAD_DAY);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, G_DATE_BAD_DAY);

	return d->day;
}

/**
* g_date_get_julian:
* @date: a #GDate_t to extract the Julian day from
*
* Returns the Julian day or "serial number" of the #GDate_t. The
* Julian day is simply the number of days since January 1, Year 1; i.e.,
* January 1, Year 1 is Julian day 1; January 2, Year 1 is Julian day 2,
* etc. The date must be valid.
*
* Returns: Julian day
*/
guint32 g_date_get_julian(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), G_DATE_BAD_JULIAN);

	if (!d->julian)
		g_date_update_julian(d);

	//g_return_val_if_fail(d->julian, G_DATE_BAD_JULIAN);

	return d->julian_days;
}

/**
* g_date_get_day_of_year:
* @date: a #GDate_t to extract day of year from
*
* Returns the day of the year, where Jan 1 is the first day of the
* year. The date must be valid.
*
* Returns: day of the year
*/
guint g_date_get_day_of_year(const GDate_t *d)
{
	gint idx;

	//g_return_val_if_fail(g_date_valid(d), 0);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, 0);

	idx = g_date_is_leap_year(d->year) ? 1 : 0;

	return (days_in_year[idx][d->month] + d->day);
}

/**
* g_date_get_monday_week_of_year:
* @date: a #GDate_t
*
* Returns the week of the year, where weeks are understood to start on
* Monday. If the date is before the first Monday of the year, return 0.
* The date must be valid.
*
* Returns: week of the year
*/
guint g_date_get_monday_week_of_year(const GDate_t *d)
{
	GDateWeekday wd;
	guint day;
	GDate_t first;

	//g_return_val_if_fail(g_date_valid(d), 0);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, 0);

	g_date_clear(&first, 1);

	g_date_set_dmy(&first, 1, 1, d->year);

	wd = g_date_get_weekday(&first) - 1; /* make Monday day 0 */
	day = g_date_get_day_of_year(d) - 1;

	return ((day + wd) / 7U + (wd == 0 ? 1 : 0));
}

/**
* g_date_get_sunday_week_of_year:
* @date: a #GDate_t
*
* Returns the week of the year during which this date falls, if
* weeks are understood to begin on Sunday. The date must be valid.
* Can return 0 if the day is before the first Sunday of the year.
*
* Returns: week number
*/
guint g_date_get_sunday_week_of_year(const GDate_t *d)
{
	GDateWeekday wd;
	guint day;
	GDate_t first;

	//g_return_val_if_fail(g_date_valid(d), 0);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, 0);

	g_date_clear(&first, 1);

	g_date_set_dmy(&first, 1, 1, d->year);

	wd = g_date_get_weekday(&first);
	if (wd == 7) wd = 0; /* make Sunday day 0 */
	day = g_date_get_day_of_year(d) - 1;

	return ((day + wd) / 7U + (wd == 0 ? 1 : 0));
}

/**
* g_date_get_iso8601_week_of_year:
* @date: a valid #GDate_t
*
* Returns the week of the year, where weeks are interpreted according
* to ISO 8601.
*
* Returns: ISO 8601 week number of the year.
*
* Since: 2.6
**/
guint g_date_get_iso8601_week_of_year(const GDate_t *d)
{
	guint j, d4, L, d1, w;

	//g_return_val_if_fail(g_date_valid(d), 0);

	if (!d->julian)
		g_date_update_julian(d);

	//g_return_val_if_fail(d->julian, 0);

	/* Formula taken from the Calendar FAQ; the formula was for the
	* Julian Period which starts on 1 January 4713 BC, so we add
	* 1,721,425 to the number of days before doing the formula.
	*/
	j = d->julian_days + 1721425;
	d4 = (j + 31741 - (j % 7)) % 146097 % 36524 % 1461;
	L = d4 / 1460;
	d1 = ((d4 - L) % 365) + L;
	w = d1 / 7 + 1;

	return w;
}

/**
* g_date_days_between:
* @date1: the first date
* @date2: the second date
*
* Computes the number of days between two dates.
* If @date2 is prior to @date1, the returned value is negative.
* Both dates must be valid.
*
* Returns: the number of days between @date1 and @date2
*/
gint g_date_days_between(const GDate_t *d1,
	const GDate_t *d2)
{
	//g_return_val_if_fail(g_date_valid(d1), 0);
	//g_return_val_if_fail(g_date_valid(d2), 0);

	return (gint)g_date_get_julian(d2) - (gint)g_date_get_julian(d1);
}

/**
* g_date_clear:
* @date: pointer to one or more dates to clear
* @n_dates: number of dates to clear
*
* Initializes one or more #GDate_t structs to a sane but invalid
* state. The cleared dates will not represent an existing date, but will
* not contain garbage. Useful to init a date declared on the stack.
* Validity can be tested with g_date_valid().
*/
void g_date_clear(GDate_t *d, guint ndates)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(ndates != 0);

	memset(d, 0x0, ndates * sizeof(GDate_t));
}

static gchar *long_month_names[13] =
{
	NULL,
};

static gchar *long_month_names_alternative[13] =
{
	NULL,
};

static gchar *short_month_names[13] =
{
	NULL,
};

static gchar *short_month_names_alternative[13] =
{
	NULL,
};

/* This tells us if we need to update the parse info */
static gchar *current_locale = NULL;

/* order of these in the current locale */
static GDateDMY dmy_order[3] =
{
	G_DATE_DAY, G_DATE_MONTH, G_DATE_YEAR
};

/* Where to chop two-digit years: i.e., for the 1930 default, numbers
* 29 and below are counted as in the year 2000, numbers 30 and above
* are counted as in the year 1900.
*/

static const GDateYear twodigit_start_year = 1930;

/* It is impossible to enter a year between 1 AD and 99 AD with this
* in effect.
*/
static gboolean using_twodigit_years = FALSE;

/* Adjustment of locale era to AD, non-zero means using locale era
*/
static gint locale_era_adjust = 0;

struct _GDate_tParseTokens {
	gint num_ints;
	gint n[3];
	guint month;
};

typedef struct _GDate_tParseTokens GDate_tParseTokens;

static inline gboolean
update_month_match(gsize *longest,
	const gchar *haystack,
	const gchar *needle)
{
	gsize length;

	if (needle == NULL)
		return FALSE;

	length = strlen(needle);
	if (*longest >= length)
		return FALSE;

	if (strstr(haystack, needle) == NULL)
		return FALSE;

	*longest = length;
	return TRUE;
}

#define NUM_LEN 10

/**
* g_date_set_time_t:
* @date: a #GDate_t
* @timet: time_t value to set
*
* Sets the value of a date to the date corresponding to a time
* specified as a time_t. The time to date conversion is done using
* the user's current timezone.
*
* To set the value of a date to the current day, you could write:
* |[<!-- language="C" -->
*  time_t now = time (NULL);
*  if (now == (time_t) -1)
*    // handle the error
*  g_date_set_time_t (date, now);
* ]|
*
* Since: 2.10
*/
void g_date_set_time_t(GDate_t *date,
	time_t timet)
{
	struct tm tm;

	//g_return_if_fail(date != NULL);

#ifdef HAVE_LOCALTIME_R
	localtime_r(&timet, &tm);
#else
	{
		struct tm *ptm = localtime(&timet);

		if (ptm == NULL)
		{
			/* Happens at least in Microsoft's C library if you pass a
			* negative time_t. Use 2000-01-01 as default date.
			*/
			//g_return_if_fail_warning(G_LOG_DOMAIN, "g_date_set_time", "ptm != NULL");

			tm.tm_mon = 0;
			tm.tm_mday = 1;
			tm.tm_year = 100;
		}
		else
			memcpy((void *)&tm, (void *)ptm, sizeof(struct tm));
	}
#endif

	date->julian = FALSE;

	date->month = tm.tm_mon + 1;
	date->day = tm.tm_mday;
	date->year = tm.tm_year + 1900;

	//g_return_if_fail(g_date_valid_dmy(date->day, date->month, date->year));

	date->dmy = TRUE;
}


/**
* g_date_set_time:
* @date: a #GDate_t.
* @time_: #GTime value to set.
*
* Sets the value of a date from a #GTime value.
* The time to date conversion is done using the user's current timezone.
*
* Deprecated: 2.10: Use g_date_set_time_t() instead.
*/
void g_date_set_time(GDate_t *date,	GTime  time_)
{
	g_date_set_time_t(date, (time_t)time_);
}

/**
* g_date_set_time_val:
* @date: a #GDate_t
* @timeval: #GTimeVal value to set
*
* Sets the value of a date from a #GTimeVal value.  Note that the
* @tv_usec member is ignored, because #GDate_t can't make use of the
* additional precision.
*
* The time to date conversion is done using the user's current timezone.
*
* Since: 2.10
*/
void g_date_set_time_val(GDate_t    *date, 	GTimeVal *timeval)
{
	g_date_set_time_t(date, (time_t)timeval->tv_sec);
}

/**
* g_date_set_month:
* @date: a #GDate_t
* @month: month to set
*
* Sets the month of the year for a #GDate_t.  If the resulting
* day-month-year triplet is invalid, the date will be invalid.
*/
void g_date_set_month(GDate_t  *d, GDateMonth m)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(g_date_valid_month(m));

	if (d->julian && !d->dmy) g_date_update_dmy(d);
	d->julian = FALSE;

	d->month = m;

	if (g_date_valid_dmy(d->day, d->month, d->year))
		d->dmy = TRUE;
	else
		d->dmy = FALSE;
}

/**
* g_date_set_day:
* @date: a #GDate_t
* @day: day to set
*
* Sets the day of the month for a #GDate_t. If the resulting
* day-month-year triplet is invalid, the date will be invalid.
*/
void g_date_set_day(GDate_t    *d,	GDateDay  day)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(g_date_valid_day(day));

	if (d->julian && !d->dmy) g_date_update_dmy(d);
	d->julian = FALSE;

	d->day = day;

	if (g_date_valid_dmy(d->day, d->month, d->year))
		d->dmy = TRUE;
	else
		d->dmy = FALSE;
}

/**
* g_date_set_year:
* @date: a #GDate_t
* @year: year to set
*
* Sets the year for a #GDate_t. If the resulting day-month-year
* triplet is invalid, the date will be invalid.
*/
void g_date_set_year(GDate_t     *d,	GDateYear  y)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(g_date_valid_year(y));

	if (d->julian && !d->dmy) g_date_update_dmy(d);
	d->julian = FALSE;

	d->year = y;

	if (g_date_valid_dmy(d->day, d->month, d->year))
		d->dmy = TRUE;
	else
		d->dmy = FALSE;
}

/**
* g_date_set_dmy:
* @date: a #GDate_t
* @day: day
* @month: month
* @y: year
*
* Sets the value of a #GDate_t from a day, month, and year.
* The day-month-year triplet must be valid; if you aren't
* sure it is, call g_date_valid_dmy() to check before you
* set it.
*/
void g_date_set_dmy(GDate_t      *d,	GDateDay    day,	GDateMonth  m,	GDateYear   y)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(g_date_valid_dmy(day, m, y));

	d->julian = FALSE;

	d->month = m;
	d->day = day;
	d->year = y;

	d->dmy = TRUE;
}

/**
* g_date_set_julian:
* @date: a #GDate_t
* @julian_date: Julian day number (days since January 1, Year 1)
*
* Sets the value of a #GDate_t from a Julian day number.
*/
void g_date_set_julian(GDate_t   *d,	guint32  j)
{
	//g_return_if_fail(d != NULL);
	//g_return_if_fail(g_date_valid_julian(j));

	d->julian_days = j;
	d->julian = TRUE;
	d->dmy = FALSE;
}

/**
* g_date_is_first_of_month:
* @date: a #GDate_t to check
*
* Returns %TRUE if the date is on the first of a month.
* The date must be valid.
*
* Returns: %TRUE if the date is the first of the month
*/
gboolean g_date_is_first_of_month(const GDate_t *d)
{
	//g_return_val_if_fail(g_date_valid(d), FALSE);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, FALSE);

	if (d->day == 1) return TRUE;
	else return FALSE;
}

/**
* g_date_is_last_of_month:
* @date: a #GDate_t to check
*
* Returns %TRUE if the date is the last day of the month.
* The date must be valid.
*
* Returns: %TRUE if the date is the last day of the month
*/
gboolean g_date_is_last_of_month(const GDate_t *d)
{
	gint idx;

	//g_return_val_if_fail(g_date_valid(d), FALSE);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_val_if_fail(d->dmy, FALSE);

	idx = g_date_is_leap_year(d->year) ? 1 : 0;

	if (d->day == days_in_months[idx][d->month]) return TRUE;
	else return FALSE;
}

/**
* g_date_add_days:
* @date: a #GDate_t to increment
* @n_days: number of days to move the date forward
*
* Increments a date some number of days.
* To move forward by weeks, add weeks*7 days.
* The date must be valid.
*/
void g_date_add_days(GDate_t *d, guint  ndays)
{
	//g_return_if_fail(g_date_valid(d));

	if (!d->julian)
		g_date_update_julian(d);

	//g_return_if_fail(d->julian);
	//g_return_if_fail(ndays <= MAXINT32 - d->julian_days);

	d->julian_days += ndays;
	d->dmy = FALSE;
}

/**
* g_date_subtract_days:
* @date: a #GDate_t to decrement
* @n_days: number of days to move
*
* Moves a date some number of days into the past.
* To move by weeks, just move by weeks*7 days.
* The date must be valid.
*/
void g_date_subtract_days(GDate_t *d,	guint  ndays)
{
	//g_return_if_fail(g_date_valid(d));

	if (!d->julian)
		g_date_update_julian(d);

	//g_return_if_fail(d->julian);
	//g_return_if_fail(d->julian_days > ndays);

	d->julian_days -= ndays;
	d->dmy = FALSE;
}

/**
* g_date_add_months:
* @date: a #GDate_t to increment
* @n_months: number of months to move forward
*
* Increments a date by some number of months.
* If the day of the month is greater than 28,
* this routine may change the day of the month
* (because the destination month may not have
* the current day in it). The date must be valid.
*/
void g_date_add_months(GDate_t *d,	guint  nmonths)
{
	guint years, months;
	gint idx;

	//g_return_if_fail(g_date_valid(d));

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_if_fail(d->dmy != 0);
	//g_return_if_fail(nmonths <= MAXINT - (d->month - 1));

	nmonths += d->month - 1;

	years = nmonths / 12;
	months = nmonths % 12;

	//g_return_if_fail(years <= (guint)(MAXINT16 - d->year));

	d->month = months + 1;
	d->year += years;

	idx = g_date_is_leap_year(d->year) ? 1 : 0;

	if (d->day > days_in_months[idx][d->month])
		d->day = days_in_months[idx][d->month];

	d->julian = FALSE;

	//g_return_if_fail(g_date_valid(d));
}

/**
* g_date_subtract_months:
* @date: a #GDate_t to decrement
* @n_months: number of months to move
*
* Moves a date some number of months into the past.
* If the current day of the month doesn't exist in
* the destination month, the day of the month
* may change. The date must be valid.
*/
void g_date_subtract_months(GDate_t *d,	guint  nmonths)
{
	guint years, months;
	gint idx;

	//g_return_if_fail(g_date_valid(d));

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_if_fail(d->dmy != 0);

	years = nmonths / 12;
	months = nmonths % 12;

	//g_return_if_fail(d->year > years);

	d->year -= years;

	if (d->month > months) d->month -= months;
	else
	{
		months -= d->month;
		d->month = 12 - months;
		d->year -= 1;
	}

	idx = g_date_is_leap_year(d->year) ? 1 : 0;

	if (d->day > days_in_months[idx][d->month])
		d->day = days_in_months[idx][d->month];

	d->julian = FALSE;

	//g_return_if_fail(g_date_valid(d));
}

/**
* g_date_add_years:
* @date: a #GDate_t to increment
* @n_years: number of years to move forward
*
* Increments a date by some number of years.
* If the date is February 29, and the destination
* year is not a leap year, the date will be changed
* to February 28. The date must be valid.
*/
void g_date_add_years(GDate_t *d,	guint  nyears)
{
	//g_return_if_fail(g_date_valid(d));

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_if_fail(d->dmy != 0);
	//g_return_if_fail(nyears <= (guint)(MAXINT16 - d->year));

	d->year += nyears;

	if (d->month == 2 && d->day == 29)
	{
		if (!g_date_is_leap_year(d->year))
			d->day = 28;
	}

	d->julian = FALSE;
}

/**
* g_date_subtract_years:
* @date: a #GDate_t to decrement
* @n_years: number of years to move
*
* Moves a date some number of years into the past.
* If the current day doesn't exist in the destination
* year (i.e. it's February 29 and you move to a non-leap-year)
* then the day is changed to February 29. The date
* must be valid.
*/
void g_date_subtract_years(GDate_t *d,	guint  nyears)
{
	//g_return_if_fail(g_date_valid(d));

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_if_fail(d->dmy != 0);
	//g_return_if_fail(d->year > nyears);

	d->year -= nyears;

	if (d->month == 2 && d->day == 29)
	{
		if (!g_date_is_leap_year(d->year))
			d->day = 28;
	}

	d->julian = FALSE;
}

/**
* g_date_is_leap_year:
* @year: year to check
*
* Returns %TRUE if the year is a leap year.
*
* For the purposes of this function, leap year is every year
* divisible by 4 unless that year is divisible by 100. If it
* is divisible by 100 it would be a leap year only if that year
* is also divisible by 400.
*
* Returns: %TRUE if the year is a leap year
*/
gboolean g_date_is_leap_year(GDateYear year)
{
	//g_return_val_if_fail(g_date_valid_year(year), FALSE);

	return ((((year % 4) == 0) && ((year % 100) != 0)) ||
		(year % 400) == 0);
}

/**
* g_date_get_days_in_month:
* @month: month
* @year: year
*
* Returns the number of days in a month, taking leap
* years into account.
*
* Returns: number of days in @month during the @year
*/
guint8 g_date_get_days_in_month(GDateMonth month,
	GDateYear  year)
{
	gint idx;

	//g_return_val_if_fail(g_date_valid_year(year), 0);
	//g_return_val_if_fail(g_date_valid_month(month), 0);

	idx = g_date_is_leap_year(year) ? 1 : 0;

	return days_in_months[idx][month];
}

/**
* g_date_get_monday_weeks_in_year:
* @year: a year
*
* Returns the number of weeks in the year, where weeks
* are taken to start on Monday. Will be 52 or 53. The
* date must be valid. (Years always have 52 7-day periods,
* plus 1 or 2 extra days depending on whether it's a leap
* year. This function is basically telling you how many
* Mondays are in the year, i.e. there are 53 Mondays if
* one of the extra days happens to be a Monday.)
*
* Returns: number of Mondays in the year
*/
guint8 g_date_get_monday_weeks_in_year(GDateYear year)
{
	GDate_t d;

	//g_return_val_if_fail(g_date_valid_year(year), 0);

	g_date_clear(&d, 1);
	g_date_set_dmy(&d, 1, 1, year);
	if (g_date_get_weekday(&d) == G_DATE_MONDAY) return 53;
	g_date_set_dmy(&d, 31, 12, year);
	if (g_date_get_weekday(&d) == G_DATE_MONDAY) return 53;
	if (g_date_is_leap_year(year))
	{
		g_date_set_dmy(&d, 2, 1, year);
		if (g_date_get_weekday(&d) == G_DATE_MONDAY) return 53;
		g_date_set_dmy(&d, 30, 12, year);
		if (g_date_get_weekday(&d) == G_DATE_MONDAY) return 53;
	}
	return 52;
}

/**
* g_date_get_sunday_weeks_in_year:
* @year: year to count weeks in
*
* Returns the number of weeks in the year, where weeks
* are taken to start on Sunday. Will be 52 or 53. The
* date must be valid. (Years always have 52 7-day periods,
* plus 1 or 2 extra days depending on whether it's a leap
* year. This function is basically telling you how many
* Sundays are in the year, i.e. there are 53 Sundays if
* one of the extra days happens to be a Sunday.)
*
* Returns: the number of weeks in @year
*/
guint8 g_date_get_sunday_weeks_in_year(GDateYear year)
{
	GDate_t d;

	//g_return_val_if_fail(g_date_valid_year(year), 0);

	g_date_clear(&d, 1);
	g_date_set_dmy(&d, 1, 1, year);
	if (g_date_get_weekday(&d) == G_DATE_SUNDAY) return 53;
	g_date_set_dmy(&d, 31, 12, year);
	if (g_date_get_weekday(&d) == G_DATE_SUNDAY) return 53;
	if (g_date_is_leap_year(year))
	{
		g_date_set_dmy(&d, 2, 1, year);
		if (g_date_get_weekday(&d) == G_DATE_SUNDAY) return 53;
		g_date_set_dmy(&d, 30, 12, year);
		if (g_date_get_weekday(&d) == G_DATE_SUNDAY) return 53;
	}
	return 52;
}

/**
* g_date_compare:
* @lhs: first date to compare
* @rhs: second date to compare
*
* qsort()-style comparison function for dates.
* Both dates must be valid.
*
* Returns: 0 for equal, less than zero if @lhs is less than @rhs,
*     greater than zero if @lhs is greater than @rhs
*/
gint g_date_compare(const GDate_t *lhs,
	const GDate_t *rhs)
{
	//g_return_val_if_fail(lhs != NULL, 0);
	//g_return_val_if_fail(rhs != NULL, 0);
	//g_return_val_if_fail(g_date_valid(lhs), 0);
	//g_return_val_if_fail(g_date_valid(rhs), 0);

	/* Remember the self-comparison case! I think it works right now. */

	while (TRUE)
	{
		if (lhs->julian && rhs->julian)
		{
			if (lhs->julian_days < rhs->julian_days) return -1;
			else if (lhs->julian_days > rhs->julian_days) return 1;
			else                                          return 0;
		}
		else if (lhs->dmy && rhs->dmy)
		{
			if (lhs->year < rhs->year)               return -1;
			else if (lhs->year > rhs->year)               return 1;
			else
			{
				if (lhs->month < rhs->month)         return -1;
				else if (lhs->month > rhs->month)         return 1;
				else
				{
					if (lhs->day < rhs->day)              return -1;
					else if (lhs->day > rhs->day)              return 1;
					else                                       return 0;
				}

			}

		}
		else
		{
			if (!lhs->julian) g_date_update_julian(lhs);
			if (!rhs->julian) g_date_update_julian(rhs);
			//g_return_val_if_fail(lhs->julian, 0);
			//g_return_val_if_fail(rhs->julian, 0);
		}

	}
	return 0; /* warnings */
}

/**
* g_date_to_struct_tm:
* @date: a #GDate_t to set the struct tm from
* @tm: (not nullable): struct tm to fill
*
* Fills in the date-related bits of a struct tm using the @date value.
* Initializes the non-date parts with something sane but meaningless.
*/
void g_date_to_struct_tm(const GDate_t *d,
	struct tm   *tm)
{
	GDateWeekday day;

	//g_return_if_fail(g_date_valid(d));
	//g_return_if_fail(tm != NULL);

	if (!d->dmy)
		g_date_update_dmy(d);

	//g_return_if_fail(d->dmy != 0);

	/* zero all the irrelevant fields to be sure they're valid */

	/* On Linux and maybe other systems, there are weird non-POSIX
	* fields on the end of struct tm that choke strftime if they
	* contain garbage.  So we need to 0 the entire struct, not just the
	* fields we know to exist.
	*/

	memset(tm, 0x0, sizeof(struct tm));

	tm->tm_mday = d->day;
	tm->tm_mon = d->month - 1; /* 0-11 goes in tm */
	tm->tm_year = ((int)d->year) - 1900; /* X/Open says tm_year can be negative */

	day = g_date_get_weekday(d);
	if (day == 7) day = 0; /* struct tm wants days since Sunday, so Sunday is 0 */

	tm->tm_wday = (int)day;

	tm->tm_yday = g_date_get_day_of_year(d) - 1; /* 0 to 365 */
	tm->tm_isdst = -1; /* -1 means "information not available" */
}

/**
* g_date_clamp:
* @date: a #GDate_t to clamp
* @min_date: minimum accepted value for @date
* @max_date: maximum accepted value for @date
*
* If @date is prior to @min_date, sets @date equal to @min_date.
* If @date falls after @max_date, sets @date equal to @max_date.
* Otherwise, @date is unchanged.
* Either of @min_date and @max_date may be %NULL.
* All non-%NULL dates must be valid.
*/
void g_date_clamp(GDate_t       *date,
	const GDate_t *min_date,
	const GDate_t *max_date)
{
	//g_return_if_fail(g_date_valid(date));

	if (min_date != NULL)
		//g_return_if_fail(g_date_valid(min_date));

	if (max_date != NULL)
		//g_return_if_fail(g_date_valid(max_date));

	if (min_date != NULL && max_date != NULL)
		//g_return_if_fail(g_date_compare(min_date, max_date) <= 0);

	if (min_date && g_date_compare(date, min_date) < 0)
		*date = *min_date;

	if (max_date && g_date_compare(max_date, date) < 0)
		*date = *max_date;
}

/**
* g_date_order:
* @date1: the first date
* @date2: the second date
*
* Checks if @date1 is less than or equal to @date2,
* and swap the values if this is not the case.
*/
void g_date_order(GDate_t *date1,
	GDate_t *date2)
{
	//g_return_if_fail(g_date_valid(date1));
	//g_return_if_fail(g_date_valid(date2));

	if (g_date_compare(date1, date2) > 0)
	{
		GDate_t tmp = *date1;
		*date1 = *date2;
		*date2 = tmp;
	}
}
