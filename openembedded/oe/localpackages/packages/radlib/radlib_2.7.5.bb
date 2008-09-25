require radlib-common.inc
require radlib.inc

SRC_URI = "${SOURCEFORGE_MIRROR}/radlib/radlib-${PV}.tar.gz \
	file://radlib-2.7.5.pat;patch=1"

PR = "r0"
