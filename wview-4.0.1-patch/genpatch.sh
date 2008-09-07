
#
# This needs to be a Makefile.  Get things rolling for now.
#

filenames=`cat` << EOF
configure.in
stations/Makefile.am
stations/BW9xx/bw9xx.c
stations/BW9xx/bw9xx.h
stations/BW9xx/Makefile.am
wviewconfig/Makefile.am
wviewconfig/wviewconfig.sh
EOF

rm wview.pat

for name in $filenames
do
	echo $name
	diff -Nau $1/$name $name >> wview.pat
done

