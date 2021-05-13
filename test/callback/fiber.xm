system = Module("system"
callback = Module("callback"
print = system.out.write_line

fm = Fiber.current(
fc = Fiber(@(x)
	print("server: " + x
	print("server: " + fm("Hello."
	print("server: " + fm("This is client."
	"Good bye."

Derived = callback.Client + @
	$on_message = @(message) print("client: " + fc(message

client = Derived(

server = callback.Server(
server.add(client
server.post("Hello."
server.post("This is server."
server.post("Good bye."
