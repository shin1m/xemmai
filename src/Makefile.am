bin_PROGRAMS = xemmai
AM_CPPFLAGS = -I$(top_srcdir)/include -DXEMMAI_MODULE_PATH="$(libdir)/xemmai-$(VERSION)"
AM_CXXFLAGS =
AM_LDFLAGS = -rdynamic
LDADD = -ldl -lpthread -lrt
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
xemmai_SOURCES = \
	portable.cc \
	value.cc \
	hash.cc \
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
	ast.cc \
	parser.cc \
	global.cc \
	io.cc \
	io__file.cc \
	io__reader.cc \
	io__writer.cc \
	io__path.cc \
	engine.cc \
	main.cc
