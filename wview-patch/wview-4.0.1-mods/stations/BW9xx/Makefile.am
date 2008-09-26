# Makefile - Simulator station daemon

#define the executable to be built
bin_PROGRAMS    = wviewd

# define include directories
INCLUDES = \
		-I$(top_srcdir)/common \
		-I$(top_srcdir)/stations/common \
		-I$(prefix)/include \
		-DHOST_IS_BIGENDIAN=@HOST_IS_BIGENDIAN@ \
		-D_GNU_SOURCE \
		-DWV_CONFIG_DIR=\"$(prefix)/etc/wview\" \
		-DWV_RUN_DIR=\"$(prefix)/var/wview\" \
		-DBUILD_WVIEWD

# define the sources
wviewd_SOURCES     = \
		$(top_srcdir)/common/sensor.c \
		$(top_srcdir)/common/wvutils.c \
		$(top_srcdir)/common/dbfiles.c \
		$(top_srcdir)/common/windAverage.c \
		$(top_srcdir)/stations/common/computedData.c \
		$(top_srcdir)/stations/common/daemon.c \
		$(top_srcdir)/stations/common/station.c \
		$(top_srcdir)/stations/common/serial.c \
		$(top_srcdir)/stations/common/ethernet.c \
		$(top_srcdir)/stations/common/stormRain.c \
		$(top_srcdir)/stations/BW9xx/bw9xx.c \
		$(top_srcdir)/common/sensor.h \
		$(top_srcdir)/common/datadefs.h \
		$(top_srcdir)/common/dbfiles.h \
		$(top_srcdir)/common/services.h \
		$(top_srcdir)/common/sysdefs.h \
		$(top_srcdir)/common/windAverage.h \
		$(top_srcdir)/common/beaufort.h \
		$(top_srcdir)/stations/common/computedData.h \
		$(top_srcdir)/stations/common/daemon.h \
		$(top_srcdir)/stations/common/station.h \
		$(top_srcdir)/stations/common/serial.h \
		$(top_srcdir)/stations/common/ethernet.h \
		$(top_srcdir)/stations/common/stormRain.h \
		$(top_srcdir)/stations/BW9xx/bw9xx.h

# define libraries
wviewd_LDADD       = -lc -lm -lrad -lpthread

if DB_MYSQL
wviewd_LDADD       += -lmysqlclient
else
if DB_PGRESQL
wviewd_LDADD       += -lpq
endif
endif


# define library directories
wviewd_LDFLAGS     = -L/usr/lib -L/usr/local/lib -L$(prefix)/lib

if DB_MYSQL
wviewd_LDFLAGS     += -L$(prefix)/lib/mysql -L/usr/lib/mysql
else
if DB_PGRESQL
wviewd_LDFLAGS     += -L$(prefix)/lib -L$(prefix)/pgsql/lib
INCLUDES           += -I$(prefix)/pgsql/include
endif
endif
