#ifndef _BW953_H
#define _BW953_H

struct bw953_device_options
	{
	int (*ws_open)(char *);
	int (*ws_close)(int);

	int (*ws_start)(int);
	int (*ws_stop)(int);

	int (*ws_write)(int, char *, int);
	int (*ws_read)(int, int *);

	long usage_code;
	};

int set_bw953_dev_options();

#endif
