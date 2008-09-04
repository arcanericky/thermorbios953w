#ifndef _THERMOSWSD_H
#define _THERMOSWSD_H

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#define RAW_DATA			1

#define NUM_DATA			8

#define UNIX_PATH	"/tmp/wsd"

/* Data Types */
#define DATA_TYPE_DATE			0x02
#define DATA_TYPE_TIME			0x03
#define DATA_TYPE_PRESSURE		0x10
#define DATA_TYPE_RAIN			0x11
#define DATA_TYPE_WIND_CHILL	0x12	/* not used yet */
#define DATA_TYPE_OUT_TEMP		0x13
#define DATA_TYPE_IN_TEMP		0x14
#define DATA_TYPE_WIND_DIR		0x15
#define DATA_TYPE_WIND_SPEED	0x16
#define DATA_TYPE_WIND_GUST		0x17
#define DATA_TYPE_HUMIDITY		0x18
#define DATA_TYPE_FORECAST		0x19
#define DATA_TYPE_TREND			0x1a
#define DATA_TYPE_UNKNOWN1		0x1b
#define DATA_TYPE_MAX			0x1c

/* Data Categories */
#define DATA_MIN				0x01
#define DATA_MAX				0x02
#define DATA_CURRENT			0x03

struct ws_prog_options
	{
	char *vendor;
	char *product;

	char *device;
	char *logfile;

	char *unix_path;

	int foreground;
	int fuzzy;
	int playback_rate;

	char *output_filename;
	FILE *output_fs;

	char *record_data_file;
	char *play_data_file;

	int debug_lvl;

	int out_temp_adj;
	int in_temp_adj;
	int pressure_adj;
	
	int rain_clicks_per_inch;

	/* text output */
	char *max_txt;
	char *min_txt;
	char *current_txt;

	char *data_prefix;

	char *time_txt;
	char *time_suffix_txt;
	char *date_txt;
	char *date_suffix_txt;
	char *in_temp_txt;
	char *in_temp_suffix_txt;
	char *out_temp_txt;
	char *out_temp_suffix_txt;
	char *rain_txt;
	char *rain_suffix_txt;
	char *humidity_txt;
	char *humidity_suffix_txt;
	char *pressure_txt;
	char *pressure_suffix_txt;

	char *wind_dir_txt;
	char *wind_dir_suffix_txt;

	char *wind_speed_txt;
	char *wind_speed_suffix_txt;

	char *wind_gust_txt;
	char *wind_gust_suffix_txt;

	char *forecast_txt;
	char *trend_txt;

	char *wind_chill_txt;

	char *no_reading_txt;
	};

struct datum_handler
	{
	int (*data_handler)(int *);
	int (*display_handler)();
	};

#endif
