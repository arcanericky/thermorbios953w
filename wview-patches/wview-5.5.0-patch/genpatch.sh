
PGMDIR=wview-5.5.0
PGMNAME=${PGMDIR}.tar.gz
PGMMODS=${PGMDIR}-mods
URL=http://downloads.sourceforge.net/wview

#rm -f $PGMNAME 2>/dev/null
rm -rf $PGMDIR
rm -f $PGMDIR.pat

if [ ! -f $PGMNAME ]
then
	wget $URL/$PGMNAME
fi

echo Unrolling archive
tar zxf $PGMNAME
#rm -f $PGMNAME

echo Creating patch
for name in `cd $PGMMODS; find . -type f -print | grep -v \.svn/`
do
	diff -Nau $PGMDIR/$name $PGMMODS/$name >> $PGMDIR.pat
done

echo Cleaning up
rm -rf $PGMDIR

echo Done
ls -l $PGMDIR.pat
