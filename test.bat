set TARGET=%1\
set XEMMAI=%TARGET%xemmai --verbose

%XEMMAI% test\flow.xm
pause
%XEMMAI% test\race.xm
pause
%XEMMAI% test\ring.xm
pause
%XEMMAI% test\symbol.xm
pause
%XEMMAI% test\fiber.xm
pause
%XEMMAI% test\fiber2.xm
pause
%XEMMAI% test\fiber3.xm
pause
%XEMMAI% test\thread.xm
pause
%XEMMAI% test\hanoi.xm
pause
%XEMMAI% test\hanoi2.xm
pause
%XEMMAI% test\prime.xm
pause
%XEMMAI% test\prime2.xm
pause
%XEMMAI% test\prime3.xm
pause
%XEMMAI% test\psmtp.xm
pause
%XEMMAI% test\mandelbrot.xm
pause
%XEMMAI% %TARGET%test_container.xm
pause
%XEMMAI% %TARGET%test_callback.xm
pause
