TESTS_ENVIRONMENT = $(top_builddir)/src/xemmai --verbose
scripts = \
	field.xm \
	flow.xm \
	race.xm \
	ring.xm \
	symbol.xm \
	fiber.xm \
	fiber2.xm \
	fiber3.xm \
	thread.xm \
	array.xm \
	dictionary.xm \
	bytes.xm \
	file.xm \
	hanoi.xm \
	hanoi2.xm \
	prime.xm \
	prime2.xm \
	prime3.xm \
	psmtp.xm \
	mandelbrot.xm
TESTS = $(scripts) \
	container/test.xm \
	callback/test.xm
EXTRA_DIST = $(scripts) list.xm
CLEANFILES = file.xm.test

SUBDIRS = container callback
