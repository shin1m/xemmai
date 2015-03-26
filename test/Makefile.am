TEST_EXTENSIONS = .xm .xmd .xmo
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/modules;
XM_LOG_COMPILER = $(top_builddir)/src/xemmai
AM_XM_LOG_FLAGS = --verbose
XMD_LOG_COMPILER = ./run-xmd $(top_builddir)/src/xemmai
XMO_LOG_COMPILER = ./run-xmo $(top_builddir)/src/xemmai
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
	callback/fiber.xm \
	callback/fiber2.xm \
	enum/test.xm \
	hanoi3.xmd \
	tail2.xmo
EXTRA_DIST = $(scripts) \
	list.xm \
	run-xmd \
	run-xmo \
	hanoi3.xmd \
	hanoi3.xmdi \
	hanoi3.xmde \
	tail2.xmoe \
	test.proj
CLEANFILES = \
	file.xm.test \
	text.xm.test

SUBDIRS = container callback enum
