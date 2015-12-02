system = Module("system"
write = system.out.write
print = system.out.write_line
math = Module("math"
sqrt = math.sqrt

mandelbrot = @(cr, ci)
	limit = 95
	r = i = 0.0
	for n = 0; n < limit && sqrt(r * r + i * i) < 10.0; n = n + 1
		wr = r * r - i * i + cr
		wi = r * i + i * r + ci
		r = wr
		i = wi
	n

draw_mandelbrot = @(r0, i0, r1, i1, delta)
	for i = i0; i > i1; i = i - delta
		write("|"
		for r = r0; r < r1; r = r + delta
			c = 127 - mandelbrot(r, i
			write(String.from_code(c
		print("|"

draw_mandelbrot(-2.0, 1.0, 1.0, -1.0, 0.04
