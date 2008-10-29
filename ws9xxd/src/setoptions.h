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

#ifndef _SETOPTIONS_H
#define _SETOPTIONS_H

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

enum DEF_OPTIONS_TXT
	{
	FLD_MAX,
	FLD_MIN,
	FLD_CUR,

	FLD_DATA_PREFIX,
	FLD_DATA_SEPARATOR,
	FLD_UNIT_SEPARATOR,
	FLD_RANGE_SEPARATOR,
	FLD_NO_READING,

	FLD_TIME,
	FLD_TIME_SUFFIX,

	FLD_DATE,
	FLD_DATE_SUFFIX,

	FLD_IN_TEMP,
	FLD_IN_TEMP_SUFFIX,

	FLD_OUT_TEMP,
	FLD_OUT_TEMP_SUFFIX,

	FLD_RAIN,
	FLD_RAIN_SUFFIX,

	FLD_HUMIDITY,
	FLD_HUMIDITY_SUFFIX,

	FLD_PRESSURE,
	FLD_PRESSURE_SUFFIX,

	FLD_WIND_DIR,
	FLD_WIND_DIR_SUFFIX,

	FLD_WIND_SPEED,
	FLD_WIND_SPEED_SUFFIX,

	FLD_WIND_GUST,
	FLD_WIND_GUST_SUFFIX,

	FLD_FORECAST,
	FLD_FORECAST_SUFFIX,

	FLD_TREND,
	FLD_TREND_SUFFIX,

	FLD_WIND_CHILL,
	FLD_WIND_CHILL_SUFFIX,

	FLD_UNKNOWN1,
	FLD_UNKNOWN1_SUFFIX,

	FLD_MAX_DEF_OPTION
	};

enum OUTPUT_TYPES
	{
	OUTPUT_CSV,
	OUTPUT_PP
	};

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

	int output_type;

	int out_temp_adj;
	int in_temp_adj;
	int pressure_adj;
	
	int rain_clicks_per_inch;

	char *output_txt[FLD_MAX_DEF_OPTION];
	char *default_txt[FLD_MAX_DEF_OPTION];
	};

int set_dev_options();
int set_prog_options(int, char *[]);

#endif
