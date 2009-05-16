# Makefile - top level

# files to include in the distro but not build
EXTRA_DIST = reconf \
             $(top_srcdir)/stations/VantagePro/doc \
             $(top_srcdir)/bin \
             $(top_srcdir)/examples/alarms \
             $(top_srcdir)/examples/conf \
             $(top_srcdir)/examples/html \
             $(top_srcdir)/examples/scripts \
             $(top_srcdir)/cross-compile \
             $(top_srcdir)/alarms/sample-datafeed-client \
             $(top_srcdir)/ftp/wviewftp.debug.sh \
             $(top_srcdir)/UPGRADE \
             $(top_srcdir)/MAINTAIN \
             $(top_srcdir)/wview-User-Manual.html \
             $(top_srcdir)/wview-Quick-Start.html \
             $(top_srcdir)/wview-Quick-Start-MacOSX.html


SUBDIRS = stations htmlgenerator alarms cwop ftp ssh procmon wviewconfig wviewmgmt utilities dbexport examples

if HTTP
    SUBDIRS	+= http
endif

if STATION_VPRO
    STATION_T = wviewd_vpro
else
if STATION_WXT510
    STATION_T = wviewd_wxt510
else
if STATION_WS2300
    STATION_T = wviewd_ws2300
else
if STATION_WMR918
    STATION_T = wviewd_wmr918
else
if STATION_SIM
    STATION_T = wviewd_sim
else
if STATION_BW9XX
    STATION_T = wviewd_bw9xx
else
    STATION_T = wviewd_vpro
endif
endif
endif
endif
endif
endif

WV_CONFIG_DIR 		= $(prefix)/etc/wview
WV_RUN_DIR    		= $(prefix)/var/wview


# this target removes svn directories from the tarball
dist-hook:
	rm -rf `find $(distdir) -name .svn`


# this target installs the default environment in the default places
install-env:	install
	@echo "Removing OLD wview station daemon $(exec_prefix)/bin/wviewd"
	rm -f $(exec_prefix)/bin/wviewd
	@echo "Removing OLD wview SQL daemon $(exec_prefix)/bin/wviewsqld"
	rm -f $(exec_prefix)/bin/wviewsqld
	@if [ ! -d $(WV_RUN_DIR) ]; then \
		mkdir -p $(WV_RUN_DIR); \
		mkdir -p $(WV_RUN_DIR)/noaa; \
		mkdir -p $(WV_RUN_DIR)/alarms; \
		mkdir -p $(WV_RUN_DIR)/img; \
		mkdir -p $(WV_RUN_DIR)/img/Archive; \
		mkdir -p $(WV_RUN_DIR)/img/NOAA; \
		cp -rf $(top_srcdir)/bin/img $(WV_RUN_DIR); \
		cp -rf $(top_srcdir)/bin/archive $(WV_RUN_DIR); \
		sqlite3 $(WV_RUN_DIR)/archive/wview-archive.sdb '.read $(WV_RUN_DIR)/archive/wview-archive.sql'; \
		echo "$(WV_RUN_DIR) has been created with distro examples"; \
	else \
		echo "$(WV_RUN_DIR) already exists - no installation done (make sure you don't need to move old directories out of the way to install new ones)"; \
	fi
	@if [ ! -d $(WV_CONFIG_DIR) ]; then \
		mkdir -p $(WV_CONFIG_DIR); \
		mkdir -p $(WV_CONFIG_DIR)/export; \
		chmod 777 $(WV_CONFIG_DIR)/export; \
		cp -rf $(top_srcdir)/examples/html $(WV_CONFIG_DIR); \
		cp -rf $(top_srcdir)/examples/alarms $(WV_CONFIG_DIR); \
		cp -rf $(top_srcdir)/examples/conf/*.* $(WV_CONFIG_DIR); \
		sqlite3 $(WV_CONFIG_DIR)/wview-conf.sdb '.read $(WV_CONFIG_DIR)/wview-conf.sql'; \
		echo "$(WV_CONFIG_DIR) has been created with distro examples:"; \
		echo "Don't forget to run \"wviewhtmlconfig\" to configure the template directory"; \
	else \
		echo "$(WV_CONFIG_DIR) already exists - no installation done (make sure you don't need to move old directories out of the way to install new ones)"; \
	fi
	@echo "$(PACKAGE_VERSION)" > $(WV_CONFIG_DIR)/wview-version
	@echo "$(STATION_T)" > $(WV_CONFIG_DIR)/wview-binary
	@chmod 666 $(WV_CONFIG_DIR)/wview-binary
	@chmod 666 $(WV_CONFIG_DIR)/wview-conf.sdb
	@chmod 777 $(WV_CONFIG_DIR)
	@if [ -e /etc/network/interfaces ]; then \
		if [ ! -d /home/www-data ]; then \
			echo "Creating /home/www-data for network setup (Debian, Ubuntu specific):"; \
			mkdir -p /home/www-data; \
			mv /etc/network/interfaces /home/www-data; \
			ln -s /home/www-data/interfaces /etc/network/interfaces; \
			mv /etc/resolv.conf /home/www-data; \
			ln -s /home/www-data/resolv.conf /etc/resolv.conf; \
			chown -R www-data:www-data /home/www-data; \
		fi \
	else \
		echo "Non-Debian distribution: wviewmgmt network setup disabled."; \
	fi
	@if [ "x$(HTTP_DOC_ROOT)" != "x" ]; then \
		echo "Copying wviewmgmt to $(HTTP_DOC_ROOT)/wviewmgmt"; \
		rm -rf $(HTTP_DOC_ROOT)/wviewmgmt; \
		cp -rf $(top_srcdir)/wviewmgmt $(HTTP_DOC_ROOT); \
		echo "For wview start/stop control via wviewmgmt, add your http user to /etc/sudoers"; \
		echo "(with no password required for a few commands):"; \
		echo "\"sudo visudo\""; \
		echo "(add the following line to the bottom of the file):"; \
		echo "\"www-data ALL = NOPASSWD: /etc/init.d/wview, /etc/init.d/networking, /usr/local/bin/wviewPlug-updater\""; \
		echo "(save and exit the file):"; \
		echo "Only these three commands are enabled as root for the http user."; \
		echo "Then point your browser to: http://[your_server_url_or_IP]/wviewmgmt/login.php"; \
		echo "to manage wview via your web browser."; \
		echo "DISCLAIMER: web control via sudo may be a security threat, enable at your own risk."; \
	else \
		echo "Environment variable HTTP_DOC_ROOT is not defined; cannot install wviewmgmt"; \
		echo "To install wviewmgmt, define your HTTP path during configuration:"; \
		echo "\"HTTP_DOC_ROOT=[your_http_path] ./configure --enable-station-[station] ...\""; \
	fi

