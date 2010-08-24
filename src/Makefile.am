## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/aum/ui
ui_DATA = aum.ui

conffile  = ${sysconfdir}/pacman.conf
dbpath    = ${localstatedir}/lib/pacman/
cachedir  = ${localstatedir}/cache/pacman/pkg/
logfile   = ${localstatedir}/log/pacman.log

bin_PROGRAMS = aum

DEFS = -DLOCALEDIR=\"@localedir@\" \
       -DCONFFILE=\"$(conffile)\" \
       -DROOTDIR=\"$(ROOTDIR)\" \
       -DDBPATH=\"$(dbpath)\" \
       -DCACHEDIR=\"$(cachedir)\" \
       -DLOGFILE=\"$(logfile)\" \
       @DEFS@
AM_CFLAGS = -Wall -pedantic -lalpm -g -D_GNU_SOURCE

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(AUM_CFLAGS)

aum_SOURCES = \
	aum.h \
	aum.c \
	conf.c \
	conf.h \
	sync.h \
	sync.c \
	main-window.h \
	main-window.c \
	util.c \
	util.h \
	error.h 
	
aum_LDFLAGS = \
	-Wl,--export-dynamic

aum_LDADD = $(AUM_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/aum
