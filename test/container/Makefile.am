check_PROGRAMS = container.so
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -std=c++17 -fPIC
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
container_so_SOURCES = queue.h queue.cc container.h container.cc

check_SCRIPTS = test.xm
EXTRA_DIST = test.xm.in
CLEANFILES = $(check_SCRIPTS)
%.xm: %.xm.in
	cp $< $@
