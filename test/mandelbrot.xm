system = Module("system");
write = system.out.write;
print = system.out.write_line;

mandelbrot = @(cr, ci) {
	limit = 95;
	n = 0;
	r = i = 0.0;
	while (n < limit && sqrt(r * r + i * i) < 10.0) {
		wr = r * r - i * i + cr;
		wi = r * i + i * r + ci;
		r = wr;
		i = wi;
		n = n + 1;
	}
	n;
};

draw_mandelbrot = @(r0, i0, r1, i1, delta) {
	i = i0;
	while (i > i1) {
		write("|");
		r = r0;
		while (r < r1) {
			c = 127 - mandelbrot(r, i);
			write(String.from_code(c));
			r = r + delta;
		}
		print("|");
		i = i - delta;
	}
};

draw_mandelbrot(-2.0, 1.0, 1.0, -1.0, 0.04);
