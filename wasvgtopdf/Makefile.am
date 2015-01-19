AUTOMAKE_OPTIONS = foreign subdir-objects
CFLAGS=-Wall -I/usr/local/include -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\"" 
LDFLAGS=-L/usr/local/lib -L/usr/lib -lstdc++ -lrsvg-2 -lcairo -lgobject-2.0
CPPFLAGS=-Wall -I/usr/local/include -I/usr/local/include/cairo -I/usr/local/include/librsvg-2.0/librsvg -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include/gdk-pixbuf-2.0 -DNDEBUG
bin_PROGRAMS = wasvgtopdf
wasvgtopdf_SOURCES = wasvg.cc
