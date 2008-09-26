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

#ifndef _BW953_H
#define _BW953_H

struct bw953_device_options
	{
	int (*ws_open)(char *);
	int (*ws_close)(int);

	int (*ws_start)(int);
	int (*ws_stop)(int);

	int (*ws_write)(int, unsigned char *, int);
	int (*ws_read)(int, int *);

	long usage_code;
	};

int set_bw953_dev_options();

#endif
