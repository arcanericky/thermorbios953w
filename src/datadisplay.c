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

#include "list.h"
#include "select.h"
#include "debug.h"

#define TENTHS(a) a / 10, abs(a %10)

extern struct ws_prog_options prog_options;

/*-----------------------------------------------------------------*/
static char *dyn_vsnprintf(const char *fmt, va_list ap)
{
char *out;
int len;
int ret;

len = vsnprintf(NULL, 0, fmt, ap);
len++;

out = xmalloc(len);
if (out == NULL)
	{
	return NULL;
	}

ret = vsnprintf(out, len, fmt, ap);

if (ret >= len)
	{
	xfree(out);
	return NULL;
	}

return out;
}

/*-----------------------------------------------------------------*/
static char *dyn_snprintf(const char *fmt, ...)
{
va_list ap;
char *out;

va_start(ap, fmt);

out = dyn_vsnprintf(fmt, ap);

va_end(ap);

return out;
}


/*-----------------------------------------------------------------*/
static void
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
	strftime(stampbuf, sizeof (stampbuf), "%F,%T,", localtime(&t));

	va_start(ap, fmt);

	/* databuf is dynamically allocated */
	databuf = dyn_vsnprintf(fmt, ap);
	va_end(ap);
	if (databuf == NULL)
		{
		return;
		}

	/* now outbuf is dynamically allocated */
	outbuf = dyn_snprintf("%s,,%s", stampbuf, databuf);

	/* done with databuff - free it */
	xfree(databuf);
	}
else
	{
	va_start(ap, fmt);
	outbuf = dyn_vsnprintf(fmt, ap);
	va_end(ap);
	}

/* if bad outbuf, no need to proceed */
if (outbuf == NULL)
	{
	return;
	}

/* send to log file */
fprintf(prog_options.output_fs, outbuf);
fflush(prog_options.output_fs);

/* and queue for server output */
wsd_queue_broadcast(outbuf, strlen(outbuf));

/* outbuf was dynamically allocated */
xfree(outbuf);

return;
}

/*-----------------------------------------------------------------*/
static char
*hlc(int x)
{
/* FIXME: Horrible fixed size array here */
static char text[15];
char *format = "%s%s";

switch (x)
	{
	case 0x01:
		snprintf(text, sizeof(text), format,
			prog_options.data_prefix, prog_options.min_txt);
		break;
	case 0x02:
		snprintf(text, sizeof(text), format,
			prog_options.data_prefix, prog_options.max_txt);
		break;
	case 0x03:
		snprintf(text, sizeof(text), format,
			prog_options.data_prefix, prog_options.current_txt);
		break;
	default:
		snprintf(text, sizeof(text), format,
			prog_options.data_prefix, "");
		break;
	}

return text;
}

/*-----------------------------------------------------------------*/
int
display_date(int datatype, int *data)
{
output_data("%s%s%s%2.2d/%2.2d/%2.2d%s%s\n",
	prog_options.data_prefix,
	prog_options.date_txt,
	prog_options.data_separator,
	data[1],
	data[2],
	data[0],
	prog_options.unit_separator,
	prog_options.date_suffix_txt);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_time(int datatype, int *data)
{
output_data("%s%s%s%2.2d:%2.2d:%2.2d%s%s\n",
	prog_options.data_prefix,
	prog_options.time_txt,
	prog_options.data_separator,
	data[0],
	data[1],
	data[2],
	prog_options.unit_separator,
	prog_options.time_suffix_txt);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_humidity(int datatype, int data)
{
if ((data | 0xFF) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.humidity_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.humidity_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d%s%s\n",
		hlc(datatype),
		prog_options.humidity_txt,
		prog_options.data_separator,
		data,
		prog_options.unit_separator,
		prog_options.humidity_suffix_txt);
	}

return (0);
}

/*-----------------------------------------------------------------*/
int
display_intemp(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.in_temp_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.in_temp_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.in_temp_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.in_temp_suffix_txt);
	}

return (0);
}

/*-----------------------------------------------------------------*/
int
display_outtemp(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.out_temp_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.out_temp_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.out_temp_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.out_temp_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
display_pressure(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.pressure_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.pressure_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.pressure_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.pressure_suffix_txt);
	}

return (0);
}

/*-----------------------------------------------------------------*/
int
display_rain(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.rain_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.rain_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d%s%s\n",
		hlc(datatype),
		prog_options.rain_txt,
		prog_options.data_separator,
		data,
		prog_options.unit_separator,
		prog_options.rain_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
display_windspeed(int datatype, int data)
{
if ((data | 0xFFFF) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.wind_speed_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.wind_speed_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.wind_speed_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.wind_speed_suffix_txt);
	}

return (0);
}

/*-----------------------------------------------------------------*/
int
display_windgust(int datatype, int data)
{
if ((data | 0xFFFF) == data)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.wind_gust_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.wind_gust_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.wind_gust_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.wind_gust_suffix_txt);
	}

return (0);
}

/*-----------------------------------------------------------------*/
int
display_winddir(int datatype, int data)
{
if (data == 0x10)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.wind_dir_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.wind_dir_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d%s%s\n",
		hlc(datatype),
		prog_options.wind_dir_txt,
		prog_options.data_separator,
		data,
		prog_options.unit_separator,
		prog_options.wind_dir_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
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
		forecast = "-";
		break;
	}

output_data("%s%s%s%s%s%s\n",
	hlc(datatype),
	prog_options.forecast_txt,
	prog_options.data_separator,
	forecast,
	prog_options.unit_separator,
	prog_options.forecast_suffix_txt);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_trend(int datatype, int *data)
{
output_data("%s%s%s%d %d%s%s\n",
	hlc(datatype),
	prog_options.trend_txt,
	prog_options.data_separator,
	*(data),
	*(data + 1),
	prog_options.unit_separator,
	prog_options.trend_suffix_txt);

return 0;
}

/*-----------------------------------------------------------------*/
int
display_windchill(int datatype, int data)
{
if (data == 48028)
	{
	output_data("%s%s%s%s%s%s\n",
		hlc(datatype),
		prog_options.wind_chill_txt,
		prog_options.data_separator,
		prog_options.no_reading_txt,
		prog_options.unit_separator,
		prog_options.wind_chill_suffix_txt);
	}
else
	{
	output_data("%s%s%s%d.%d%s%s\n",
		hlc(datatype),
		prog_options.wind_chill_txt,
		prog_options.data_separator,
		TENTHS(data),
		prog_options.unit_separator,
		prog_options.wind_chill_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
display_unknown1(int datatype, int data)
{
output_data("%s%s%s%d%s%s\n",
	hlc(datatype),
	prog_options.unknown1_txt,
	prog_options.data_separator,
	data,
	prog_options.unit_separator,
	prog_options.unknown1_suffix_txt);

return 0;
}
