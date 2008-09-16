#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "thermorwsd.h"
#include "common.h"
#include "datadisplay.h"
#include "datadispcsv.h"

#include "list.h"
#include "select.h"
#include "debug.h"

#define TENTHS(a) a / 10, abs(a %10)

extern struct ws_prog_options prog_options;
extern struct datum_handler data_handlers[DATA_TYPE_MAX];

/*-----------------------------------------------------------------*/
void
set_csv_default_text()
{
prog_options.default_txt[FLD_MAX] = DEF_MAX_TXT;
prog_options.default_txt[FLD_MIN] = DEF_MIN_TXT;
prog_options.default_txt[FLD_CUR] = DEF_CUR_TXT;
prog_options.default_txt[FLD_NO_READING] = DEF_NO_READING_TXT;

prog_options.default_txt[FLD_DATA_PREFIX] = DEF_DATA_PREFIX_TXT;

prog_options.default_txt[FLD_DATA_SEPARATOR] = DEF_DATA_SEPARATOR_TXT;
prog_options.default_txt[FLD_UNIT_SEPARATOR] = DEF_UNIT_SEPARATOR_TXT;

prog_options.default_txt[FLD_TIME] = DEF_TIME_TXT;
prog_options.default_txt[FLD_TIME_SUFFIX] = DEF_TIME_SUFFIX_TXT;

prog_options.default_txt[FLD_DATE] = DEF_DATE_TXT;
prog_options.default_txt[FLD_DATE_SUFFIX] = DEF_DATE_SUFFIX_TXT;

prog_options.default_txt[FLD_IN_TEMP] = DEF_IN_TEMP_TXT;
prog_options.default_txt[FLD_IN_TEMP_SUFFIX] = DEF_IN_TEMP_SUFFIX_TXT;

prog_options.default_txt[FLD_OUT_TEMP] = DEF_OUT_TEMP_TXT;
prog_options.default_txt[FLD_OUT_TEMP_SUFFIX] = DEF_OUT_TEMP_SUFFIX_TXT;

prog_options.default_txt[FLD_RAIN] = DEF_RAIN_TXT;
prog_options.default_txt[FLD_RAIN_SUFFIX] = DEF_RAIN_SUFFIX_TXT;

prog_options.default_txt[FLD_HUMIDITY] = DEF_HUMIDITY_TXT;
prog_options.default_txt[FLD_HUMIDITY_SUFFIX] = DEF_HUMIDITY_SUFFIX_TXT;

prog_options.default_txt[FLD_PRESSURE] = DEF_PRESSURE_TXT;
prog_options.default_txt[FLD_PRESSURE_SUFFIX] = DEF_PRESSURE_SUFFIX_TXT;

prog_options.default_txt[FLD_WIND_DIR] = DEF_WIND_DIR_TXT;
prog_options.default_txt[FLD_WIND_DIR_SUFFIX] = DEF_WIND_DIR_SUFFIX_TXT;

prog_options.default_txt[FLD_WIND_SPEED] = DEF_WIND_SPEED_TXT;
prog_options.default_txt[FLD_WIND_SPEED_SUFFIX] = DEF_WIND_SPEED_SUFFIX_TXT;

prog_options.default_txt[FLD_WIND_GUST] = DEF_WIND_GUST_TXT;
prog_options.default_txt[FLD_WIND_GUST_SUFFIX] = DEF_WIND_GUST_SUFFIX_TXT;

prog_options.default_txt[FLD_FORECAST] = DEF_FORECAST_TXT;
prog_options.default_txt[FLD_FORECAST_SUFFIX] = DEF_FORECAST_SUFFIX_TXT;

prog_options.default_txt[FLD_TREND] = DEF_TREND_TXT;
prog_options.default_txt[FLD_TREND_SUFFIX] = DEF_TREND_SUFFIX_TXT;

prog_options.default_txt[FLD_WIND_CHILL] = DEF_WIND_CHILL_TXT;
prog_options.default_txt[FLD_WIND_CHILL_SUFFIX] = DEF_WIND_CHILL_SUFFIX_TXT;

prog_options.default_txt[FLD_UNKNOWN1] = DEF_UNKNOWN1_TXT;
prog_options.default_txt[FLD_UNKNOWN1_SUFFIX] = DEF_UNKNOWN1_SUFFIX_TXT;
}

/*-----------------------------------------------------------------*/
void
set_csv_data_displayers()
{
int x;

/* default all display handlers to NULL */
for (x = 0; x < DATA_TYPE_MAX; x++)
	{
	data_handlers[x].display_handler = NULL;
	}

/* override generic */
/* CSV mode uses pretty print's display handlers for now */
data_handlers[DATA_TYPE_DATE].display_handler = display_date;
data_handlers[DATA_TYPE_TIME].display_handler = display_time;
data_handlers[DATA_TYPE_PRESSURE].display_handler = display_pressure;
data_handlers[DATA_TYPE_RAIN].display_handler = display_rain;
data_handlers[DATA_TYPE_OUT_TEMP].display_handler = display_outtemp;
data_handlers[DATA_TYPE_IN_TEMP].display_handler = display_intemp;
data_handlers[DATA_TYPE_WIND_DIR].display_handler = display_winddir;
data_handlers[DATA_TYPE_WIND_SPEED].display_handler = display_windspeed;
data_handlers[DATA_TYPE_WIND_GUST].display_handler = display_windgust;
data_handlers[DATA_TYPE_HUMIDITY].display_handler = display_humidity;
data_handlers[DATA_TYPE_FORECAST].display_handler = display_forecast;
data_handlers[DATA_TYPE_TREND].display_handler = display_trend;
data_handlers[DATA_TYPE_WIND_CHILL].display_handler = display_windchill;
data_handlers[DATA_TYPE_UNKNOWN1].display_handler = display_unknown1;

return;
}

