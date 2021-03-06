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

#include <asm/types.h>
#include <linux/hiddev.h>
#include <linux/input.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "thermorwsd.h"
#include "setoptions.h"
#include "bw953.h"
#include "common.h"

#define NUM_DEVICE_EVENTS		8

extern struct ws_prog_options prog_options;
extern struct bw953_device_options dev_options;

/*-----------------------------------------------------------------*/
int
bw953_open(char *filename)
{
int fd;

if (prog_options.fuzzy == 0 && prog_options.play_data_file == NULL)
	{
	fd = open(filename, O_RDWR);
	}
	{
	fd = open(filename, O_RDONLY);
	}

if (fd == -1)
	{
	perror("open");
	return fd;
	}

return fd;
}

/*-----------------------------------------------------------------*/
int
bw953_close(int fd)
{
int ret;

ret = close(fd);

return ret;
}

/*-----------------------------------------------------------------*/
int
bw953_start(int fd)
{
#define START_MSG_LEN 16
unsigned char start_msgs[][START_MSG_LEN] = {
#if 0
	{
	0x00, 0x01, 0x11, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
	},
	{
	0x00, 0x01, 0x17, 0x01,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
	},
#endif
	{
	0x00, 0x01, 0x17, 0x02,  0xff, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
	}
	};
int data[NUM_DATA];
int x;
int ret;

for (x = 0; x < sizeof(start_msgs) / START_MSG_LEN; x++)
	{
	ret = (*dev_options.ws_write)(fd, start_msgs[x], START_MSG_LEN);
	if (ret)
		{
		return 1;
		}

	ret = (*dev_options.ws_read)(fd, data);
	if (ret == -1)
		{
		return 1;
		}

	if (prog_options.fuzzy == 0 && prog_options.play_data_file == NULL)
		{
		sleep(1);
		}
	}

return 0;
#undef START_MSG_LEN
}

/*-----------------------------------------------------------------*/
int
bw953_stop(int fd)
{
#define STOP_MSG_LEN 16
unsigned char stop_msg1[] = {
	0x00, 0x01, 0x16, 0x03,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
	};

(*dev_options.ws_write)(fd, stop_msg1, STOP_MSG_LEN);
sleep(1);

#undef STOP_MSG_LEN

return 0;
}

/*-----------------------------------------------------------------*/
int
bw953_read(int fd, int *dest)
{
int ret;
int x;
struct hiddev_event event[NUM_DEVICE_EVENTS];
int chars_received = 0;

while (chars_received < sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS)
{
	ret = read(fd, (char*)(&event)+chars_received,
		sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS - chars_received);

	if (ret == -1)
	{
		perror("read");
		return ret;
	}

	if (ret != sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS)
		{
		fprintf(prog_options.output_fs, "Short read: %d %d\n", ret,
			sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS);
		}

	chars_received += ret;
}

datadump("REC", &event, chars_received);

if (prog_options.record_data_file != NULL)
	{
	int rec_fd;

	rec_fd = open(prog_options.record_data_file,
		O_RDWR | O_APPEND | O_CREAT,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (rec_fd != -1)
		{
		write(rec_fd, &event, chars_received);
		close(rec_fd);
		}
	}

if (chars_received != sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS)
	{
	fprintf(prog_options.output_fs, "Short read FAIL: %d %d\n", ret,
		sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS);
	return -1;
	}

for (x = 0; x != (chars_received / sizeof(struct hiddev_event)); x++)
	{
#if HOST_IS_BIGENDIAN
	*dest = (unsigned char) (event[x].value >> 24);
#else
	*dest = (unsigned char) event[x].value;
#endif
	dest++;
	}

return chars_received;
}

/*-----------------------------------------------------------------*/
int
bw953_write(int fd, unsigned char *output, int len)
{
struct hiddev_usage_ref_multi uref;
struct hiddev_report_info rinfo;
int ret;
int i;

#if 0
printf("Writing HIDIOCSUSAGES to fd %d, for length %d, usage code %8.8X\n",
	fd, len, dev_options.usage_code);
#endif

memset(&uref, 0, sizeof(uref));
memset(&rinfo, 0, sizeof(rinfo));

/* FIXME: What to use for report id, one example (mouse uses 0x10),  */
/* another uses 0x01 - only 255 to test! */
uref.uref.report_id = HID_REPORT_ID_UNKNOWN;

uref.uref.report_type = HID_REPORT_TYPE_OUTPUT;
uref.uref.field_index = 0;

uref.uref.usage_code = dev_options.usage_code;
uref.uref.usage_index = 0;
uref.num_values = len;

for (i = 0; i < len; i++)
	{
	uref.values[i] = *(output + i);
	}

if (prog_options.fuzzy == 0 && prog_options.play_data_file == NULL)
	{
	ret = ioctl(fd, HIDIOCSUSAGES, &uref);
	if (ret == -1)
		{
		perror("ioctl HIDIOCSUSAGES");
		return ret;
		}
	}

rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
rinfo.report_id = 0x00;

rinfo.num_fields = 1;

if (prog_options.fuzzy == 0 && prog_options.play_data_file == NULL)
	{
	ret = ioctl(fd, HIDIOCSREPORT, &rinfo);
	if (ret == -1)
		{
		perror("ioctl HIDIOCSREPORT");
		return ret;
		}
	}

return 0;
}

/*-----------------------------------------------------------------*/
int
bw953_info(int fd)
{
struct hiddev_devinfo device_info;
int ret;

ret = ioctl(fd, HIDIOCGDEVINFO, &device_info);
if (ret == -1)
	{
	perror("ioctl");
	return ret;
	}

fprintf(prog_options.output_fs,
	"Vendor : %4.4X\n", device_info.vendor);
fprintf(prog_options.output_fs,
	"Product: %4.4X\n", device_info.product);
fprintf(prog_options.output_fs,
	"Apps   : %d\n", device_info.num_applications);

return 0;
}

/*-----------------------------------------------------------------*/
int
set_bw953_dev_options()
{
/* function pointers */
dev_options.ws_open = bw953_open;
dev_options.ws_close = bw953_close;
dev_options.ws_start = bw953_start;
dev_options.ws_stop = bw953_stop;
dev_options.ws_write = bw953_write;
dev_options.ws_read = bw953_read;

/* device codes */
dev_options.usage_code = 0xFFA10004;

return 0;
}
