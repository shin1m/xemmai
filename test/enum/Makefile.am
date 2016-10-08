check_PROGRAMS = enum.so
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -fPIC
AM_LDFLAGS = -shared
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
enum_so_SOURCES = enum.cc

check_SCRIPTS = test.xm
CLEANFILES = test.xm
test.xm: test.xm.in
	cp $< $@
