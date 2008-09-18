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

#include "list.h"
#include "select.h"

#define TENTHS(a) a / 10, abs(a %10)

extern struct ws_prog_options prog_options;
extern struct datum_handler data_handlers[DATA_TYPE_MAX];
extern char bw9xx_date[];
extern char bw9xx_time[];

static int display_date(int, int *);
static int display_time(int, int *);
static int display_winddir(int, int);
static int display_forecast(int, int);
static int display_trend(int, int *);

/*-----------------------------------------------------------------*/
void
set_pp_default_text()
{
prog_options.default_txt[FLD_MAX] = DEF_MAX_TXT;
prog_options.default_txt[FLD_MIN] = DEF_MIN_TXT;
prog_options.default_txt[FLD_CUR] = DEF_CUR_TXT;
prog_options.default_txt[FLD_NO_READING] = DEF_NO_READING_TXT;

prog_options.default_txt[FLD_DATA_PREFIX] = DEF_DATA_PREFIX_TXT;

prog_options.default_txt[FLD_DATA_SEPARATOR] = DEF_DATA_SEPARATOR_TXT;
prog_options.default_txt[FLD_UNIT_SEPARATOR] = DEF_UNIT_SEPARATOR_TXT;
prog_options.default_txt[FLD_RANGE_SEPARATOR] = DEF_RANGE_SEPARATOR_TXT;

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
set_pp_data_displayers()
{
int x;

/* default all handlers to generic */
for (x = 0; x < DATA_TYPE_MAX; x++)
	{
	data_handlers[x].display_handler = NULL;
	}

/* override generic */
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
void
output_data(const char *fmt, ...)
{
va_list ap;
char *outbuf;

if (prog_options.data_csv)
	{
	char stampbuf[21];  /* "CCYY/MM/DD,HH:MM:SS," + null */
	char *databuf;
	time_t t;

	/* populate date buffer */
	t = time(NULL);
	strftime(stampbuf, sizeof (stampbuf), "%F,%T",
		localtime(&t));

	va_start(ap, fmt);

	/* databuf is dynamically allocated */
	databuf = dyn_vsprintf(fmt, ap);
	va_end(ap);
	if (databuf == NULL)
		{
		return;
		}

	/* now outbuf is dynamically allocated */
	outbuf = dyn_sprintf("%s%s%s%s%s%s%s",
		stampbuf,
		prog_options.output_txt[FLD_DATA_SEPARATOR],
		bw9xx_date,
		prog_options.output_txt[FLD_DATA_SEPARATOR],
		bw9xx_time,
		prog_options.output_txt[FLD_DATA_SEPARATOR],
		databuf);

	/* done with databuff - free it */
	xfree(databuf);
	}
else
	{
	va_start(ap, fmt);
	outbuf = dyn_vsprintf(fmt, ap);
	va_end(ap);
	}

/* if bad outbuf, no need to proceed */
if (outbuf == NULL)
	{
	return;
	}

/* send to log file */
fprintf(prog_options.output_fs, "%s", outbuf);
fflush(prog_options.output_fs);

/* and queue for server output */
wsd_queue_broadcast(outbuf, strlen(outbuf));

/* outbuf was dynamically allocated */
xfree(outbuf);

return;
}

/*-----------------------------------------------------------------*/
static char
*getrangetxt(int x)
{
char *text;

switch (x)
	{
	case 0x01:
		text = prog_options.output_txt[FLD_MIN];
		break;
	case 0x02:
		text = prog_options.output_txt[FLD_MAX];
		break;
	case 0x03:
		text = prog_options.output_txt[FLD_CUR];
		break;
	default:
		text = "";
		break;
	}

return text;
}

/*-----------------------------------------------------------------*/
static int
display_date(int datatype, int *data)
{
output_data("%s%s%s%2.2d/%2.2d/%2.2d%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_DATE],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	data[1],
	data[2],
	data[0],
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_DATE_SUFFIX]);

return (0);
}

/*-----------------------------------------------------------------*/
static int
display_time(int datatype, int *data)
{
output_data("%s%s%s%2.2d:%2.2d:%2.2d%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_TIME],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	data[0],
	data[1],
	data[2],
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_TIME_SUFFIX]);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_humidity(int datatype, int data)
{
char *value;

if ((data | 0xFF00) == data)
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

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_HUMIDITY],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_HUMIDITY_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_intemp(int datatype, int data)
{
char *value;

if ((data | 0xFF00) == data)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_IN_TEMP],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_IN_TEMP_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_outtemp(int datatype, int data)
{
char *value;

if ((data | 0xFF00) == data)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_OUT_TEMP],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_OUT_TEMP_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_pressure(int datatype, int data)
{
char *value;

if ((data | 0xFF00) == data)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_PRESSURE],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_PRESSURE_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_rain(int datatype, int data)
{
char *value;

if ((data | 0xFF00) == data)
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

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_RAIN],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_RAIN_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_windspeed(int datatype, int data)
{
char *value;

if ((data | 0xFFFF) == data)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_WIND_SPEED],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_WIND_SPEED_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_windgust(int datatype, int data)
{
char *value;

if ((data | 0xFFFF) == data)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_WIND_GUST],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_WIND_GUST_SUFFIX]);

xfree(value);

return 0;
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

output_data("%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
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

output_data("%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
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
output_data("%s%s%s%d %d%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	prog_options.output_txt[FLD_TREND],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	*(data),
	*(data + 1),
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_TREND_SUFFIX]);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_windchill(int datatype, int data)
{
char *value;

if (data == 48028)
	{
	value = dyn_sprintf("%s", prog_options.output_txt[FLD_NO_READING]);
	}
else
	{
	value = dyn_sprintf("%d.%d", TENTHS(data));
	}

if (value == NULL)
	{
	return 0;
	}

output_data("%s%s%s%s%s%s%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_WIND_CHILL],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	value,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_WIND_CHILL_SUFFIX]);

xfree(value);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_unknown1(int datatype, int data)
{
output_data("%s%s%s%s%s%d%s%s\n",
	prog_options.output_txt[FLD_DATA_PREFIX],
	getrangetxt(datatype),
	prog_options.output_txt[FLD_RANGE_SEPARATOR],
	prog_options.output_txt[FLD_UNKNOWN1],
	prog_options.output_txt[FLD_DATA_SEPARATOR],
	data,
	prog_options.output_txt[FLD_UNIT_SEPARATOR],
	prog_options.output_txt[FLD_UNKNOWN1_SUFFIX]);

return 0;
}
