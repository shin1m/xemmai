system = Module("system");
print = system.out.write_line;

symbol = @{
	print(Symbol("Hello, World!!"));
	print('Good.__string() + " " + 'bye + ", " + 'World + "!!");
};

symbols = @(a_n) {
	if (a_n > 0) symbols(a_n - 1);
	symbol();
};

symbols(100);
