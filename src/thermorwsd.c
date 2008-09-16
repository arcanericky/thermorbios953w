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

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "thermorwsd.h"
#include "bw953.h"
#include "datahandlers.h"
#include "datadisplay.h"
#include "datadispcsv.h"

#include "list.h"
#include "select.h"
#include "common.h"
#include "servers.h"

struct datum_handler data_handlers[DATA_TYPE_MAX];

struct ws_prog_options prog_options;
struct bw953_device_options dev_options;

int usb_device_fd;

/*-----------------------------------------------------------------*/
static int
daemonize()
{
pid_t pid;

/* mutual exclusion */
/* FIXME: Make sure only one copy is running */

/* fork */
pid = fork();
if (pid < 0)
	{
	/* error forking */
	perror("fork");
	return 1;
	}

/* if parent process, then exit */
if (pid != 0)
	{
	exit(0);
	}

/* close descriptors */
/* FIXME: Code to close all but log file */

/* change directory */
/* FIXME: Change to a home directory if given on command line */

/* set umask */

/* detach from tty and set process group */
setsid();

return 0;
}

/*-----------------------------------------------------------------*/
static int
set_dev_options()
{
set_bw953_dev_options();

return 0;
}

/*-----------------------------------------------------------------*/
static int
set_prog_options(int argc, char *argv[])
{
int c;
int txt_count;
int option_index;
enum options
	{
	inside_temp_adj,
	outside_temp_adj,
	pressure_adj,

	device_name,
	log_filename,
	debug,

	date_text,
	date_suffix_text,

	time_text,
	time_suffix_text,

	inside_temp_text,
	inside_temp_suffix_text,

	outside_temp_text,
	outside_temp_suffix_text,

	humidity_text,
	humidity_suffix_text,

	pressure_text,
	pressure_suffix_text,

	rain_text,
	rain_suffix_text,

	wind_dir_text,
	wind_dir_suffix_text,

	wind_speed_text,
	wind_speed_suffix_text,

	wind_gust_text,
	wind_gust_suffix_text,

	wind_chill_text,
	wind_chill_suffix_text,

	forecast_text,
	trend_text,

	unknown1_text,
	unknown1_suffix_text,

	max_text,
	min_text,
	current_text,

	unix_path,

	record_data_file,
	play_data_file,

	foreground,
	fuzzy,
	playback_rate,

	data_csv,
	help
	};

static struct option long_options[] = {
	{ "inside-temp-adj",		required_argument, 0, inside_temp_adj },
	{ "outside-temp-adj",		required_argument, 0, outside_temp_adj },
	{ "pressure-adj",			required_argument, 0, pressure_adj },
	{ "device-name",			required_argument, 0, device_name },
	{ "log-filename",			required_argument, 0, log_filename },
	{ "debug",					required_argument, 0, debug },

	{ "inside-temp-text",		required_argument, 0, inside_temp_text },
	{ "inside-temp-suffix-text",
								required_argument, 0,
												inside_temp_suffix_text },
	{ "outside-temp-text",		required_argument, 0, outside_temp_text },
	{ "outside-temp-suffix-text",
								required_argument, 0,
												outside_temp_suffix_text },
	{ "humidity-text",			required_argument, 0, humidity_text },
	{ "humidity-suffix-text",	required_argument, 0,
												humidity_suffix_text },
	{ "pressure-text",			required_argument, 0, pressure_text },
	{ "pressure-suffix-text",	required_argument, 0,
												pressure_suffix_text },
	{ "rain-text",				required_argument, 0, rain_text },
	{ "rain-suffix-text",		required_argument, 0, rain_suffix_text },
	{ "wind-dir-text",			required_argument, 0, wind_dir_text },
	{ "wind-dir-suffix-text",	required_argument, 0,
												wind_dir_suffix_text },
	{ "wind-speed-text",		required_argument, 0, wind_speed_text },
	{ "wind-speed-suffix-text",	required_argument, 0,
												wind_speed_suffix_text },
	{ "wind-gust-text",			required_argument, 0, wind_gust_text },
	{ "wind-gust-suffix-text",	required_argument, 0,
												wind_gust_suffix_text },

	{ "wind-chill-text",			required_argument, 0, wind_chill_text },
	{ "wind-chill-suffix-text",		required_argument, 0,
												wind_chill_suffix_text },

	{ "unknown1-text",				required_argument, 0, unknown1_text },
	{ "unknown1-suffix-text",		required_argument, 0,
												unknown1_suffix_text },

	{ "forecast-text",			required_argument, 0, forecast_text },
	{ "trend-text",				required_argument, 0, trend_text },

	{ "date-text",				required_argument, 0, date_text },
	{ "date-suffix-text",		required_argument, 0, date_suffix_text },
	{ "time-text",				required_argument, 0, time_text },
	{ "time-suffix-text",		required_argument, 0, time_suffix_text },

	{ "max-text",				required_argument, 0, max_text },
	{ "min-text",				required_argument, 0, min_text },
	{ "current-text",			required_argument, 0, current_text },

	{ "unix-path",				required_argument, 0, unix_path },
	{ "record-data-file",		required_argument, 0, record_data_file },
	{ "play-data-file",			required_argument, 0, play_data_file },
	{ "foreground",				no_argument,       0, foreground },
	{ "fuzzy",					no_argument,       0, fuzzy },
	{ "playback-rate",			required_argument, 0, playback_rate },
	{ "data-csv",				no_argument,       0, data_csv },
	{ "help",					no_argument,       0, help },
	{ 0, 0, 0, 0 }
	};

char option_map[] = "iopdlv";

prog_options.debug_lvl = 3;

prog_options.output_filename = "/tmp/ws9xxd.log";
prog_options.output_fs = stdout;

prog_options.out_temp_adj = 0;
prog_options.in_temp_adj = 0;
prog_options.pressure_adj = 0;

prog_options.unix_path = UNIX_PATH;

prog_options.device = "/dev/hiddev0";

prog_options.foreground = 0;
prog_options.fuzzy = 0;
prog_options.playback_rate = 1;
prog_options.data_csv = 0;

prog_options.play_data_file = NULL;
prog_options.record_data_file = NULL;

/* null out all entries in options text */
for (txt_count = 0; txt_count < FLD_MAX_DEF_OPTION; txt_count++)
	{
	prog_options.output_txt[txt_count] = NULL;
	}

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
		case inside_temp_adj:
			prog_options.in_temp_adj = atoi(optarg);
			break;
		case 'o':
		case outside_temp_adj:
			prog_options.out_temp_adj = atoi(optarg);
			break;
		case 'p':
		case pressure_adj:
			prog_options.pressure_adj = atof(optarg) * 10;
			break;
		case 'd':
		case device_name:
			prog_options.device = optarg;
			break;
		case 'l':
		case log_filename:
			prog_options.output_filename = optarg;
			break;
		case 'v':
		case debug:
			prog_options.debug_lvl = atoi(optarg);
			break;
		case inside_temp_text:
			prog_options.output_txt[FLD_IN_TEMP] = optarg;
			break;
		case inside_temp_suffix_text:
			prog_options.output_txt[FLD_IN_TEMP_SUFFIX] = optarg;
			break;
		case outside_temp_text:
			prog_options.output_txt[FLD_OUT_TEMP] = optarg;
			break;
		case outside_temp_suffix_text:
			prog_options.output_txt[FLD_OUT_TEMP_SUFFIX] = optarg;
			break;
		case humidity_text:
			prog_options.output_txt[FLD_HUMIDITY] = optarg;
			break;
		case humidity_suffix_text:
			prog_options.output_txt[FLD_HUMIDITY_SUFFIX] = optarg;
			break;
		case pressure_text:
			prog_options.output_txt[FLD_PRESSURE] = optarg;
			break;
		case pressure_suffix_text:
			prog_options.output_txt[FLD_PRESSURE_SUFFIX] = optarg;
			break;
		case rain_text:
			prog_options.output_txt[FLD_RAIN] = optarg;
			break;
		case rain_suffix_text:
			prog_options.output_txt[FLD_RAIN_SUFFIX] = optarg;
			break;
		case wind_dir_text:
			prog_options.output_txt[FLD_WIND_DIR] = optarg;
			break;
		case wind_dir_suffix_text:
			prog_options.output_txt[FLD_WIND_DIR_SUFFIX] = optarg;
			break;
		case wind_speed_text:
			prog_options.output_txt[FLD_WIND_SPEED] = optarg;
			break;
		case wind_speed_suffix_text:
			prog_options.output_txt[FLD_WIND_SPEED_SUFFIX] = optarg;
			break;
		case wind_gust_text:
			prog_options.output_txt[FLD_WIND_GUST] = optarg;
			break;
		case wind_gust_suffix_text:
			prog_options.output_txt[FLD_WIND_GUST_SUFFIX] = optarg;
			break;
		case wind_chill_text:
			prog_options.output_txt[FLD_WIND_CHILL] = optarg;
			break;
		case wind_chill_suffix_text:
			prog_options.output_txt[FLD_WIND_CHILL_SUFFIX] = optarg;
			break;
		case unknown1_text:
			prog_options.output_txt[FLD_UNKNOWN1] = optarg;
			break;
		case unknown1_suffix_text:
			prog_options.output_txt[FLD_UNKNOWN1_SUFFIX] = optarg;
			break;
		case forecast_text:
			prog_options.output_txt[FLD_FORECAST] = optarg;
			break;
		case trend_text:
			prog_options.output_txt[FLD_TREND] = optarg;
			break;
		case date_text:
			prog_options.output_txt[FLD_DATE] = optarg;
			break;
		case date_suffix_text:
			prog_options.output_txt[FLD_DATE_SUFFIX] = optarg;
			break;
		case time_text:
			prog_options.output_txt[FLD_TIME] = optarg;
			break;
		case time_suffix_text:
			prog_options.output_txt[FLD_TIME_SUFFIX] = optarg;
			break;
		case max_text:
			prog_options.output_txt[FLD_MAX] = optarg;
			break;
		case min_text:
			prog_options.output_txt[FLD_MIN] = optarg;
			break;
		case current_text:
			prog_options.output_txt[FLD_CUR] = optarg;
			break;
		case unix_path:
			prog_options.unix_path = optarg;
			break;
		case foreground:
			prog_options.foreground = 1;
			prog_options.output_filename = "";
			break;
		case fuzzy:
			prog_options.fuzzy = 1;
			break;
		case playback_rate:
			prog_options.playback_rate = atoi(optarg);
			break;
		case record_data_file:
			prog_options.record_data_file = optarg;
			break;
		case play_data_file:
			prog_options.play_data_file = optarg;
			break;
		case data_csv:
			prog_options.data_csv = 1;
			prog_options.output_txt[FLD_DATA_PREFIX] = "";
			prog_options.output_txt[FLD_DATA_SEPARATOR] = ",";
			prog_options.output_txt[FLD_UNIT_SEPARATOR] = ",";
			prog_options.output_txt[FLD_NO_READING] = "";
			break;
		case '?':
			fprintf(stderr, "Use --help for more information.\n");
			return -1;
			break;
		case help:
printf("ws9xxd - Weather Station Daemon for Bios/Thermor 9xx Series\n");
printf("Usage:\n");
printf("\tws9xxd [options]\n");
printf("\n");
printf("Options:\n");
printf("\t--device-name\n");
printf("\t\tWeather station device.\n");
printf("\t\tDefault: %s\n", prog_options.device);
printf("\t\t\tUbuntu could be  : /dev/usb/hiddev0\n");
printf("\t\t\tMandriva could be: /dev/hiddev0\n");
printf("\t--unix-path\n");
printf("\t\tName for Unix domain socket.\n");
printf("\t\tDefault: %s\n", prog_options.unix_path);
printf("\t--log-filename\n");
printf("\n");
printf("Reading Adjustment Options:\n");
printf("\t--inside-temp-adj\n");
printf("\t--outside-temp-adj\n");
printf("\t--pressure-adj\n");
printf("\n");
printf("Text Output Options:\n");
printf("\t--date-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_DATE]);
printf("\t--date-suffix-text\n");
printf("\t--time-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_TIME]);
printf("\t--time-suffix-text\n");
printf("\t--inside-temp-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_IN_TEMP]);
printf("\t--inside-temp-suffix-text\n");
printf("\t--outside-temp-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_OUT_TEMP]);
printf("\t--outside-temp-suffix-text\n");
printf("\t--humidity-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_HUMIDITY]);
printf("\t--humidity-suffix-text\n");
printf("\t--pressure-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_PRESSURE]);
printf("\t--pressure-suffix-text\n");
printf("\t--rain-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_RAIN]);
printf("\t--rain-suffix-text\n");
printf("\t--wind-dir-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_WIND_DIR]);
printf("\t--wind-dir-suffix-text\n");
printf("\t--wind-speed-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_WIND_SPEED]);
printf("\t--wind-speed-suffix-text\n");
printf("\t--wind-gust-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_WIND_GUST]);
printf("\t--wind-gust-suffix-text\n");
printf("\t--wind-chill-text\n");
printf("\t--wind-chill-suffix-text\n");
printf("\t--forecast-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_FORECAST]);
printf("\t--trend-text\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_TREND]);
printf("\t--current-text\n");
printf("\t\tText to display for current reading.\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_CUR]);
printf("\t--max-text\n");
printf("\t\tText to display for maximum reading.\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_MAX]);
printf("\t--min-text\n");
printf("\t\tText to display for minimum reading.\n");
printf("\t\tDefault: %s\n", prog_options.default_txt[FLD_MIN]);
printf("\n");

printf("Debugging Options:\n");
printf("\t--record-data-file\n");
printf("\t--play-data-file\n");
printf("\t--debug\n");
printf("\t--foreground\n");
printf("\t\tRun program in foreground (don't run as a server) for testing\n");
printf("\t\tAlso sets --log-filename to the screen unless overridden with\n");
printf("\t\tsubsequent option\n");
printf("\t--fuzzy\n");
printf("\t\tGenerate weather station data using random number generator.\n");
printf("\t\tCauses strange data, but useful for testing.\n");
printf("\t--playback-rate\n");
printf("\t\tRate at which to play back weather station events.\n");
printf("\t\tDefault: 1 second.\n");
			return -1;
			break;
		}
	}

/* set default text strings */
if (prog_options.data_csv)
	{
	set_csv_default_text();
	}
else
	{
	set_pp_default_text();
	}

/* process user's command line text options */
for (txt_count = 0; txt_count < FLD_MAX_DEF_OPTION; txt_count++)
	{
	if (prog_options.output_txt[txt_count] == NULL)
		{
		prog_options.output_txt[txt_count] =
			prog_options.default_txt[txt_count];
		}
	}

if (prog_options.foreground && strlen(prog_options.output_filename) == 0)
	{
	printf("Running in foreground, logging to screen.\n");
	prog_options.output_fs = stdout;
	}
else
	{
	/* Program running in background - log to file */
	prog_options.output_fs = fopen(prog_options.output_filename,
		"w");
	if (prog_options.output_fs == NULL)
		{
		perror("fopen");
		fprintf(stderr, "Could not open log file %s\n",
			prog_options.output_filename);
		return -1;
		}
	
	}
return 0;
}

/*-----------------------------------------------------------------*/
static int
proc_data(int *data)
{
unsigned int data_type;

/* This is the only place where --fuzzy and --play-data-file are */
/* different and --fuzzy needs to be checked separately */
if (prog_options.fuzzy)
	{
	data[3] = abs(data[3]) % (DATA_TYPE_MAX - 1);
	data[7] = (abs(data[7]) % 0x03) + 1;
	}

data_type = data[3];

if ((data_type >= DATA_TYPE_MAX) ||
	(data_handlers[data_type].data_handler == NULL))
	{
	fprintf(prog_options.output_fs, "No data handler for %2.2x\n", data_type);
	return 0;
	}

(*data_handlers[data_type].data_handler)(data);

return 0;
}

/*-----------------------------------------------------------------*/
static int
generic_data_handler(int *data)
{
int x;

if (prog_options.fuzzy)
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
static int
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
data_handlers[DATA_TYPE_TIME].data_handler = dh_time;
data_handlers[DATA_TYPE_PRESSURE].data_handler = dh_pressure;
data_handlers[DATA_TYPE_RAIN].data_handler = dh_rain;
data_handlers[DATA_TYPE_OUT_TEMP].data_handler = dh_outtemp;
data_handlers[DATA_TYPE_IN_TEMP].data_handler = dh_intemp;
data_handlers[DATA_TYPE_WIND_DIR].data_handler = dh_winddir;
data_handlers[DATA_TYPE_WIND_SPEED].data_handler = dh_windspeed;
data_handlers[DATA_TYPE_WIND_GUST].data_handler = dh_windgust;
data_handlers[DATA_TYPE_HUMIDITY].data_handler = dh_humidity;
data_handlers[DATA_TYPE_FORECAST].data_handler = dh_forecast;
data_handlers[DATA_TYPE_TREND].data_handler = dh_trend;
data_handlers[DATA_TYPE_WIND_CHILL].data_handler = dh_windchill;
data_handlers[DATA_TYPE_UNKNOWN1].data_handler = dh_unknown1;

return 0;
}

/*-----------------------------------------------------------------*/
static int
set_data_displayers()
{
if (prog_options.data_csv)
	{
	set_csv_data_displayers();
	}
else
	{
	set_pp_data_displayers();
	}

return 0;
}

/*-----------------------------------------------------------------*/
static int
wsd_connection_cb(int fd, int eventtypes, void *conn_data)
{
int data[NUM_DATA];
int ret;

ret = (*dev_options.ws_read)(fd, data);
if (ret < 0)
	{
	return 1;
	}

ret = proc_data(data);
if (ret != 0)
	{
	return 0;
	}

return 0;
}

/*-----------------------------------------------------------------*/
static void
sigexit(int signal)
{

/* FIXME: Code needs cleanup here.  Some of the shutdown functions are
 * correct, but could be restructured, and more need to be called,
 * such as closing all the client connections
 */

(*dev_options.ws_close)(usb_device_fd);

close_local_listener();
unlink(prog_options.unix_path);

fclose(prog_options.output_fs);

exit(0);
}

/*-----------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
struct select_node *ws_connection;
int ret;

ret = set_prog_options(argc, argv);
if (ret)
	{
	return 1;
	}

ret = set_dev_options();
if (ret)
	{
	fprintf(prog_options.output_fs, "Failed to set device options\n");
	return 1;
	}

ret = set_data_handlers();
ret = set_data_displayers();

if (prog_options.foreground == 0)
	{
	if (daemonize())
		{
		return 1;
		}
	}

if (prog_options.fuzzy)
	{
	prog_options.play_data_file = "/dev/urandom";
	}

if (prog_options.play_data_file)
	{
	prog_options.device = prog_options.play_data_file;
	}

usb_device_fd = (*dev_options.ws_open)(prog_options.device);
if (usb_device_fd < 0)
	{
	fprintf(prog_options.output_fs, "Failed to open device %s\n",
		prog_options.device);
	fprintf(prog_options.output_fs,
		"Check for file existence and permissions.\n");
	return 1;
	}

ret = (*dev_options.ws_start)(usb_device_fd);
if (ret)
	{
	fprintf(prog_options.output_fs, "Failed to initialize device\n");
	return 1;
	}

wsd_init_selector();

/*-------------------*/

signal(SIGINT, sigexit);
signal(SIGTERM, sigexit);

/*-------------------*/

ws_connection = xmalloc(sizeof (struct select_node));

memset(ws_connection, 0, sizeof (struct select_node));

wsd_init_fd(ws_connection,			/* struct */
	usb_device_fd,					/* file descriptor */
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
	if (prog_options.play_data_file != NULL)
		{
		sleep(prog_options.playback_rate);
		}

	ret = wsd_selector();
	if (ret != 0)
		{
		break;
		}
	}

(*dev_options.ws_close)(usb_device_fd);

return 0;
}
