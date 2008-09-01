#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "thermorwsd.h"
#include "common.h"

#define DISPLAY_HANDLER(dt, x, y) (*data_handlers[dt].display_handler)(x, y)

extern struct datum_handler data_handlers[DATA_TYPE_MAX];
extern struct ws_prog_options prog_options;

/*-----------------------------------------------------------------*/
static int
log_data(int *data)
{
char text_time[20];
int x;

if (prog_options.debug_lvl < 4)
	{
	return 0;
	}

get_time(text_time);

fprintf(prog_options.output_fs, "%s IN  ", text_time);

for (x = 0; x < NUM_DATA; x++)
	{
	fprintf(prog_options.output_fs, "%2.2X ", data[x]);
	}

fprintf(prog_options.output_fs, "\n");
fflush(prog_options.output_fs);

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_date(int *data)
{
log_data(data);

if (data[3] != DATA_TYPE_DATE)
	{
	/* FIXME: Warning here */
	}

DISPLAY_HANDLER(data[3], data[7], &data[4]);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_humidity(int *data)
{
int datatype;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_HUMIDITY)
	{
	/* FIXME: Warning here */
	}

DISPLAY_HANDLER(datatype, data[7], data[5]);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_intemp(int *data)
{
int datatype;
short temperature;

log_data(data);

datatype = data[3];

if (datatype != DATA_TYPE_IN_TEMP)
	{
	/* FIXME: Warning here */
	}

temperature = (data[5] << 8) + data[6];

if ((temperature | 0xFF00) != temperature)
	{
	temperature = temperature + prog_options.in_temp_adj;
	}

DISPLAY_HANDLER(datatype, data[7], temperature);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_outtemp(int *data)
{
int datatype;
int temperature;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_OUT_TEMP)
	{
	/* FIXME: Warning here */
	}

temperature = (data[5] << 8) + data[6];

if ((temperature | 0xFF00) != temperature)
	{
	temperature = temperature - 400;
	temperature = temperature + prog_options.out_temp_adj;
	}

DISPLAY_HANDLER(datatype, data[7], temperature);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_pressure(int *data)
{
int datatype;
short pressure;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_PRESSURE)
	{
	/* FIXME: Warning here */
	}

pressure = (data[5] << 8) + data[6];

pressure = pressure + prog_options.pressure_adj;

DISPLAY_HANDLER(datatype, data[7], pressure);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_rain(int *data)
{
int rain;
int datatype;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_RAIN)
	{
	/* FIXME: Warning here */
	}

rain = (data[5] << 8) + data[6];
rain = rain * 5;
DISPLAY_HANDLER(datatype, data[7], rain);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_time(int *data)
{
log_data(data);

if (data[3] != DATA_TYPE_TIME)
	{
	/* FIXME: Warning here */
	}

DISPLAY_HANDLER(data[3], data[7], &data[4]);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_wind1(int *data)
{
int datatype;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_WIND_1)
	{
	/* FIXME: Warning here */
	}

DISPLAY_HANDLER(datatype, data[7], data[6]);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_wind2(int *data)
{
int datatype;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_WIND_2)
	{
	/* FIXME: Warning here */
	}

DISPLAY_HANDLER(datatype, data[7], data[6]);

return (0);
}

/*-----------------------------------------------------------------*/
int
dh_winddir(int *data)
{
int datatype;
int wind_direction;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_WIND_DIR)
	{
	/* FIXME: Warning here */
	}

wind_direction = data[5];
wind_direction = wind_direction % 16;
wind_direction = wind_direction * 225;

DISPLAY_HANDLER(datatype, data[7], wind_direction);

return (0);
}
