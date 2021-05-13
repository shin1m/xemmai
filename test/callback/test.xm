system = Module("system"
callback = Module("callback"
write = system.out.write

Derived = callback.Client + @
	$on_message = @(message)
		write("Derived.on_message(\n\t"
		callback.Client.on_message[$](message
		write(")\n"

client0 = callback.Client(
client1 = Derived(

client0.on_message("Hello, client0."
client1.on_message("Hello, client1."

server = callback.Server(
server.add(client0
server.add(client1
#server.run(
server.post("Hello, server."
server.post("This is shin."
server.post("Good bye."
