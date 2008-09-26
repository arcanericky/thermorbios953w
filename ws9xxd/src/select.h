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

#ifndef _SELECT_H
#define _SELECT_H

#define WSD_FD_READ		0x01
#define WSD_FD_WRITE	0x02
#define WSD_FD_EXCEPT	0x04

struct broadcast_message
	{
	struct list_head list;
	void *message;
	unsigned long length;
	};

struct select_node
	{
	struct list_head list;

	int fd;
	unsigned int events;
	int (*callback)();

	int broadcast_my_messages;
	int deliver_broadcasts_to_me;

	void *data;

	struct broadcast_message broadcast_msgs;
	};


int wsd_selector(void);
int wsd_queue_broadcast(void *, unsigned long);
int wsd_init_fd(struct select_node *, int,  unsigned int, int(*callback)(), int);
int wsd_add_fd(struct select_node *);
int wsd_del_fd(struct select_node *);
int wsd_init_selector(void);
int wsd_free_fd(struct select_node *);

#endif /* _SELECT_H */
