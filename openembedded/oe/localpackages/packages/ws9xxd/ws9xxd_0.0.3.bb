DESCRIPTION = "Thermor / BIOS BW9xx Weather Station Server"

PN = "ws9xxd"
PV = "0.0.3"

SRC_URI = "svn://thermorbios953w.svn.sourceforge.net/svnroot/thermorbios953w/trunk;module=ws9xxd;proto=https;rev=53"
S = "${WORKDIR}/${PN}"

#
# For development
#
#SRC_URI = "http://localhost/ws9xxd/${PN}-${PV}.tar.gz"
#S =

inherit autotools

PR = "r0"
