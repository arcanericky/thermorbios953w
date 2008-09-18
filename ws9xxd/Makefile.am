
EXTRA_DIST = wview-4.0.1-patch

SUBDIRS = src \
	examples

# this target removes svn directories from the tarball
dist-hook:
	rm -rf `find $(distdir) -name .svn`
