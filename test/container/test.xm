system = Module("system"
container = Module("container"
print = system.out.write_line

queue = container.Queue("this is a message"
print(queue

print("push"
queue.push(0
queue.push("one"
queue.push(2
queue.push("three"
print(queue

print("pop"
print(queue.pop(
print(queue.pop(
print(queue

print(queue.pop(
print(queue.pop(
print(queue

try
	print(queue.pop(
catch Throwable e
	print(e
	e.dump(
