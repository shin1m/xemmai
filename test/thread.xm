hello = @(a_name, a_n) {
	if (a_n > 0) {
		print("Hello, " + a_name + ": " + a_n + "\n");
		hello(a_name, a_n - 1);
		print("Good bye, " + a_name + ": " + a_n + "\n");
	}
};

good_bye = @(a_name, a_n) {
	return @{
		hello(a_name, a_n);
	};
};

foo = Thread(good_bye("foo", 30));
bar = Thread(good_bye("bar", 20));
zot = Thread(good_bye("zot", 10));

foo.join();
bar.join();
zot.join();
