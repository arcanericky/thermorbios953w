# Makefile - stations

# add subdirectories to be included in the distribution 
# (thus requiring Makefile.am files)
# we always default to VantagePro if not given or not recognized...
if STATION_VPRO
SUBDIRS = VantagePro
else
if STATION_WXT510
SUBDIRS = WXT510
else
if STATION_WS2300
SUBDIRS = WS-2300
else
if STATION_WMR918
SUBDIRS = WMR918
else
if STATION_SIM
SUBDIRS = Simulator
else
if STATION_BW9XX
SUBDIRS = BW9xx
else
SUBDIRS = VantagePro
endif
endif
endif
endif
endif
endif
