bin_PROGRAMS = ws9xxd

# define include directories
INCLUDES = \
		-DHOST_IS_BIGENDIAN=@HOST_IS_BIGENDIAN@ \
		-DBW9XX_CFG_DIR=$(prefix)/etc/ws9xxd

noinst_HEADERS = bw953.h \
thermorwsd.h \
datahandlers.h \
datadisplay.h \
datadispcsv.h \
common.h \
select.h \
debug.h \
servers.h \
list.h

ws9xxd_SOURCES = bw953.c \
thermorwsd.c \
datahandlers.c \
datadisplay.c \
datadispcsv.c \
util.c \
select.c \
debug.c \
servers.c

AM_CFLAGS = -Wall

#bin_SCRIPTS = wsdpyclient.py

#wsdpyclient.py: wsdpyclient.py.in
#	cp wsdpyclient.py.in wsdpyclient.py
#	chmod +x wsdpyclient.py


