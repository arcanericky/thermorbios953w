#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
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
int x;

if (prog_options.debug_lvl < 4)
	{
	return 0;
	}

fprintf(prog_options.output_fs, "IN  ");

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

return 0;
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

return 0;
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

return 0;
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

return 0;
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

return 0;
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
#if 0
rain = rain * 5;
#endif
DISPLAY_HANDLER(datatype, data[7], rain);

return 0;
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

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_windspeed(int *data)
{
int datatype;
short speed;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_WIND_SPEED)
	{
	/* FIXME: Warning here */
	}

speed = (data[5] << 8) + data[6];

DISPLAY_HANDLER(datatype, data[7], speed);

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_windgust(int *data)
{
int datatype;
short gust;

log_data(data);

datatype = data[3];
if (datatype != DATA_TYPE_WIND_GUST)
	{
	/* FIXME: Warning here */
	}

gust = (data[5] << 8) + data[6];

DISPLAY_HANDLER(datatype, data[7], gust);

return 0;
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
wind_direction = abs(wind_direction);

/* Needed for --fuzzy option so it falls within bounds */
wind_direction = wind_direction % 17;

switch(wind_direction)
	{
	case  0x0: wind_direction = 157; break;
	case  0x1: wind_direction = 225; break;
	case  0x2: wind_direction = 180; break;
	case  0x3: wind_direction = 202; break;
	case  0x4: wind_direction = 135; break;
	case  0x5: wind_direction = 247; break;
	case  0x6: wind_direction = 112; break;
	case  0x7: wind_direction = 270; break;
	case  0x8: wind_direction =  45; break;
	case  0x9: wind_direction = 337; break;
	case  0xa: wind_direction =  22; break;
	case  0xb: wind_direction =   0; break;
	case  0xc: wind_direction =  67; break;
	case  0xd: wind_direction = 315; break;
	case  0xe: wind_direction =  90; break;
	case  0xf: wind_direction = 292; break;
	case 0x10: /* sensor not connected */ break;
	}

DISPLAY_HANDLER(datatype, data[7], wind_direction);

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_forecast(int *data)
{
int datatype;
int forecast;

log_data(data);

datatype = data[3];

forecast = data[5];

DISPLAY_HANDLER(datatype, data[7], forecast);

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_trend(int *data)
{
int datatype;

log_data(data);

datatype = data[3];

DISPLAY_HANDLER(datatype, data[7], &data[4]);

return 0;
}

/*-----------------------------------------------------------------*/
int
dh_windchill(int *data)
{
int datatype;
int chill;

log_data(data);

datatype = data[3];

chill = (data[5] << 8) + data[6];
chill = chill - 740;

DISPLAY_HANDLER(datatype, data[7], chill);

return 0;
}
