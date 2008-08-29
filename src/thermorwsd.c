#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <getopt.h>

#include "thermorwsd.h"
#include "bw953.h"
#include "datahandlers.h"
#include "datadisplay.h"

#include "list.h"
#include "select.h"
#include "common.h"
#include "servers.h"

struct datum_handler data_handlers[DATA_TYPE_MAX];

struct ws_prog_options prog_options;
struct bw953_device_options dev_options;

#define NUM_INPUT_EVENTS		4

/*-----------------------------------------------------------------*/
int
set_dev_options()
{
set_bw953_dev_options();

return 0;
}

/*-----------------------------------------------------------------*/
int
set_prog_options(int argc, char *argv[])
{
int c;
int option_index;
static struct option long_options[] = {
	{ "inside-temp-adj",		required_argument, 0, 0 },
	{ "outside-temp-adj",		required_argument, 0, 0 },
	{ "pressure-adj",			required_argument, 0, 0 },
	{ "device-name",			required_argument, 0, 0 },
	{ "log-filename",			required_argument, 0, 0 },
	{ "debug",					required_argument, 0, 0 },

	{ "inside-temp-text",		required_argument, 0, 0 },
	{ "outside-temp-text",		required_argument, 0, 0 },
	{ "humidity-text",			required_argument, 0, 0 },
	{ "pressure-text",			required_argument, 0, 0 },
	{ "rain-text",				required_argument, 0, 0 },

	{ "date-text",				required_argument, 0, 0 },
	{ "time-text",				required_argument, 0, 0 },

	{ 0, 0, 0, 0 }
	};

char option_map[] = "iopdlv";

prog_options.debug_lvl = 3;

prog_options.output_filename = NULL;
prog_options.output_fs = stdout;

prog_options.out_temp_adj = 0;
prog_options.in_temp_adj = 0;
prog_options.pressure_adj = 0;

/* FIXME: Finish implementing user configurable connection */
prog_options.unix_path = UNIX_PATH;

prog_options.device = "/dev/hiddev0";
prog_options.max_txt = "Maximum";
prog_options.min_txt = "Minimum";
prog_options.current_txt = "Current";

prog_options.data_prefix = "DATA: ";
prog_options.time_txt = "Time";
prog_options.date_txt = "Date";

prog_options.in_temp_txt = "Inside Temperature";
prog_options.out_temp_txt = "Outside Temperature";
prog_options.rain_txt = "Rain";
prog_options.humidity_txt = "Humidity";
prog_options.pressure_txt = "Pressure";
prog_options.wind_dir_txt = "Wind Direction";
prog_options.wind_speed_txt = "Wind Speed";

prog_options.no_reading_txt = "No Reading";

while (1)
	{
	c = getopt_long(argc, argv, "i:o:p:d:l:v:",
		long_options, &option_index);
	if (c == -1)
		{
		break;
		}

	if (c == 0)
		{
		c = option_map[option_index];
		}

	switch (c)
		{
		case 'i':
			printf("Inside Temperature Offset: %d\n", atoi(optarg));
			prog_options.in_temp_adj = atoi(optarg);
			break;
		case 'o':
		case 1:
			printf("Outside Temperature Offset: %d\n", atoi(optarg));
			prog_options.out_temp_adj = atoi(optarg);
			break;
		case 'p':
		case 2:
			printf("Pressure Offset: %d\n", atoi(optarg));
			prog_options.pressure_adj = atoi(optarg);
			break;
		case 'd':
		case 3:
			printf("Device Name: %s\n", optarg);
			prog_options.device = optarg;
			break;
		case 'l':
		case 4:
			printf("Log Name: %s\n", optarg);
			prog_options.output_filename = optarg;
			break;
		case 'v':
		case 5:
			printf("Debug Level: %d\n", atoi(optarg));
			prog_options.debug_lvl = atoi(optarg);
		case 6:
			prog_options.in_temp_txt = optarg;
			break;
		case 7:
			prog_options.out_temp_txt = optarg;
			break;
		case 8:
			prog_options.humidity_txt = optarg;
			break;
		case 9:
			prog_options.pressure_txt = optarg;
			break;
		case 10:
			prog_options.rain_txt = optarg;
			break;
		case '?':
printf("wsd9xxd - Weather Station Daemon for Bios/Thermor 953\n");
printf("Usage:\n");
printf("\tws9xxd [options]\n");
printf("Options:\n");
printf("\t--inside-temp-adj\n");
printf("\t--outside-temp-adj\n");
printf("\t--pressure-adj\n");
printf("\t--device-name\n");
printf("\t--log-name\n");
			return -1;
			break;
		}
	}

if (prog_options.output_filename != NULL)
	{
	printf("setting output file: %s\n", prog_options.output_filename);
	prog_options.output_fs = fopen(prog_options.output_filename,
		"w");
	if (prog_options.output_fs == NULL)
		{
		return -1;
		}
	
	}
return 0;
}

/*-----------------------------------------------------------------*/
int
proc_data(int *data)
{
unsigned int data_type;

#ifdef FUZZ_SIMULATOR
data[3] = abs(data[3]) % 0x19;
data[7] = (abs(data[7]) % 0x03) + 1;
#endif

data_type = data[3];

if ((data_type >= DATA_TYPE_MAX) ||
	(data_handlers[data_type].data_handler == NULL))
	{
	printf("No data handler for %2.2x\n", data_type);
	return 0;
	}

(*data_handlers[data_type].data_handler)(data);

return 0;
}

/*-----------------------------------------------------------------*/
int
generic_data_handler(int *data)
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
set_data_handlers()
{
int x;

/* default all handlers to generic */
for (x = 0; x < DATA_TYPE_MAX; x++)
	{
	data_handlers[x].data_handler = generic_data_handler;
	}

/* override generic */
data_handlers[DATA_TYPE_DATE].data_handler = dh_date;
data_handlers[DATA_TYPE_DATE].display_handler = display_date;

data_handlers[DATA_TYPE_TIME].data_handler = dh_time;
data_handlers[DATA_TYPE_TIME].display_handler = display_time;

data_handlers[DATA_TYPE_PRESSURE].data_handler = dh_pressure;
data_handlers[DATA_TYPE_PRESSURE].display_handler = display_pressure;

data_handlers[DATA_TYPE_RAIN].data_handler = dh_rain;
data_handlers[DATA_TYPE_RAIN].display_handler = display_rain;

data_handlers[DATA_TYPE_OUT_TEMP].data_handler = dh_outtemp;
data_handlers[DATA_TYPE_OUT_TEMP].display_handler = display_outtemp;

data_handlers[DATA_TYPE_IN_TEMP].data_handler = dh_intemp;
data_handlers[DATA_TYPE_IN_TEMP].display_handler = display_intemp;

data_handlers[DATA_TYPE_WIND_DIR].data_handler = dh_winddir;
data_handlers[DATA_TYPE_WIND_DIR].display_handler = display_winddir;

data_handlers[DATA_TYPE_WIND_1].data_handler = dh_wind1;
data_handlers[DATA_TYPE_WIND_1].display_handler = display_wind1;

data_handlers[DATA_TYPE_WIND_2].data_handler = dh_wind2;
data_handlers[DATA_TYPE_WIND_2].display_handler = display_wind2;

data_handlers[DATA_TYPE_HUMIDITY].data_handler = dh_humidity;
data_handlers[DATA_TYPE_HUMIDITY].display_handler = display_humidity;

return 0;
}

/*-----------------------------------------------------------------*/
int
wsd_connection_cb(int fd, int eventtypes, void *conn_data)
{
int data[NUM_DATA];
int ret;

ret = (*dev_options.ws_read)(fd, data);
if (ret < 0)
	{
	return 0;
	}

ret = proc_data(data);
if (ret != 0)
	{
	return 0;
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
struct select_node *ws_connection;

int fd;
int ret;

ret = set_prog_options(argc, argv);
if (ret)
	{
	fprintf(stderr, "Failed to set program options\n");
	return 1;
	}

ret = set_dev_options();
if (ret)
	{
	fprintf(stderr, "Failed to set device options\n");
	return 1;
	}

ret = set_data_handlers();

fd = (*dev_options.ws_open)(prog_options.device);
if (fd < 0)
	{
	fprintf(stderr, "Failed to open device\n");
	return 1;
	}

ret = (*dev_options.ws_start)(fd);
if (ret)
	{
	fprintf(stderr, "Failed to initialize device\n");
	return 1;
	}

wsd_init_selector();

/*-------------------*/

ws_connection = xmalloc(sizeof (struct select_node));

memset(ws_connection, 0, sizeof (struct select_node));

wsd_init_fd(ws_connection,			/* struct */
	fd,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	wsd_connection_cb,				/* callback */
	0								/* no broadcasts */
	);

wsd_add_fd(ws_connection);

/*-------------------*/

init_local_listener();

/*-------------------*/

while (1)
	{
#ifdef FUZZ_SIMULATOR
	sleep(1);
#endif

	wsd_selector();
	}

(*dev_options.ws_close)(fd);

return 0;
}
