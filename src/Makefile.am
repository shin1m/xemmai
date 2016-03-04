bin_PROGRAMS = xemmai
if JIT
AM_CPPFLAGS = -DXEMMAI_ENABLE_JIT
AM_LDFLAGS = $(LLVM_LDFLAGS)
LDADD = libjit.a $(LLVM_LIBS)
else
AM_CPPFLAGS =
AM_LDFLAGS =
LDADD =
endif
AM_CPPFLAGS += -I$(top_srcdir)/include -DXEMMAI_MODULE_PATH="$(libdir)/xemmai-$(VERSION)"
AM_CXXFLAGS = -std=c++1y
AM_LDFLAGS += -rdynamic
LDADD += -ldl -lpthread -lrt
if DEBUG
AM_CXXFLAGS += -O0 -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
xemmai_SOURCES = \
	portable.cc \
	value.cc \
	type.cc \
	object.cc \
	class.cc \
	structure.cc \
	module.cc \
	fiber.cc \
	thread.cc \
	tuple.cc \
	symbol.cc \
	scope.cc \
	ast.cc \
	code_operator.h \
	code.cc \
	method.cc \
	lambda.cc \
	native.cc \
	throwable.cc \
	null.cc \
	boolean.cc \
	integer.cc \
	float.cc \
	string.cc \
	array.cc \
	dictionary.cc \
	bytes.cc \
	lexer.cc \
	parser.cc \
	global.cc \
	io.cc \
	io__file.cc \
	io__reader.cc \
	io__writer.cc \
	io__path.cc \
	engine.cc \
	main.cc
if JIT
noinst_LIBRARIES = libjit.a
libjit_a_CPPFLAGS = $(LLVM_CPPFLAGS) $(AM_CPPFLAGS)
libjit_a_CXXFLAGS = $(LLVM_CXXFLAGS) $(AM_CXXFLAGS) -Wno-non-virtual-dtor -Wno-switch
libjit_a_SOURCES = jit.cc
endif
