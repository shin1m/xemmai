system = Module("system"

server = @(co, log)
	hostname = null
	sender = null
	recipients = ""
	x = co("220 SMTP pseudo server\n"
	while true
		if x.substring(0, 5) == "HELO "
			hostname = x.substring(5
			x = co("250 Hello " + hostname + "\n"
			continue
		if x.substring(0, 11) == "MAIL FROM: "
			if sender === null
				sender = x.substring(11
				x = co("250 Sender " + sender + "\n"
			else
				x = co("503 Sender already specified\n"
			continue
		if x.substring(0, 9) == "RCPT TO: "
			recipient = x.substring(9
			recipients = recipients + "\tTo: " + recipient + "\n"
			x = co("250 Recipient " + recipient + "\n"
			continue
		if x == "DATA"
			if sender === null
				x = co("503 Sender not specified\n"
			else if recipients == ""
				x = co("503 Recipients not specified\n"
			else
				mail = hostname === null ? "" : "\tReceived: from " + hostname + "\n"
				mail = mail + "\tFrom: " + sender + "\n"
				mail = mail + recipients
				mail = mail + "\t\n"
				x = co("354 Enter mail, end with \".\"\n"
				while x != "."
					mail = mail + "\t" + x + "\n"
					x = co(null
				sender = false
				recipients = 0
				x = co("250 Accepted\n"
				log("queued:"
				log(mail
			continue
		x == "QUIT" && return "221 SMTP pseudo server closing\n"
		x = co("500 Command unrecognized: " + x + "\n"

if system.arguments.size() > 0
	log = system.error.write_line
	log("start: " + system.arguments[0]
	read = @
		x = system.in.read_line(
		x == "" ? throw Throwable("unexpected EOF") : x.substring(0, x.size() - 1
	write = @(x)
		system.out.write(x
		system.out.flush(
	write(server(
		@(x)
			x !== null && write(x
			read(
		log
	log("done."
else
	coroutine = @(f)
		main = Fiber.current(
		Fiber(@(x) f(main, x
	co = coroutine(server
	log = ""
	assert = @(x, y)
		system.out.write(x
		system.out.flush(
		x == y || throw Throwable("Assertion failed."
	assert(
		co(@(x) :log = log + x + "\n"
		"220 SMTP pseudo server\n"
	assert(
		co("HELO localhost"
		"250 Hello localhost\n"
	assert(
		co("MAIL FROM: foo@foo.com"
		"250 Sender foo@foo.com\n"
	assert(
		co("RCPT TO: bar@bar.com"
		"250 Recipient bar@bar.com\n"
	assert(
		co("RCPT TO: zot@zot.com"
		"250 Recipient zot@zot.com\n"
	assert(
		co("DATA"
		"354 Enter mail, end with \".\"\n"
	co("Hello everyone,"
	co("This is shin."
	co("Good bye."
	assert(
		co("."
		"250 Accepted\n"
	assert(
		co("QUIT"
		"221 SMTP pseudo server closing\n"
	assert(log, "queued:\n\tReceived: from localhost\n\tFrom: foo@foo.com\n\tTo: bar@bar.com\n\tTo: zot@zot.com\n\t\n\tHello everyone,\n\tThis is shin.\n\tGood bye.\n\n"
