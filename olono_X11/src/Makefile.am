lib_LTLIBRARIES         = libCases.la libIA.la libPlay.la libDisplay.la libGUI.la

libCases_la_SOURCES     = libCases.c libDefine.h
libIA_la_SOURCES        = libIA.c libDefine.h
libPlay_la_SOURCES      = libPlay.c libDefine.h
libDisplay_la_SOURCES   = libDisplay.c libDefine.h
libGUI_la_SOURCES       = libGUI.c libDefine.h

dist_include_HEADERS    = libCases.h libIA.h libPlay.h libDisplay.h libGUI.h libDefine.h





bin_PROGRAMS            = olono-gtk

### On indique les sources du programme
olono_gtk_SOURCES       = main.c 

### On link les dépendances du programme.
olono_gtk_LDADD         = $(DEPS_LIBS) libCases.la libIA.la libPlay.la libDisplay.la libGUI.la

### C'est la section flag C, mais ils vont passer sous libtool.
### Donc suivant la version du compilo et des versions, -export-dynamic
### sera transformé en -Wl,--exort-dynamic (GCC) ou autre chose.
olono_gtk_LDFLAGS       = -export-dynamic

### Puis les flags C et libtool
if DEBUG
AM_CPPFLAGS             = -ggdb -p -Wall -DDEBUG $(DEPS_CFLAGS) -DIMGS_DIR=\""$(datadir)/imgs"\" -DGLADE_DIR=\""$(datadir)/glade"\"
else
AM_CPPFLAGS             = $(DEPS_CFLAGS) -DIMGS_DIR=\""$(datadir)/imgs"\" -DGLADE_DIR=\""$(datadir)/glade"\"
endif
