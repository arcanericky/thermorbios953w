#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if 0
#define FUZZ_SIMULATOR 1
#endif

#include <asm/types.h>
#include <linux/hiddev.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define NUM_DEVICE_EVENTS	8

int
read_data(int fd, int *dest)
{
int ret;
int x;
struct hiddev_event event[NUM_DEVICE_EVENTS];

#ifdef FUZZ_SIMULATOR
ret = 0;
#endif

ret = read(fd, &event, sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS);
if (ret == -1)
	{
	perror("read");
	exit(EXIT_FAILURE);
	}

if (ret != sizeof(struct hiddev_event) * NUM_DEVICE_EVENTS)
	{
	fprintf(stderr, "Short read: %d\n", ret);	
	exit(EXIT_FAILURE);
	}

for (x = 0; x != (ret / sizeof(struct hiddev_event)); x++)
	{
#if HOST_IS_BIGENDIAN
	*dest = (unsigned char) (event[x].value >> 24);
#else
	*dest = (unsigned char) event[x].value;
#endif
	dest++;
	}

return ret;
}

int
write_data(int fd, unsigned char *output, int len)
{
struct hiddev_usage_ref_multi uref;
struct hiddev_report_info rinfo;
int ret;

memset(&uref, 0, sizeof(uref));
memset(&rinfo, 0, sizeof(rinfo));

uref.uref.report_id = HID_REPORT_ID_UNKNOWN;

uref.uref.report_type = HID_REPORT_TYPE_OUTPUT;
uref.uref.field_index = 0;

uref.uref.usage_code = 0xFFA10004;
uref.uref.usage_index = 0;
uref.num_values = len;

for (ret = 0; ret < len; ret++)
	{
	uref.values[ret] = *(output + ret);
	}

#ifndef FUZZ_SIMULATOR
ret = ioctl(fd, HIDIOCSUSAGES, &uref);
if (ret == -1)
	{
	perror("ioctl");
	exit(EXIT_FAILURE);
	}
#else
ret = 0;
#endif

rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
rinfo.report_id = 0x00;

rinfo.num_fields = 1;

#ifndef FUZZ_SIMULATOR
ret = ioctl(fd, HIDIOCSREPORT, &rinfo);
if (ret == -1)
	{
	perror("ioctl");
	exit(EXIT_FAILURE);
	}
#endif

return 0;
}

int
main()
{
int fd;
int x;
char *rtype;
int data[NUM_DEVICE_EVENTS];

unsigned char start_msgs[][16] = {
		{
		0x00, 0x01, 0x17, 0x02,  0xff, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
		}
	};

#ifdef FUZZ_SIMULATOR
char *filename = "/dev/urandom";
#else
char *filename = "/dev/usb/hiddev0";
#endif

fd = open(filename, O_RDWR);
if (fd == -1)
	{
	perror("open");
	return EXIT_FAILURE;
	}

write_data(fd, start_msgs[0], 16);
read_data(fd, data);
sleep(1);

while (1)
	{
	read_data(fd, data);

#ifdef FUZZ_SIMULATOR
	data[7] = (data[7] % 0x03) + 1;
	data[3] = (data[3] % 0x1d) + 1;
#endif

	switch (data[7])
		{
		case 0x01:	/* Minimum */
			rtype = "Minimum";
			break;
		case 0x02:	/* Maximum */
			rtype = "Maximum";
			break;
		case 0x03:	/* Current */
			rtype = "Current";
			break;
		default:
			rtype = "";
			break;
		}

	switch (data[3])
		{
		case 0x02:	/* Date */
			printf("Date: %2.2d/%2.2d/%2.2d\n", data[5], data[6], data[4]);
			break;
		case 0x03:	/* Time */
			printf("Time: %2.2d:%2.2d\n", data[4], data[5]);
			break;
		case 0x10:	/* Pressure */
			x = (data[5] << 8) + data[6];
			printf("%s Pressure: %d.%d mb\n", rtype, x / 10, abs(x % 10));
			break;
		case 0x11:	/* Rain */
			x = (data[5] << 8) + data[6];
			x = x * 5;
			printf("%s Rain: ", rtype);

			if ((x | 0xFF00) == x)
				printf("No Reading\n");
			else
				printf("%d.%d ml\n", x / 10, abs(x % 10));
			break;
		case 0x12:	/* Wind Chill */
			break;
		case 0x13:	/* Outside Temperature */
			x = (data[5] << 8) + data[6];
			x = x - 400;
			printf("%s Outside Temperature: ", rtype);

			if ((x | 0xFF00) == x)
				printf("No Reading\n");
			else
				printf("%s%d.%d", x < 0 ? "-" : "", abs(x / 10),
					abs(x % 10));
			break;
		case 0x14:	/* Inside Temperature */
			x = (data[5] << 8) + data[6];
			printf("%s Inside Temperature: ", rtype);
			if ((x | 0xFF00) == x)
				printf("No Reading\n");
			else
				printf("%s%d.%d", x < 0 ? "-" : "", abs(x / 10),
					abs(x % 10));
			break;
		case 0x15:	/* Wind Directon */
			x = data[5];
			printf("Wind Direction: ");
			switch (x)
				{
				case 0x0: printf("157"); break;
				case 0x10: printf("No Reading\n"); break;
				}

			if (x != 0x10)
				{
				printf(" degrees\n");
				}
			break;
		case 0x16:	/* Wind Speed */
			x = (data[5] << 8) + data[6];
			printf("%s Wind Speed: %d\n", rtype, x);
			break;
		case 0x17:	/* Wind Gust */
			x = (data[5] << 8) + data[6];
			printf("%s Wind Gust: %d\n", rtype, x);
			break;
		case 0x18:	/* Humidity */
			x = data[5];
			printf("%s Humidity: %d %%\n", rtype, x);
			break;
		case 0x19:	/* Forecast */
			break;
		case 0x1a:	/* Trend */
			break;
		case 0x1b:	/* Unknown */
			break;
		}

	fflush(stdout);
	}

return EXIT_SUCCESS;
}
