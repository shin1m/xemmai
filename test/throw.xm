system = Module("system"
print = system.error.write_line

f = @
	throw Throwable("Hello, World!"
try
	f(
catch Throwable e
	print(e
	e.dump(

(@
	try
		throw Throwable("Hello, World!"
	catch Throwable e
		print(e
		e.dump(
	finally
		print("Good bye!"
)(
