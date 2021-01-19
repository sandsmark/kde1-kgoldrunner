# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# just set the variable
APPSDIR = $(kde_appsdir)/Games
# set the include path for X, qt and KDE
INCLUDES= @all_includes@
# claim, which subdirectories you want to install
SUBDIRS = doc pics levels

#DEFS = @DEFS@ -I. -I$(srcdir) -I.. -DDOTRACE
####### This part is very kgoldrunner specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	kgoldrunner

# Which sources should be compiled for kgoldrunner.
kgoldrunner_SOURCES	= kgoldrunner.cpp kgoldrunnerwidget.cpp main.cpp kgrobj.cpp

# the library search path
kgoldrunner_LDFLAGS = @all_libraries@ @KDE_RPATH@

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kgoldrunner_LDADD  = -lkdeui -lkdecore -lqt -lX11 -lXext @LIBSOCKET@     

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = kgoldrunner.h kgoldrunnerwidget.h kgrobj.h

# just to make sure, automake makes them 
BUILTSOURCES =	kgoldrunner.moc kgoldrunnerwidget.moc kgrobj.moc

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ki18n $(kgoldrunner_SOURCES)

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) kgoldrunner.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) kgoldrunner.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kgoldrunner.xpm $(kde_minidir)/kgoldrunner.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/kgoldrunner.kdelnk
	-rm -f $(kde_icondir)/kgoldrunner.xpm
	-rm -f $(kde_minidir)/kgoldrunner.xpm

# add a dependency for every moc file to be full portable
kgrobj.cpp:kgrobj.moc
kgrobj.moc:kgrobj.h
	$(MOC) kgrobj.h -o kgrobj.moc

kgoldrunner.cpp:kgoldrunner.moc
kgoldrunner.moc: kgoldrunner.h
	$(MOC) kgoldrunner.h -o kgoldrunner.moc

kgoldrunnerwidget.cpp:kgoldrunnerwidget.moc
kgoldrunnerwidget.moc: kgoldrunnerwidget.h
	$(MOC) kgoldrunnerwidget.h -o kgoldrunnerwidget.moc

