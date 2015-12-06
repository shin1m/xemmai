system = Module("system"
print = system.out.write_line
assert = @(x) if !x: throw Throwable("Assertion failed."

i = 0
while true
	try
		if i > 1
			i = 3
			break
		if i > 0: throw 2
		i = 1
	catch Float e
		throw Throwable("never reach here."
	catch Object e
		assert(e.@ === Integer
		assert(e == 2
		i = e
	finally
		print(i
assert(i == 3

(@
	try
		throw null
	catch Null e
		assert(e === null
)(

assert((if true
) === null
assert((if false
	true
) === null
assert((if false
	true
else
) === null
assert((while false
) === null
assert((while true
	break
) === null
assert((while true
	break true
) === true
assert((for ; false;
) === null
assert((for ;;
	break
) === null
assert((for ;;
	break true
) === true

assert((try
finally
) === null
assert((try
	if true
finally
) === null
assert((try
	if false: true
finally
) === null
assert((try
	if false
		true
	else
finally
) === null
assert((try
	while false
finally
) === null
assert((try
	while true: break
finally
) === null
assert((try
	while true: break true
finally
) === true
assert((try
	for ; false;
finally
) === null
assert((try
	for ;;: break
finally
) === null
assert((try
	for ;;: break true
finally
) === true

assert((try
	throw null
catch Null e
) === null
assert((try
	throw null
catch Null e
	if true
) === null
assert((try
	throw null
catch Null e
	if false: true
) === null
assert((try
	throw null
catch Null e
	if false
		true
	else
) === null
assert((try
	throw null
catch Null e
	while false
) === null
assert((try
	throw null
catch Null e
	while true: break
) === null
assert((try
	throw null
catch Null e
	while true: break true
) === true
assert((try
	throw null
catch Null e
	for ; false;
) === null
assert((try
	throw null
catch Null e
	for ;;: break
) === null
assert((try
	throw null
catch Null e
	for ;;: break true
) === true
