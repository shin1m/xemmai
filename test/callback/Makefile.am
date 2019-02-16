check_PROGRAMS = callback.so
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
callback_so_SOURCES = callback.h callback.cc extension.cc

check_SCRIPTS = test.xm fiber.xm fiber2.xm
EXTRA_DIST = test.xm.in fiber.xm.in fiber2.xm.in
CLEANFILES = $(check_SCRIPTS)
%.xm: %.xm.in
	cp $< $@
