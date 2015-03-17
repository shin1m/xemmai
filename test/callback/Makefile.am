check_PROGRAMS = callback.so
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -std=c++11 -fPIC
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
CLEANFILES = test.xm fiber.xm fiber2.xm
%.xm: %.xm.in
	cp $< $@
