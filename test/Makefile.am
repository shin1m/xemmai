TEST_EXTENSIONS = .xm .xmd
if AUTOMAKE_OLDER_THAN_1_13
TESTS_ENVIRONMENT = XEMMAI_MODULE_PATH=$(top_builddir)/modules $(top_builddir)/src/xemmai --verbose
else
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/modules;
XM_LOG_COMPILER = $(top_builddir)/src/xemmai
AM_XM_LOG_FLAGS = --verbose
XMD_LOG_COMPILER = ./run-xmd $(top_builddir)/src/xemmai
endif
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
	fix.xm \
	tail.xm \
	time.xm
TESTS = $(scripts) \
	container/test.xm \
	callback/test.xm \
	enum/test.xm \
	hanoi3.xmd
EXTRA_DIST = $(scripts) \
	list.xm \
	run-xmd \
	hanoi3.xmd \
	hanoi3.xmdi \
	hanoi3.xmde \
	test.proj
CLEANFILES = \
	file.xm.test \
	text.xm.test

SUBDIRS = container callback enum
