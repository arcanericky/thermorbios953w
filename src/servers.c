#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "list.h"
#include "select.h"
#include "common.h"
#include "thermorwsd.h"

#include "debug.h"

extern struct ws_prog_options prog_options;

/*---------------------------------------------------------*/
static int
client_cb(int fd, int eventtypes, void *data)
{
char buf[1024];
struct select_node *active_fd;
int ret;

active_fd = data;

debug(5, "Client side callback\n");

if (eventtypes & WSD_FD_READ)
	{
	debug(5, "Client read ready\n");

	memset(buf, 0, sizeof(buf));
	ret = read(fd, &buf, sizeof(buf));
	if (ret == 0)
		{
		debug(5, "Client closed connection\n");
		close(fd);

		wsd_free_fd(active_fd);
		}
	else if (ret == -1)
		{
		debug(5, "Error reading client connection\n");
		close(fd);

		wsd_free_fd(active_fd);
		}
	else
		{
		/* Incoming data ignored (at least for now) */
		}
	}

if (eventtypes & WSD_FD_WRITE)
	{
	debug(5, "Client write ready\n");
	}

if (eventtypes & WSD_FD_EXCEPT)
	{
	debug(5, "Client exception ready\n");
	}

return 0;
}

/*---------------------------------------------------------*/
static int
listener_cb(int accept_fd, int eventtypes, void *data)
{
struct select_node *new_node;
int new_fd;

debug(5, "Accepting connection on: %d\n", accept_fd);

new_fd = accept(accept_fd, NULL, 0);
if (new_fd < 0)
	{
	perror("accept");
	return 1;
	}

debug(5, "Accepted new connection: %d\n", new_fd);

new_node = xmalloc(sizeof (struct select_node));

wsd_init_fd(new_node,				/* struct */
	new_fd,							/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	client_cb,						/* callback */
	1								/* broadcasts */
	);
	
wsd_add_fd(new_node);

return 0;
}

/*---------------------------------------------------------*/

int
close_local_listener()
{
return 0;
}

/*---------------------------------------------------------*/
int
init_local_listener()
{
struct sockaddr_un sun;
struct select_node *new_node;
mode_t old_umask;
int fd;
int ret;

fd = socket(PF_UNIX, SOCK_STREAM, 0);
if (fd < 0)
	{
	perror("socket");
	return -1;
	}

memset(&sun, 0, sizeof(sun));

sun.sun_family = AF_UNIX;
memcpy(sun.sun_path, prog_options.unix_path,
	strlen(prog_options.unix_path) + 1);
unlink(sun.sun_path);

old_umask = umask(0111);
ret = bind(fd, (struct sockaddr *) &sun, sizeof(sun));
if (ret < 0)
	{
	perror("bind");
	umask(old_umask);
	close(fd);
	return -1;
	}

umask(old_umask);
ret = listen(fd, 5);
if (ret < 0)
	{
	perror("listen");
	close(fd);
	return -1;
	}

new_node = xmalloc(sizeof (struct select_node));

wsd_init_fd(new_node,				/* struct */
	fd,								/* file descriptor */
	WSD_FD_READ | WSD_FD_EXCEPT,	/* events */
	listener_cb,					/* callback */
	0								/* no broadcasts */
	);

wsd_add_fd(new_node);

return fd;
}
