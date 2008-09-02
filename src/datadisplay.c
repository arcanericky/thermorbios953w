#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <stdarg.h>

#include "thermorwsd.h"
#include "common.h"

#include "list.h"
#include "select.h"
#include "debug.h"

#define TENTHS(a) a / 10, abs(a %10)

extern struct ws_prog_options prog_options;

/*-----------------------------------------------------------------*/
void
output_data(const char *fmt, ...)
{
va_list ap;
char outbuf[80];
int ret;

va_start(ap, fmt);

ret = vsnprintf(outbuf, sizeof (outbuf), fmt, ap);
if (ret >= sizeof (outbuf))
	{
	fprintf(prog_options.output_fs,
		"Output data truncated: %s\n", outbuf);
	}

fprintf(prog_options.output_fs, outbuf);
fflush(prog_options.output_fs);

wsd_queue_broadcast(outbuf, strlen(outbuf));

return;
}

/*-----------------------------------------------------------------*/
char
*hlc(int x)
{
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
output_data("%s%s%2.2d/%2.2d/%2.2d\n",
	prog_options.data_prefix, prog_options.date_txt, data[1],
	data[2], data[0]);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_humidity(int datatype, int data)
{
output_data("%s %s%d%s\n", hlc(datatype),
	prog_options.humidity_txt, data, prog_options.humidity_suffix_txt);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_intemp(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s %s%s\n", hlc(datatype),
		prog_options.in_temp_txt, prog_options.no_reading_txt);
	}
else
	{
	output_data("%s %s%d.%d%s\n",
		hlc(datatype), prog_options.in_temp_txt, TENTHS(data),
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
	output_data("%s %s%s\n", hlc(datatype),
		prog_options.out_temp_txt, prog_options.no_reading_txt);
	}
else
	{
	output_data("%s %s%d.%d%s\n",
		hlc(datatype), prog_options.out_temp_txt, TENTHS(data),
		prog_options.out_temp_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
display_pressure(int datatype, int data)
{
output_data("%s %s%d.%d%s\n",
	hlc(datatype), prog_options.pressure_txt, TENTHS(data),
	prog_options.pressure_suffix_txt);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_rain(int datatype, int data)
{
if ((data | 0xFF00) == data)
	{
	output_data("%s %s%s\n",
		hlc(datatype), prog_options.rain_txt, prog_options.no_reading_txt);
	}
else
	{
	output_data("%s %s%d.%d%s\n",
		hlc(datatype), prog_options.rain_txt, TENTHS(data),
		prog_options.rain_suffix_txt);
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
display_time(int datatype, int *data)
{
output_data("%s%s%2.2d:%2.2d:%2.2d\n",
	prog_options.data_prefix, prog_options.time_txt, data[0],
	data[1], data[2]);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_wind1(int datatype, int data)
{
output_data("%s Wind1: %d\n",
	hlc(datatype), data);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_wind2(int datatype, int data)
{
output_data("%s Wind2: %d\n",
	hlc(datatype), data);

return (0);
}

/*-----------------------------------------------------------------*/
int
display_winddir(int datatype, int data)
{
if (data == 0x10)
	{
	fprintf(prog_options.output_fs, "Datatype is: %x\n", datatype);
	output_data("%s %s%s\n", hlc(datatype),
		prog_options.wind_dir_txt, prog_options.no_reading_txt);
	}
else
	{
	output_data("%s %s%d.%d\n",
		hlc(datatype), prog_options.wind_dir_txt, TENTHS(data));
	}

return 0;
}
