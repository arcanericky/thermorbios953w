
#
# This needs to be a Makefile.  Get things rolling for now.
#

if [ $# -eq 0 ]; then
	echo Need a wview source directory
	exit 0
fi

wviewdir=$1
if [ ! -d $wviewdir ]; then
	echo Invalid wview source directory
	exit 0
fi

filenames=`cat` << EOF
configure.in
stations/Makefile.am
stations/BW9xx/bw9xx.c
stations/BW9xx/bw9xx.h
stations/BW9xx/Makefile.am
wviewconfig/Makefile.am
wviewconfig/wviewconfig.sh
EOF

rm -f wview.pat

for name in $filenames
do
	echo $name
	diff -Nau $wviewdir/$name $name >> wview.pat
done

ls -l wview.pat

