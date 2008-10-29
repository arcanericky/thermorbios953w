/* Thermor / BIOS Weather Station Server
 *
 * This file is file is part of ws9xxd.
 *
 * ws9xxd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ws9xxd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ws9xxd.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include "setoptions.h"
#include "datadisplay.h"
#include "datadispcsv.h"
#include "common.h"

extern struct ws_prog_options prog_options;
extern struct datum_handler data_handlers[DATA_TYPE_MAX];

/* these are overridden from datadisplay.c */
static int display_time(int, int *);
static int display_date(int, int *);
static int display_winddir(int, int);
static int display_forecast(int, int);
static int display_trend(int, int *);

char bw9xx_date[9];
char bw9xx_time[9];

/*-----------------------------------------------------------------*/
void
set_csv_default_text()
{
/* specific to csv */
prog_options.output_txt[FLD_NO_READING] = "";
prog_options.output_txt[FLD_DATA_PREFIX] = "";
prog_options.output_txt[FLD_DATA_SEPARATOR] = ",";
prog_options.output_txt[FLD_UNIT_SEPARATOR] = ",";
prog_options.output_txt[FLD_RANGE_SEPARATOR] = ",";
strcpy(bw9xx_date, "");
strcpy(bw9xx_time, "");

prog_options.default_txt[FLD_MAX] = DEF_MAX_TXT;
prog_options.default_txt[FLD_MIN] = DEF_MIN_TXT;
prog_options.default_txt[FLD_CUR] = DEF_CUR_TXT;

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

/*-----------------------------------------------------------------*/
static int
display_date(int datatype, int *data)
{
sprintf(bw9xx_date, "%2.2d/%2.2d/%2.2d", data[1], data[2], data[0]);

output_data("%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_DATE],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	bw9xx_date,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_DATE_SUFFIX]);

return (0);
}

/*-----------------------------------------------------------------*/
static int
display_time(int datatype, int *data)
{
sprintf(bw9xx_time, "%2.2d:%2.2d:%2.2d", data[0], data[1], data[2]);

output_data("%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_TIME],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	bw9xx_time,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_TIME_SUFFIX]);

return (0);
}

/*-----------------------------------------------------------------*/
static int
display_winddir(int datatype, int data)
{
char *value;

if (data == 0x10)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d", data);
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_WIND_DIR],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_WIND_DIR_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
static int
display_forecast(int datatype, int data)
{
char *forecast;

switch (data)
	{
	case 0:
		forecast = "Sunny";
		break;
	case 1:
		forecast = "Partly cloudy";
		break;
	case 2:
		forecast = "Cloudy";
		break;
	case 3:
		forecast = "Rainy";
		break;
	default:
		forecast = prog_options.output_txt[FLD_NO_READING];
		break;
	}

output_data("%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_FORECAST],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	forecast,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_FORECAST_SUFFIX]);

return 0;
}

/*-----------------------------------------------------------------*/
static int
display_trend(int datatype, int *data)
{
output_data("%s%s%s%s%d %d%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_TREND],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	*(data),
	*(data + 1),
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_TREND_SUFFIX]);

return 0;
}

