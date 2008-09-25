
EXTRA_DIST = dists

SUBDIRS = src \
	examples

# this target removes svn directories from the tarball
dist-hook:
	rm -rf `find $(distdir) -name .svn`
