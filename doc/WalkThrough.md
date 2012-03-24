# Walk Through xemmai Features

## Introduction

This document walks through xemmai language.


## Basic Types

    # This is a comment.
    'this_is_a_symbol;
    1 + 2; # => 3
    1.5 * 2.0; # => 3.0
    "This is a string.";
    null;
    true;
    false;


## Variables

    a = 1;
    b = 2;
    c = a + b; # => 3


## Control Flows

### `if` Statement

    if (a > 0) {
        # a is greater than zero.
        greater = true;
    } else {
        # a is less or equal to zero.
        greater = false;
    }

Braces are optional for a single statement:

    if (a > 0)
        greater = true;
    else
        greater = false;


### `while` Statement

    while (a > 0) {
        a = a - 1;
    }

Braces are optional for a single statement:

    while (a > 0) a = a - 1;

Using `break` and `continue` statements:

    a = b = c = 0;
    while (true) {
        if (a % 2 == 0) {
            b = b + a;
            continue;
        }
        if (a % 3 == 0) {
            c = c + a;
            continue;
        }
        if (b + c >= 100) break;
        a = a + 1;
    }


### `for` Statement

    n = 0;
    for (i = 0; i < 10; i = i + 1) {
        n = n + i;
    }

Braces are optional for a single statement:

    n = 0;
    for (i = 0; i < 10; i = i + 1) n = n + i;


## Functions

### Simple Case

    sum = @(n) {
        a = 0;
        while (n > 0) {
            a = a + n;
            n = n - 1;
        }
        a;
    };
    sum(10); # => 55

Using `return` statement:

    sum2 = @(n) {
        if (n <= 0) return 0;
        n + sum2(n - 1); # Tail return can be omitted.
    };
    sum2(10); # => 55

An empty arguments list can be omitted:

    fiftyfive = @{ 55; }; # Same as @() { 55; };
    fiftyfive(); # => 55;


### Scopes

xemmai has a static scoping.
Each function is a lambda closure and has its own scope.

    n = 0;
    foo = @(m) {
        i = 0;
        @{
            :i = i + 1;
            n = i; # This defines a local n.
            ::n = ::n + m * n; # The second n refers to the local n.
        };
    };

`:` denotes an outer scope, `::` denotes an outer outer scope, and so on.

Each reference is resolved from its local scope to an outer scope.
Assigning to an outer variable must be explicitly prefixed with `:`.


## Exceptions

    foo = @{
        throw Throwable("A message.");
    };
    try {
        foo();
    } catch (Throwable e) {
        e.dump();
    } finally {
        # Executed whenever exiting try clause.
    }

Any object can be thrown.


## Classes

### Simple Case

Using an instance as a record:

    Foo = Class();
    foo = Foo();
    foo.x = 3.0;
    foo.y = 4.0;
    foo.x * foo.x + foo.y * foo.y; # => 25.0

More object oriented way:

    Foo = Class() :: @{
        $__initialize = @(x, y) {
            $x = x;
            $y = y;
        };
        $l2 = @{
            $x * $x + $y * $y;
        };
    };
    foo = Foo(3.0, 4.0);
    foo.l2(); # => 25.0

Now, `Foo` has a couple of methods.
`__initialize` is a special method which is called when a new instance is created.

`$` is a special variable.
In a class definition, `$` denotes "this class".
In a method definition, `$` denotes "this instance".

"." between `$` and a symbol can be omitted.
So `$x` means `$.x`.

A class definition is also a function.

More verbose definition of `Foo`:

    Foo = Class() :: @() {
        # $ is this class in this scope.
        $.__initialize = @(x, y) {
            # $ is this instance in this scope.
            $.x = x;
            $.y = y;
        };
        $.l2 = @() {
            # $ is this instance in this scope.
            $.x * $.x + $.y * $.y;
        };
    };

### Inheritance

xemmai supports a single inheritance.

    Bar = Class(Foo) :: @{
        # $ is Bar.
        # $^ is Foo.
        $__initialize = @(x, y, z) {
            # :$ is Bar.
            # :$^ is Foo.
            :$^__initialize[$](x, y);
            $z = z;
        };
        $l2 = @{
            :$^l2[$]() + $z * $z;
        };
    };

`^` denotes a super class.
So `:$^` in an instance method refers to the super class of its defining class.

`[]` operator of method binds this instance to a method.


## Containers

### Tuples

    a = '(0, 1, 2);
    a.size(); # => 3;
    a[0] + a[1] + a[2]; #=> 6

Tuples are immutable.


### Arrays

    a = [0, 1];
    a.push(2);
    a.size(); # => 3;
    a[0] = 3;
    a[0] + a[1] + a[2]; #=> 6


### Dictionaries

    d = {"one": 1, 2: "two"};
    d["three"] = 3.0;
    d[4.0] = "four";
    d["one"]; # => 1
    d[2]; # => "two"
    d["three"]; # => 3.0
    d[4.0]; # => "four"
    d.remove("one");
    d.remove(2);


## Modules

`$` in a module scope denotes this module.
So objects can be exported by assigning to a field of a module.

    # foo.xm
    $add = @(x, y) {
        x + y;
    };

    foo = Module("foo");
    foo.add(1, 2); # => 3


## Threads

    a = 0;
    t0 = Thread(@{
        i = 0;
        while (i < 100) {
            :a = a + i;
            i = i + 1;
        }
    });
    b = 0;
    t1 = Thread(@{
        i = 0;
        while (i < 100) {
            :b = b + i;
            i = i + 1;
        }
    });
    t0.join();
    t1.join();


## Fibers

    f0 = f1 = f2 = null;
    fm = Fiber.current();
    f0 = Fiber(@(x) {
        x = f1(x + 1);
        fm(x + 1);
    });
    f1 = Fiber(@(x) {
        f2(x + 1);
    });
    f2 = Fiber(@(x) {
        f0(x + 1);
    });
    f0(0); # => 4