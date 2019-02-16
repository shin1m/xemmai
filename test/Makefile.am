TEST_EXTENSIONS = .xm .xmd .xmo
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/modules;
XM_LOG_COMPILER = $(top_builddir)/src/xemmai
AM_XM_LOG_FLAGS = --verbose
XMD_LOG_COMPILER = $(srcdir)/run-xmd $(top_builddir)/src/xemmai
XMO_LOG_COMPILER = $(srcdir)/run-xmo $(top_builddir)/src/xemmai
scripts = \
	field.xm \
	flow.xm \
	flow2.xm \
	flow3.xm \
	flow4.xm \
	tail.xm \
	throw.xm \
	throw2.xm \
	throw3.xm \
	throw4.xm \
	throw5.xm \
	throw6.xm \
	throw7.xm \
	throw8.xm \
	throw9.xm \
	primitives.xm \
	operators.xm \
	indent.xm \
	errors.xm \
	call-errors.xm \
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
	dictionary2.xm \
	bytes.xm \
	file.xm \
	text.xm \
	path.xm \
	hanoi.xm \
	hanoi2.xm \
	hanoi4.xm \
	prime.xm \
	prime2.xm \
	prime3.xm \
	psmtp.xm \
	mandelbrot.xm \
	fix.xm \
	time.xm \
	totp.xm \
	dump.xmo \
	dump2.xmo \
	dump3.xmo \
	fd.xmo
TESTS = $(scripts) \
	backtrace.xmd \
	hanoi3.xmd \
	container/test.xm \
	callback/test.xm \
	callback/fiber.xm \
	callback/fiber2.xm \
	enum/test.xm
EXTRA_DIST = $(scripts) \
	list.xm \
	run-xmd \
	run-xmo \
	backtrace.xmd.in \
	backtrace.xmdi \
	backtrace.xmde \
	hanoi3.xmd.in \
	hanoi3.xmdi \
	hanoi3.xmde \
	dump.xmoe \
	dump2.xmoe \
	dump3.xmoe \
	fd.xmoe \
	test.proj
CLEANFILES = \
	file.xm.test \
	text.xm.test \
	backtrace.xmd \
	hanoi3.xmd
%.xmd: %.xmd.in
	cp $< $@

SUBDIRS = container callback enum
