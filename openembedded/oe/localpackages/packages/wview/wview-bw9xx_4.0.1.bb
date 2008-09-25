require wview-common.inc
require wview.inc

SRC_URI = "${SOURCEFORGE_MIRROR}/wview/wview-${PV}.tar.gz \
	file://wview-4.0.1.pat;patch=1"

EXTRA_OECONF += " --enable-station-bw9xx"
PR = "r0"
