#ifndef _THERMOSWSD_H
#define _THERMOSWSD_H

#include <stdio.h>

#if 0
#define FUZZ_SIMULATOR			1
#endif

#define RAW_DATA			1

#define NUM_DATA				8

#define UNIX_PATH	"/tmp/wsd"

/* Data Types */
#define DATA_TYPE_DATE			0x02
#define DATA_TYPE_TIME			0x03
#define DATA_TYPE_PRESSURE		0x10
#define DATA_TYPE_RAIN			0x11
#define DATA_TYPE_OUT_TEMP		0x13
#define DATA_TYPE_IN_TEMP		0x14
#define DATA_TYPE_WIND_DIR		0x15
#define DATA_TYPE_WIND_1		0x16
#define DATA_TYPE_WIND_2		0x17
#define DATA_TYPE_HUMIDITY		0x18
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

	char *output_filename;
	FILE *output_fs;

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
	char *date_txt;
	char *in_temp_txt;
	char *out_temp_txt;
	char *rain_txt;
	char *humidity_txt;
	char *pressure_txt;
	char *wind_dir_txt;
	char *wind_speed_txt;

	char *no_reading_txt;
	};

struct datum_handler
	{
	int (*data_handler)(int *);
	int (*display_handler)();
	};

#endif
