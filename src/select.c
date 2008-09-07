#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
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

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include "list.h"
#include "select.h"
#include "common.h"

#include "debug.h"

static struct select_node select_fds;

/*---------------------------------------------------------*/
static int
free_broadcast_msg(struct broadcast_message *bm)
{
free(bm->message);
list_del(&bm->list);

xfree(bm);

return 0;
}

/*---------------------------------------------------------*/
static int
wsd_deliver_broadcast(int fd, int eventtypes, struct select_node *cur)
{
struct broadcast_message *bm;
int ret;

bm = list_entry(cur->broadcast_msgs.list.next,
	struct broadcast_message, list);

/* FIXME: Nonportable - handle SIGPIPE later */
ret = send(fd, bm->message, bm->length, MSG_NOSIGNAL);
fsync(fd);
#if 0
ret = write(fd, bm->message, bm->length);
#endif

/* FIXME: Check ret */
/* FIXME: Do something with ret? */

free_broadcast_msg(bm);

debug(5, "Message broadcast to %d\n", fd);

return 0;
}

/*---------------------------------------------------------*/
static int
set_select_events(struct select_node *select_fds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
struct select_node *cur;
int max_fd;

FD_ZERO(readfds);
FD_ZERO(writefds);
FD_ZERO(exceptfds);

max_fd = 0;

list_for_each_entry(cur, &select_fds->list, list)
	{
	if (cur->events & WSD_FD_READ)
		{
		debug(5, "Setting read check for %d\n", cur->fd);
		FD_SET(cur->fd, readfds);
		}

	if (cur->events & WSD_FD_WRITE ||
		!list_empty(&cur->broadcast_msgs.list))
		{
		debug(5, "Setting write check for %d\n", cur->fd);
		FD_SET(cur->fd, writefds);
		}

	if (cur->events & WSD_FD_EXCEPT)
		{
		debug(5, "Setting exception check for %d\n", cur->fd);
		FD_SET(cur->fd, exceptfds);
		}

	if (cur->fd > max_fd)
		{
		max_fd = cur->fd;
		}
	}

return max_fd;
}

/*---------------------------------------------------------*/
static int
dispatch_select_events(struct select_node *select_fds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds)
{
struct select_node *cur;
struct select_node *tmp;

int eventtypes;
int ret;

list_for_each_entry_safe(cur, tmp, &select_fds->list, list)
	{
	eventtypes = 0;

	debug(5, "Checking select result for fd %d\n", cur->fd);

	if (FD_ISSET(cur->fd, readfds))
		{
		debug(5, "Setting read event type for %d\n", cur->fd);
		eventtypes |= WSD_FD_READ;
		}

	if (FD_ISSET(cur->fd, writefds))
		{
		debug(5, "Setting write event type for %d\n", cur->fd);
		eventtypes |= WSD_FD_WRITE;

		/* Deliver the broadcasts before calling the user */
		/*	callback */
		wsd_deliver_broadcast(cur->fd, eventtypes, cur->data);
		}

	if (FD_ISSET(cur->fd, exceptfds))
		{
		debug(5, "Setting exception event type for %d\n", cur->fd);
		eventtypes |= WSD_FD_EXCEPT;
		}

	if ((eventtypes != 0) && (*cur->callback != NULL))
		{
		debug(5, "Executing callback for %d\n", cur->fd);
		ret = (*cur->callback)(cur->fd, eventtypes, cur->data);

		if (ret != 0)
			{
			return ret;
			}
		}
	}

return 0;
}

/*---------------------------------------------------------*/
int
wsd_init_fd(struct select_node *new_node, int fd,  unsigned int events, int(*callback)(), int recv_broadcasts)
{
new_node->fd = fd;
new_node->events = events;
new_node->callback = callback;
new_node->deliver_broadcasts_to_me = recv_broadcasts;
new_node->data = new_node;
INIT_LIST_HEAD(&new_node->broadcast_msgs.list);

return 0;
}

/*---------------------------------------------------------*/
int
wsd_add_fd(struct select_node *fd)
{
list_add_tail(&fd->list, &select_fds.list);

debug(5, "wsd_add_fd: fd = %d\n", fd->fd);

return 0;
}

/*---------------------------------------------------------*/
int
wsd_del_fd(struct select_node *fd)
{
struct broadcast_message *cur;
struct broadcast_message *tmp;

list_for_each_entry_safe(cur, tmp, &fd->broadcast_msgs.list, list)
	{
	debug(5, "Removing message\n");
	free_broadcast_msg(cur);
	}


debug(5, "Removing node: %d\n", fd->fd);
list_del(&fd->list);

return 0;
}

/*---------------------------------------------------------*/
int
wsd_free_fd(struct select_node *fd)
{
wsd_del_fd(fd);
xfree(fd);

return 0;
}

/*---------------------------------------------------------*/
int
wsd_selector()
{
fd_set readfds;
fd_set writefds;
fd_set exceptfds;
SELECT_TYPE_ARG1 max_fd;

int ret;

max_fd = set_select_events(&select_fds, &readfds, &writefds, &exceptfds);

debug(5, "Before select on %d descriptors\n", max_fd);

ret = select(max_fd + 1, &readfds, &writefds, &exceptfds, NULL);
if (ret <= 0)
	{
	perror("select");
	return ret;
	}

debug(5, "After select\n");

ret = dispatch_select_events(&select_fds, &readfds, &writefds, &exceptfds);

return ret;
}

/*---------------------------------------------------------*/
int
wsd_queue_broadcast(void *msg, unsigned long len)
{
struct select_node *cur;
struct broadcast_message *broadcast_msg;

list_for_each_entry(cur, &select_fds.list, list)
	{
	if (cur->deliver_broadcasts_to_me == 0)
		{
		continue;
		}

	broadcast_msg = xmalloc(sizeof (struct broadcast_message));

	broadcast_msg->message = xmalloc(len);

	memcpy(broadcast_msg->message, msg, len);
	broadcast_msg->length = len;

	list_add_tail(&broadcast_msg->list,
		&cur->broadcast_msgs.list);
	}

return 0;
}

/*---------------------------------------------------------*/
int
wsd_init_selector()
{
/* FIXME: there really needs to be a better way of doing this */
INIT_LIST_HEAD(&select_fds.list);

return 0;
}

#if 0
/*---------------------------------------------------------*/
int test_callback(int fd, int types, void *x)
{
debug(5, "Test callback, processing %d\n", fd);
return 0;
}

int test_run()
{
struct select_node *new_node1;
struct select_node *new_node2;
char msg1[] = "Howdy Pardner";
char msg2[] = "Welcome to the Fair";

/* Test: 1 */
debug(5, "Test 1\n");
debug(5, "------\n");
new_node1 = malloc(sizeof (struct select_node));
wsd_init_fd(new_node1,				/* struct */
	1,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	NULL,							/* callback */
	0								/* no broadcasts */
	);
wsd_add_fd(new_node1);
wsd_free_fd(new_node1);

/* Test: 2 */
debug(5, "Test 2\n");
debug(5, "------\n");
new_node1 = malloc(sizeof (struct select_node));
wsd_init_fd(new_node1,				/* struct */
	1,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	NULL,							/* callback */
	1								/* broadcast to me */
	);
wsd_add_fd(new_node1);
wsd_queue_broadcast(msg1, sizeof(msg1));
/* wsd_queue_broadcast(msg2, sizeof(msg2)); */

wsd_free_fd(new_node1);

/* Test: 3 */
debug(5, "Test 3\n");
debug(5, "------\n");
new_node1 = malloc(sizeof (struct select_node));
wsd_init_fd(new_node1,				/* struct */
	1,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	test_callback,					/* callback */
	1								/* broadcast to me */
	);
wsd_add_fd(new_node1);

new_node2 = malloc(sizeof (struct select_node));
wsd_init_fd(new_node2,				/* struct */
	2,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	test_callback,					/* callback */
	1								/* broadcast to me */
	);
wsd_add_fd(new_node2);

wsd_queue_broadcast(msg1, sizeof(msg1));
wsd_queue_broadcast(msg2, sizeof(msg2));
wsd_selector();

wsd_free_fd(new_node1);
wsd_free_fd(new_node2);

return 0;
}

/*---------------------------------------------------------*/
int main()
{
int ret;

INIT_LIST_HEAD(&select_fds.list);

test_run();

return 0;

ret = init_listener();
if (ret < 0)
	{
	return 1;
	}

while (1)
	{
	wsd_selector();
	}

return 0;
}
#endif

