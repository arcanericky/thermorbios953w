#ifndef _COMMON_H
#define _COMMON_H

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

void *xmalloc(size_t);
void xfree(void *);

int get_time(char *);
void datadump(char *, void *, int);

char *dyn_vsprintf(const char *, va_list);
char *dyn_sprintf(const char *, ...);

#endif
