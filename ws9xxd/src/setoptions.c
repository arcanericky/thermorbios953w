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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "setoptions.h"
#include "configfile.h"
#include "bw953.h"
#include "thermorwsd.h"
#include "datadispcsv.h"
#include "datadisplay.h"

struct ws_prog_options prog_options;
struct bw953_device_options dev_options;

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

	output_type,
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
	{ "output-type",			required_argument, 0, output_type },
	{ "help",					no_argument,       0, help },
	{ 0, 0, 0, 0 }
	};

static int set_option_defaults();
static int print_help();
static int set_option_configfile();

/*-----------------------------------------------------------------*/
int
set_dev_options()
{
set_bw953_dev_options();

return 0;
}

/*-----------------------------------------------------------------*/
static int
set_option_configfile()
{
configOpen(BW9XX_CFG_DIR "/ws9xxd.conf");

return 0;
}

/*-----------------------------------------------------------------*/
static int
print_help()
{
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
printf("\t--output-type\n");
printf("\t\tOptions are:\n");
printf("\t\t\t\"csv\": Comma Seperated Values (default)\n");
printf("\t\t\t\"pp\": Pretty Print (human readable)\n");
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

return 0;
}

/*-----------------------------------------------------------------*/
static int
set_option_defaults()
{
int txt_count;

prog_options.debug_lvl = 3;

prog_options.output_filename = BW9XX_CFG_DIR "/ws9xxd.log";
prog_options.output_fs = stdout;

prog_options.out_temp_adj = 0;
prog_options.in_temp_adj = 0;
prog_options.pressure_adj = 0;

prog_options.unix_path = UNIX_PATH;

prog_options.device = "/dev/hiddev0";

prog_options.foreground = 0;
prog_options.fuzzy = 0;
prog_options.playback_rate = 1;
prog_options.output_type = OUTPUT_CSV;

prog_options.play_data_file = NULL;
prog_options.record_data_file = NULL;

/* null out all entries in options text */
for (txt_count = 0; txt_count < FLD_MAX_DEF_OPTION; txt_count++)
	{
	prog_options.output_txt[txt_count] = NULL;
	}

return 0;
}

/*-----------------------------------------------------------------*/
static int
get_config_file_option()
{
static int x = 0;
int val;

optarg = NULL;
val = -1;

/* loop until an option is found or until the end of */
/* the option list is reached */
while ((optarg == NULL) && (long_options[x].name != NULL))
	{
	optarg = configGetValue(long_options[x].name);
	val = long_options[x].val;

	x++;
	}

/* if end is reached, return a value that signals this */
if (long_options[x].name == NULL)
	{
	val = -1;
	}

return val;
}

/*-----------------------------------------------------------------*/
int
set_prog_options(int argc, char *argv[])
{
#define CONVERT_ADJUSTMENT \
	(atof(optarg) + ((atof(optarg) < 0) ? -0.05 : 0.05)) * 10

int c;
int x;
int option_index;
int txt_count;

set_option_defaults();

set_option_configfile();

/* set options from command line */
for (x = 0; x < 2; x++)
	{
	while (1)
		{
		if (x == 0)
			{
			/* options from config file first */
			c = get_config_file_option();
			}
		else
			{
			/* options from command line second */
			/* (to override the config file) */
			c = getopt_long(argc, argv, "i:o:p:d:l:v:",
				long_options, &option_index);
			}

		if (c == -1)
			{
			break;
			}

		switch (c)
			{
			case inside_temp_adj:
				prog_options.in_temp_adj = CONVERT_ADJUSTMENT;
				break;
			case outside_temp_adj:
				prog_options.out_temp_adj = CONVERT_ADJUSTMENT;
				break;
			case pressure_adj:
				prog_options.pressure_adj = CONVERT_ADJUSTMENT;
				break;
			case device_name:
				prog_options.device = optarg;
				break;
			case log_filename:
				prog_options.output_filename = optarg;
				break;
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
				printf("At foreground\n");
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
			case output_type:
				if ((strcmp(optarg, "pp") == 0) ||
					(strcmp(optarg, "prettyprint") == 0) ||
					(strcmp(optarg, "pretty-print") == 0))
					{
					prog_options.output_type = OUTPUT_PP;
					}
				else if (strcmp(optarg, "csv") == 0)
					{
					prog_options.output_type = OUTPUT_CSV;
					}
				break;
			case '?':
				fprintf(stderr, "Use --help for more information.\n");
				return -1;
				break;
			case help:
				print_help();
				return -1;
				break;
			default:
				break;
			} /* switch */
		} /* while */
	} /* for */

/* text options: set default */
if (prog_options.output_type == OUTPUT_CSV)
	{
	set_csv_default_text();
	}
else
	{
	set_pp_default_text();
	}

/* text options: load from config file / command line */
for (txt_count = 0; txt_count < FLD_MAX_DEF_OPTION; txt_count++)
	{
	if (prog_options.output_txt[txt_count] == NULL)
		{
		prog_options.output_txt[txt_count] =
			prog_options.default_txt[txt_count];
		}
	}

/* set options depending on foreground and background */
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

