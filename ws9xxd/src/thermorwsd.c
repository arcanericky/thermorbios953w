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

#include "thermorwsd.h"
#include "bw953.h"
#include "datahandlers.h"

#include "datadisplay.h"
#include "datadispcsv.h"

#include "select.h"
#include "common.h"
#include "servers.h"
#include "setoptions.h"
#include "configfile.h"

struct datum_handler data_handlers[DATA_TYPE_MAX];

extern struct ws_prog_options prog_options;
extern struct bw953_device_options dev_options;

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
proc_data(int *data)
{
#if 0
unsigned int data_type;
#endif
unsigned char data_type;

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
	fprintf(prog_options.output_fs, "No data handler for 0x%2.2x\n", data_type);
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
if (prog_options.output_type == OUTPUT_CSV)
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

(*dev_options.ws_stop)(usb_device_fd);
(*dev_options.ws_close)(usb_device_fd);

close_local_listener();
unlink(prog_options.unix_path);

configClose();

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
