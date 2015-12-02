system = Module("system"
write = system.out.write

server = @(co, x)
	hostname = null
	sender = null
	recipients = ""
	x = co("220 SMTP pseudo server: " + x + "\n"
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
			recipients = recipients + "To: " + recipient + "\n"
			x = co("250 Recipient " + recipient + "\n"
			continue
		if x == "DATA"
			if sender === null
				x = co("503 Sender not specified\n"
			else if recipients == ""
				x = co("503 Recipients not specified\n"
			else
				mail = hostname === null ? "" : "Received: from " + hostname + "\n"
				mail = mail + "From: " + sender + "\n"
				mail = mail + recipients
				mail = mail + "\n"
				x = co("354 Enter mail, end with \".\"\n"
				while x != "."
					mail = mail + x
					mail = mail + "\n"
					x = co(""
				sender = false
				recipients = 0
				x = co("250 Accepted\n" + mail + "\n"
		return "221 SMTP pseudo server closing\n" if x == "QUIT"
		x = co("500 Command unrecognized: " + x + "\n"

coroutine = @(f)
	main = Fiber.current(
	Fiber(@(x) f(main, x

co = coroutine(server
write(co("Pseudo SMTP Server"
write(co("HELO localhost"
write(co("MAIL FROM: foo@foo.com"
write(co("RCPT TO: bar@bar.com"
write(co("RCPT TO: zot@zot.com"
write(co("DATA"
write(co("Hello everyone,"
write(co("This is shin."
write(co("Good bye."
write(co("."
write(co("QUIT"
system.out.flush(
