#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>

#include "thermorwsd.h"
#include "debug.h"

extern struct ws_prog_options prog_options;

void
debug(int lvl, const char *fmt, ...)
{
va_list ap;

if (prog_options.debug_lvl < lvl)
	{
	return;
	}

va_start(ap, fmt);
vprintf(fmt, ap);

return;
}
