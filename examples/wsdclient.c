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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

int
main()
{
char path[] = "/tmp/wsd";
char buf[80];
struct sockaddr_un sun;
int fd;
int ret;

fd = socket(PF_UNIX, SOCK_STREAM, 0);
if (fd == -1)
	{
	perror("socket");
	return EXIT_FAILURE;
	}

memset(&sun, 0, sizeof (sun));
sun.sun_family = AF_UNIX;
memcpy(sun.sun_path, path, strlen(path) + 1);

ret = connect(fd, (struct sockaddr *) &sun, sizeof (sun));
if (ret == -1)
	{
	perror("connect");
	return EXIT_FAILURE;
	}

ret = read(fd, buf, sizeof(buf));
while (ret > 0)
	{
	write(STDOUT_FILENO, buf, ret);
	ret = read(fd, buf, sizeof(buf));
	}

close(fd);

return EXIT_SUCCESS;
}
