# Makefile - top level

# files to include in the distro but not build
EXTRA_DIST = reconf \
             $(top_srcdir)/stations/VantagePro/doc \
             $(top_srcdir)/bin \
             $(top_srcdir)/examples/alarms \
             $(top_srcdir)/examples/conf \
             $(top_srcdir)/examples/html \
             $(top_srcdir)/cross-compile \
             $(top_srcdir)/alarms/sample-datafeed-client \
             $(top_srcdir)/UPGRADE \
             $(top_srcdir)/MAINTAIN \
             $(top_srcdir)/wview-User-Manual.html \
             $(top_srcdir)/wview-Quick-Start.html \
             $(top_srcdir)/wview-Quick-Start-MacOSX.html


SUBDIRS = stations

if DB_MYSQL
SUBDIRS += sqld
else
if DB_PGRESQL
SUBDIRS += sqld
endif
endif

SUBDIRS += htmlgenerator alarms cwop ftp ssh procmon wviewconfig utilities examples

if HTTP
    SUBDIRS	+= http
endif


WV_CONFIG_DIR = $(prefix)/etc/wview
WV_RUN_DIR    = $(prefix)/var/wview


# this target removes svn directories from the tarball
dist-hook:
	rm -rf `find $(distdir) -name .svn`


# this target installs the default environment in the default places
install-env:	install
	mkdir -p $(WV_CONFIG_DIR)
	mkdir -p $(WV_RUN_DIR)
	mkdir -p $(WV_RUN_DIR)/noaa
	mkdir -p $(WV_RUN_DIR)/alarms
	mkdir -p $(WV_RUN_DIR)/archive
	mkdir -p $(WV_RUN_DIR)/img
	mkdir -p $(WV_RUN_DIR)/img/Archive
	mkdir -p $(WV_RUN_DIR)/img/NOAA
	cp -rf $(WV_RUN_DIR)/img $(WV_RUN_DIR)/img-`date '+%Y%m%d.%H%M%S'`
	cp -rf $(top_srcdir)/bin/img $(WV_RUN_DIR)
	cp -rf $(WV_CONFIG_DIR) $(WV_CONFIG_DIR)-`date '+%Y%m%d.%H%M%S'`
	rm -rf $(WV_CONFIG_DIR)/html
	cp -rf $(top_srcdir)/examples/html $(WV_CONFIG_DIR)
	cp -rf $(top_srcdir)/examples/alarms $(WV_CONFIG_DIR)
	cp -rf $(top_srcdir)/examples/conf/*.* $(WV_CONFIG_DIR)

