system = Module("system"
print = system.out.write_line
threading = Module("threading"
assert = @(x) x || throw Throwable("Assertion failed."

Foo = Object + @ $message

mutex = threading.Mutex(
condition = threading.Condition(
mutex.acquire(
try
	done = false
	object = Foo(
	thread = Thread(@
		mutex.acquire(
		try
			print(object.message
			assert(object.message == "Hello."
			condition.signal(
			condition.wait(mutex
			object.message = "Good bye."
		finally
			:done = true
			condition.signal(
			mutex.release(
	object.message = "Hello."
	condition.wait(mutex
	assert(!done
	condition.signal(
	condition.wait(mutex
	assert(done
	print(object.message
	assert(object.message == "Good bye."
finally
	thread.join(
	mutex.release(
