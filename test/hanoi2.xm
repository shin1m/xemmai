tower = @(a_height) {
	a = [];
	while (a_height > 0) {
		a.unshift(a_height);
		a_height = a_height - 1;
	}
	a;
};

hanoi = @(a_height, a_from, a_via, a_to, a_move) {
	if (a_height > 1) hanoi(a_height - 1, a_from, a_to, a_via, a_move);
	a_move(a_from, a_to);
	if (a_height > 1) hanoi(a_height - 1, a_via, a_from, a_to, a_move);
};

n = 5;
towers = [tower(n), tower(0), tower(0)];
print(towers);
print("\n");
hanoi(n, towers[0], towers[1], towers[2], @(a_from, a_to) {
	a_to.unshift(a_from.shift());
	print(towers);
	print("\n");
});
