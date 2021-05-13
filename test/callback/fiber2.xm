system = Module("system"
callback = Module("callback"
print = system.out.write_line

fm = Fiber.current(

Derived = callback.Client + @
	$on_message = @(message)
		print("fc: " + message
		fm("Good bye."

server = callback.Server(
server.add(Derived(

fc = Fiber(@(x) server.post(x

print("fm: " + fc("Hello."
