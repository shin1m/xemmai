moduledir = $(libdir)/xemmai-$(VERSION)

module_PROGRAMS = math.so os.so threading.so time.so
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
math_so_SOURCES = math.cc
os_so_SOURCES = os.cc
threading_so_SOURCES = threading.cc
time_so_SOURCES = time.cc
