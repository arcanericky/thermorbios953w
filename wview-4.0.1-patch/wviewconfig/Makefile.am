# Makefile - wviewconfig/wviewhtmlconfig

EXTRA_DIST = $(srcdir)/wviewconfig.sh $(srcdir)/wviewhtmlconfig.sh

# define the scripts to be generated
bin_SCRIPTS = wviewconfig wviewhtmlconfig

CLEANFILES = $(bin_SCRIPTS)

if STATION_VPRO
MY_STATION_TYPE = VantagePro
else
if STATION_WXT510
MY_STATION_TYPE = WXT510
else
if STATION_WS2300
MY_STATION_TYPE = WS-2300
else
if STATION_WMR918
MY_STATION_TYPE = WMR918
else
if STATION_BW9XX
MY_STATION_TYPE = BW9XX
else
if STATION_SIM
MY_STATION_TYPE = Simulator
else
MY_STATION_TYPE = VantagePro
endif
endif
endif
endif
endif
endif

wviewconfig: $(srcdir)/wviewconfig.sh
	rm -f wviewconfig
	echo "#!/bin/sh" > wviewconfig
	echo "WVIEW_CONF_DIR=$(prefix)/etc/wview" >> wviewconfig
	echo "WVIEW_DATA_DIR=$(prefix)/var/wview" >> wviewconfig
	echo "STATION_TYPE=$(MY_STATION_TYPE)" >> wviewconfig
	cat $(srcdir)/wviewconfig.sh >> wviewconfig
	chmod ugo+x wviewconfig

wviewhtmlconfig: $(srcdir)/wviewhtmlconfig.sh
	rm -f wviewhtmlconfig
	echo "#!/bin/sh" > wviewhtmlconfig
	echo "WVIEW_CONF_DIR=$(prefix)/etc/wview" >> wviewhtmlconfig
	echo "WVIEW_DATA_DIR=$(prefix)/var/wview" >> wviewhtmlconfig
	echo "WVIEWCONFIG=$(exec_prefix)/bin/wviewconfig" >> wviewhtmlconfig
	cat $(srcdir)/wviewhtmlconfig.sh >> wviewhtmlconfig
	chmod ugo+x wviewhtmlconfig

