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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

#include "configfile.h"

struct option_entry
	{
	struct list_head list;
	char *option;
	char *value;
	};

struct option_entry cfgMaster;

static char *trim(char *);

/*---------------------------------------------------------*/
char *
configGetValue(const char *option)
{
struct option_entry *cur;

if (option == NULL)
	{
	return NULL;
	}

list_for_each_entry(cur, &cfgMaster.list, list)
	{
	if (cur->option != NULL)
		{
		if (strcmp(cur->option, option) == 0)
			{
			return cur->value;
			}
		}
	}

/* not found */
return NULL;
}

/*---------------------------------------------------------*/
int
configClose()
{
struct option_entry *cur;
struct option_entry *tmp;

list_for_each_entry_safe(cur, tmp, &cfgMaster.list, list)
	{
	if (cur->option != NULL)
		{
		free(cur->option);
		}

	if (cur->value != NULL)
		{
		free(cur->value);
		}

	list_del(&cur->list);
	free(cur);
	}

return 0;
}

/*---------------------------------------------------------*/
int
configPrint()
{
struct option_entry *cur;

list_for_each_entry(cur, &cfgMaster.list, list)
	{
	printf("<%s> = <%s>\n", cur->option, cur->value);
	}

return 0;
}

/*---------------------------------------------------------*/
static char *
trim(char *s)
{
char *end;

if (s == NULL)
	{
	return NULL;
	}

while (isspace(*s))
	{
	s++;
	}

end = s + strlen(s) - 1;
while ((isspace(*end)) || (*end == '\n') || (*end == '\r'))
	{
	*end = '\0';
	end--;
	}

return s;
}

/*---------------------------------------------------------*/
int
configOpen(const char *filename)
{
FILE *fs;
char *option;
char *value;
char line[1024];
char empty[1];

/* always init list for eventual "get" calls */
INIT_LIST_HEAD(&cfgMaster.list);

fs = fopen(filename, "r");
if (fs == NULL)
	{
	return 1;
	}

while (1)
	{
	struct option_entry *optionitem;
	char *ret;
	char *toption;
	char *tvalue;
	char *tline;

	ret = fgets(line, sizeof(line) - 1, fs);
	if (ret == NULL)
		{
		/* EOF or error */
		break;
		}

	/* trim blanks from beginning and end */
	tline = trim(line);
	if (tline == NULL)
		{
		continue;
		}

	/* continue on comments */
	if (*tline == '#')
		{
		printf("comment: %s\n", trim(line));
		continue;
		}

	option = tline;
	value = strchr(tline, '=');
	if (value == NULL)
		{
		/* consider it an on/off option */
		strcpy(empty, "");
		value = empty;
		}
	else
		{
		*value = '\0';
		value++;
		}

	/* clean the data */
	toption = trim(option);
	tvalue = trim(value);

	/* alloc memory for the linked list node */
	optionitem = malloc(sizeof(struct option_entry));

	/* alloc memory for the option name and save to node */
	optionitem->option = malloc(strlen(toption) + 1);
	strcpy(optionitem->option, toption);

	/* alloc memory for the option value and save to node */
	optionitem->value = malloc(strlen(tvalue) + 1);	
	strcpy(optionitem->value, tvalue);

	/* add to list */
	list_add_tail(&optionitem->list, &cfgMaster.list);
	}

fclose(fs);

return 0;
}
