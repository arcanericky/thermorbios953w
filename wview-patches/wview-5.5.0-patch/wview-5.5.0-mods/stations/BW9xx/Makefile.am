# Makefile - station daemon

# define the executable to be built
bin_PROGRAMS    = wviewd_bw9xx

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
wviewd_bw9xx_SOURCES     = \
		$(top_srcdir)/common/sensor.c \
		$(top_srcdir)/common/wvutils.c \
		$(top_srcdir)/common/wvconfig.c \
		$(top_srcdir)/common/dbsqlite.c \
		$(top_srcdir)/common/dbsqliteHiLow.c \
		$(top_srcdir)/common/windAverage.c \
		$(top_srcdir)/common/emailAlerts.c \
		$(top_srcdir)/stations/common/computedData.c \
		$(top_srcdir)/stations/common/daemon.c \
		$(top_srcdir)/stations/common/station.c \
		$(top_srcdir)/stations/common/serial.c \
		$(top_srcdir)/stations/common/ethernet.c \
		$(top_srcdir)/stations/common/stormRain.c \
		$(top_srcdir)/stations/common/parser.c \
		$(top_srcdir)/stations/BW9xx/bw9xx.c \
		$(top_srcdir)/common/sensor.h \
		$(top_srcdir)/common/datadefs.h \
		$(top_srcdir)/common/dbsqlite.h \
		$(top_srcdir)/common/services.h \
		$(top_srcdir)/common/sysdefs.h \
		$(top_srcdir)/common/wvconfig.h \
		$(top_srcdir)/common/windAverage.h \
		$(top_srcdir)/common/emailAlerts.h \
		$(top_srcdir)/common/beaufort.h \
		$(top_srcdir)/stations/common/computedData.h \
		$(top_srcdir)/stations/common/daemon.h \
		$(top_srcdir)/stations/common/station.h \
		$(top_srcdir)/stations/common/serial.h \
		$(top_srcdir)/stations/common/ethernet.h \
		$(top_srcdir)/stations/common/stormRain.h \
		$(top_srcdir)/stations/common/parser.h \
		$(top_srcdir)/stations/BW9xx/bw9xx.h

# define libraries
wviewd_bw9xx_LDADD       = -lc -lm -lrad -lsqlite3

# define library directories
wviewd_bw9xx_LDFLAGS     = -L/usr/lib -L/usr/local/lib -L$(prefix)/lib
