system = Module("system"
print = system.out.write_line
threading = Module("threading"
assert = @(x)
	throw Throwable("Assertion failed." if !x

mutex = threading.Mutex(
condition = threading.Condition(
mutex.acquire(
try
	done = false
	object = Object(
	thread = Thread(@
		mutex.acquire(
		try
			object.own(
			print(object.message
			assert(object.message == "Hello."
			condition.signal(
			condition.wait(mutex
			object.message = "Good bye."
			object.share(
		finally
			:done = true
			condition.signal(
			mutex.release(
	object.message = "Hello."
	object.share(
	condition.wait(mutex
	assert(!done
	try
		object.own(
	catch Throwable t
	print(t
	assert(t !== null
	t.dump(
	condition.signal(
	condition.wait(mutex
	assert(done
	object.own(
	print(object.message
	assert(object.message == "Good bye."
finally
	thread.join(
	mutex.release(
