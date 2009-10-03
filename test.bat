set TARGET=%1\
set XEMMAI=%TARGET%xemmai --verbose

for %%i in (
	field.xm
	flow.xm
	race.xm
	ring.xm
	symbol.xm
	fiber.xm
	fiber2.xm
	fiber3.xm
	thread.xm
	array.xm
	dictionary.xm
	bytes.xm
	file.xm
	text.xm
	hanoi.xm
	hanoi2.xm
	prime.xm
	prime2.xm
	prime3.xm
	psmtp.xm
	mandelbrot.xm
) do (
	%XEMMAI% test\%%i
	if errorlevel 1 goto :eof
)
for %%i in (
	test_container.xm
	test_callback.xm
) do (
	%XEMMAI% %TARGET%%%i
	if errorlevel 1 goto :eof
)

echo Passed all tests.
