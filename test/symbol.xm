symbol = @{
	print(Symbol("Hello, World!!"));
	print("\nGood bye, World!!\n");
};

symbols = @(a_n) {
	if (a_n > 0) symbols(a_n - 1);
	symbol();
};

symbols(100);
