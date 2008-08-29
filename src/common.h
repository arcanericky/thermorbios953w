#ifndef _COMMON_H
#define _COMMON_H

void *xmalloc(size_t);
void xfree(void *);

int get_time(char *);
void datadump(char *, void *, int);

#endif
