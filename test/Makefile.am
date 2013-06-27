TEST_EXTENSIONS = .xm
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/modules;
XM_LOG_COMPILER = $(top_builddir)/src/xemmai
AM_XM_LOG_FLAGS = --verbose
scripts = \
	field.xm \
	flow.xm \
	flow2.xm \
	primitives.xm \
	race.xm \
	ring.xm \
	symbol.xm \
	fiber.xm \
	fiber2.xm \
	fiber3.xm \
	thread.xm \
	threading.xm \
	tuple.xm \
	variadic.xm \
	array.xm \
	dictionary.xm \
	bytes.xm \
	file.xm \
	text.xm \
	path.xm \
	hanoi.xm \
	hanoi2.xm \
	prime.xm \
	prime2.xm \
	prime3.xm \
	psmtp.xm \
	mandelbrot.xm \
	fix.xm
TESTS = $(scripts) \
	container/test.xm \
	callback/test.xm \
	enum/test.xm
EXTRA_DIST = $(scripts) list.xm
CLEANFILES = \
	file.xm.test \
	text.xm.test

SUBDIRS = container callback enum
