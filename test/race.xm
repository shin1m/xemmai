system = Module("system"
print = system.out.write_line

counter = 0

hello = @(name, n)
	while n > 0
		n = n - 1
		i = :counter
		:counter = i + 1
		print(name + ": " + i
	print("done."

good_bye = @(name, n)
	return @
		hello(name, n

threads = [
for i = 0; i < 20; i = i + 1
	threads.push(Thread(good_bye("thread " + i, (i % 3 + 1) * 10
threads.each(@(x) x.join(

print("counter: " + counter
