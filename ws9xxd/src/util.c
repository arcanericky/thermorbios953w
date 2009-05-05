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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include "thermorwsd.h"
#include "common.h"
#include "debug.h"

/*-----------------------------------------------------------------*/
void *
xmalloc(size_t size)
{
void *mem;

mem = malloc(size);
if (mem == NULL)
	{
	perror("malloc");
	exit (EXIT_FAILURE);
	}

return mem;
}

/*-----------------------------------------------------------------*/
void
xfree(void *mem)
{
free(mem);
mem = NULL;
}

/*-----------------------------------------------------------------*/
int
get_time(char *dest)
{
struct tm *ltime;
time_t u_time;

u_time = time(0);
ltime = localtime(&u_time);
strftime(dest, 9, "%T", ltime);

return 0;
}

/*-----------------------------------------------------------------*/
char *
dyn_vsprintf(const char *fmt, va_list ap)
{
char *out;
int len;
int ret;
va_list ap2;

va_copy(ap2, ap);

len = vsnprintf(NULL, 0, fmt, ap);
va_end(ap);
len++;

out = xmalloc(len);
if (out == NULL)
	{
	va_end(ap2);
	return NULL;
	}

ret = vsnprintf(out, len, fmt, ap2);
va_end(ap2);

if (ret >= len)
	{
	xfree(out);
	return NULL;
	}

return out;
}

/*-----------------------------------------------------------------*/
char *dyn_sprintf(const char *fmt, ...)
{
va_list ap;
char *out;

va_start(ap, fmt);

out = dyn_vsprintf(fmt, ap);

va_end(ap);

return out;
}

/*-----------------------------------------------------------------*/
#ifndef RAW_DATA
void
datadump(char *header, void *logdata, int len)
{
return;
}

#else

/*-----------------------------------------------------------------*/
void
datadump(char *header, void *logdata, int len)
{
char hexbuf[80];
char printbuf[80];
char hexchar[9];
char printchar[2];
char *data;

int pos;

data = (char *) logdata;

while (len >= 0)
	{
	pos = 0;

	memset(hexbuf, 0, sizeof(hexbuf));
	memset(printbuf, 0, sizeof(printbuf));

	while (len >= 0 && pos < 16)
		{
		sprintf(hexchar, "%2.2x", *data & 0xff);
		strcat(hexbuf, hexchar);
		strcat(hexbuf, pos == 7 ? "  " : " ");

		if (isprint(*data) && !isspace(*data))
			{
			sprintf(printchar, "%c", *data);
			}
		else
			{
			strcpy(printchar, ".");
			}

		strcat(printbuf, printchar);

		if (pos == 7)
			{
			strcat(printbuf, " ");
			}

		len--;
		data++;
		pos++;
		}

	debug(5, "%s: %-49.49s %s\n", header, hexbuf, printbuf);
	}
}
#endif
