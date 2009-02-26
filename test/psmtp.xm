server = @(co, x) {
	hostname = null;
	sender = null;
	recipients = "";
	x = co("220 SMTP pseudo server: " + x + "\n");
	while (true) {
		if (x.substring(0, 5) == "HELO ") {
			hostname = x.substring(5);
			x = co("250 Hello " + hostname + "\n");
			continue;
		}
		if (x.substring(0, 11) == "MAIL FROM: ") {
			if (sender === null) {
				sender = x.substring(11);
				x = co("250 Sender " + sender + "\n");
			} else {
				x = co("503 Sender already specified\n");
			}
			continue;
		}
		if (x.substring(0, 9) == "RCPT TO: ") {
			recipient = x.substring(9);
			recipients = recipients + "To: " + recipient + "\n";
			x = co("250 Recipient " + recipient + "\n");
			continue;
		}
		if (x == "DATA") {
			if (sender === null) {
				x = co("503 Sender not specified\n");
			} else if (recipients == "") {
				x = co("503 Recipients not specified\n");
			} else {
				mail = hostname === null ? "" : "Received: from " + hostname + "\n";
				mail = mail + "From: " + sender + "\n";
				mail = mail + recipients;
				mail = mail + "\n";
				x = co("354 Enter mail, end with \".\"\n");
				while (x != ".") {
					mail = mail + x;
					mail = mail + "\n";
					x = co("");
				}
				sender = false;
				recipients = 0;
				x = co("250 Accepted\n" + mail + "\n");
			}
		}
		if (x == "QUIT") return "221 SMTP pseudo server closing\n";
		x = co("500 Command unrecognized: " + x + "\n");
	}
};

coroutine = @(f) {
	main = Fiber.current();
	Fiber(@(x) {
		f(main, x);
	});
};

co = coroutine(server);
print(co("Pseudo SMTP Server"));
print(co("HELO localhost"));
print(co("MAIL FROM: foo@foo.com"));
print(co("RCPT TO: bar@bar.com"));
print(co("RCPT TO: zot@zot.com"));
print(co("DATA"));
print(co("Hello everyone,"));
print(co("This is shin."));
print(co("Good bye."));
print(co("."));
print(co("QUIT"));
