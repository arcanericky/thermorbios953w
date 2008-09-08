
# What would be great is a Makefile to do all this for the user,
# but for now, just give them something they can try in an empty
# directory

# Get wview
wget http://voxel.dl.sourceforge.net/sourceforge/wview/wview-4.0.1.tar.gz

# Get wview patch
svn co https://thermorbios953w.svn.sourceforge.net/svnroot/thermorbios953w/trunk/wview-4.0.1-patch

# Unroll wview
tar zxvf wview-4.0.1.tar.gz

# Create patch
cd wview-4.0.1-patch
sh genpatch.sh ../wview-4.0.1

# Apply patch
cd ../wview-4.0.1
patch -p0 < ../wview-4.0.1-patch/wview.pat

# Update with autotools
aclocal
autheader
automake
autoconf

# configure and make
./configure --enable-station-bw9xx
make
