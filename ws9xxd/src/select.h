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
