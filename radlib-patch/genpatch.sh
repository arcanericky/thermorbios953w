
PGMDIR=radlib-2.7.5
PGMNAME=${PGMDIR}.tar.gz
PGMMODS=${PGMDIR}-mods
URL=http://downloads.sourceforge.net/radlib

rm -f $PGMNAME 2>/dev/null
rm -rf $PGMDIR
rm -f $PGMDIR.pat

wget $URL/$PGMNAME
tar zxf $PGMNAME
rm -f $PGMNAME

for name in `cd $PGMMODS; find . -type f -print`
do
	diff -Nau $PGMDIR/$name $PGMMODS/$name >> $PGMDIR.pat
done

rm -rf $PGMDIR
ls -l $PGMDIR.pat
