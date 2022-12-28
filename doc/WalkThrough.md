# Walk Through xemmai Features

## Introduction

This document walks through xemmai language.

## Basic Types

    # This is a comment.
    'this_is_a_symbol
    1 + 2 # => 3
    1.5 * 2.0 # => 3.0
    "This is a string."
    null
    true # => 1.0
    false # => null

Only `null` and `false` are falsy.
All the other values are truthy.

## Variables

    a = 1
    b = 2
    c = a + b # => 3

## Control Flows

### `if` Expression

    if a > 0
        # a is greater than zero.
        relation = 1
    else if a == 0
        # a is equal to zero.
        relation = 0
    else
        # a is less than zero.
        relation = -1

A single line form:

    if a > 0; relation = 1 else if a == 0; relation = 0 else relation = -1

### `while` Expression

    while a > 0
        a = a - 1

A single line form:

    while a > 0; a = a - 1

Using `break` and `continue` expressions:

    a = b = c = 0
    while true
        if a % 2 == 0
            b = b + a
            continue
        if a % 3 == 0
            c = c + a
            continue
        if b + c >= 100; break
        a = a + 1

### `for` Expression

    n = 0
    for i = 0; i < 10; i = i + 1
        n = n + i

A single line form:

    n = 0
    for i = 0; i < 10; i = i + 1; n = n + i

## Functions

### Simple Case

    sum = @(n)
        a = 0
        while n > 0
            a = a + n
            n = n - 1
        a
    sum(10) # => 55

Using `return` expression:

    sum2 = @(n)
        if n <= 0; return 0
        n + sum2(n - 1) # Tail return can be omitted.
    sum2(10) # => 55

An empty arguments list can be omitted:

    # Same as @() 55.
    fiftyfive = @ 55
    fiftyfive() # => 55

A single expression form:

    sum3 = @(n) n > 0 ? n + sum3(n - 1) : 0
    sum3(10) # => 55

Tail call optimization is enabled:

    sum4 = @(n, a) n > 0 ? sum4(n - 1, a + n) : a
    sum4(10, 0) # => 55

So, `sum4(1000000, 0)` does not run out of stack.

An example of high order function:

    xy = @(x)@(y) x * y
    xy(11)(5) # => 55

### Scopes

xemmai has a static scoping.
Each function is a lambda closure and has its own scope.

    n = 0
    foo = @(m)
        i = 0
        @
            :i = i + 1
            n = i # This defines a local n.
            ::n = ::n + m * n # The second n refers to the local n.

`:` denotes an outer scope, `::` denotes an outer outer scope, and so on.

Each reference is resolved from its local scope to an outer scope.
Assigning to an outer variable must be explicitly prefixed with `:`.

### Default Arguments

    multiply = @(a, b = 1) a * b
    multiply(5, 2) # => 10
    multiply(5) # => 5

The expression of a default argument is evaluated once when the lambda is defined:

    push = @(x, xs = [])
        xs.push(x)
        xs
    push(1) # => [1]
    push(2) # => [1, 2]

### Rest Arguments

    multiply = @(x, *xs)
        xs.each(@(x) :x = :x * x
        x
    multiply(2) # => 2
    multiply(2, 3) # => 6
    multiply(2, 3, 4) # => 24

### More on Function Calls

Arguments list can be written using block form:

    # Same as foo(0, 1, 2).
    foo(
        0 # , can be omitted.
        1
        2
    )

Arguments can be expanded with `*` prefix:

    xs = '(0, 1)
    ys = [3, 4]
    # Same as foo(0, 1, 2, 3, 4).
    foo(*xs, 2, *ys)

## Grouping

Expressions can be grouped by `(` and `)`:

    (0, 1, 2) # => 2

Expressions in a group are evaluated sequentially.
The value of the last expression becomes the value of the group.

An empty group is evaluated to `null`:

    () # => null

Expressions can be written using block form:

    # Same as (0, 1, 2).
    (
        0 # , can be omitted.
        1
        2
    )

## Exceptions

    foo = @
        throw Throwable("A message.")
    try
        foo()
    catch Throwable e
        e.dump()
    finally
        # Executed whenever exiting try clause.

Any object can be thrown.

## Classes

### Simple Case

    Foo = Object + @
        # instance fields
        $x
        $y
        # class fields
        $__initialize = @(x, y)
            $x = x
            $y = y
        $l2 = @ $x * $x + $y * $y
    foo = Foo(3.0, 4.0)
    foo.l2() # => 25.0

`Foo` is a new class derived from `Object` with a couple of new instance fields and a couple of new class fields.
Functions defined as class fields are methods.

`$` is a special variable.
In a class definition, `$` denotes "the class builder".
In a method definition, `$` denotes "this instance".

A class definition is also a function.
Getting a field of `$` defines a new instance field.
Setting a value to a field of `$` defines a new class field or overrides an existing class field.

`__initialize` is a special method which is called when a new instance is created.

"." between `$` and a symbol can be omitted.
So `$x` means `$.x`.

More verbose definition of `Foo`:

    Foo = Object + @()
        # $ is the class builder in this scope.
        $.x
        $.y
        $.__initialize = @(x, y)
            # $ is this instance in this scope.
            $.x = x
            $.y = y
        $.l2 = @()
            # $ is this instance in this scope.
            $.x * $.x + $.y * $.y

### Inheritance

xemmai supports a single inheritance.

    Bar = Foo + @
        $z
        $__initialize = @(x, y, z)
            Foo.__initialize[$](x, y)
            $z = z
        $l2 = @ Foo.l2[$]() + $z * $z

Each instance field must have a unique name within the scope of instance fields and class fields including its ancestors.

Each class field must have a name that does not conflict with instance fields including its ancestors.
Defining a new class field with the same name as any class field of its ancestors overrides the existing class field.

`[]` operator of method binds this instance to a method.

## Containers

### Tuples

    a = '(0, 1, 2)
    a.size() # => 3
    a[0] + a[1] + a[2] #=> 6

Tuples are immutable.

Tuple literals can be written using block form:

    # Same as '(0, 1, 2).
    '(
        0 # , can be omitted.
        1
        2
    )

### Lists

    a = [0, 1]
    a.push(2)
    a.size() # => 3
    a[0] = 3
    a[0] + a[1] + a[2] #=> 6

List literals can be written using block form:

    # Same as [0, 1, 2].
    [
        0 # , can be omitted.
        1
        2
    ]

### Maps

    a = {"one": 1, 2: "two"}
    a["three"] = 3.0
    a[4.0] = "four"
    a["one"] # => 1
    a[2] # => "two"
    a["three"] # => 3.0
    a[4.0] # => "four"
    a.remove("one")
    a.remove(2)

Map literals can be written using block form:

    # Same as {"one": 1, 2: "two"}.
    {
        "one": 1 # , can be omitted.
        2:
            "two"
    }

## Omitting Closing Braces

Closing parentheses, brackets, and braces can be omitted when it is not necessary:

    # Same as foo(0, 1, 2).
    foo(0, 1, 2
    foo(
        0
        1
        2

    # Same as '(0, 1, 2).
    '(0, 1, 2
    '(
        0
        1
        2

    # Same as [0, 1, 2].
    [0, 1, 2
    [
        0
        1
        2

    # Same as {"one": 1, 2: "two"}.
    {"one": 1, 2: "two"
    {
        "one": 1
        2: "two"

In the subsequent examples, closing parentheses, brackets, and braces are omitted as much as possible.

    call(
        "zero"
        1
        @(x)
            foo(x
            bar(x
        @(x) x * 2
        2.0

    call("zero", 1, @(x)
        foo(x
        bar(x
    , @(x) x * 2, 2.0

    literal = {
        "one": '(
            1
            2
        2: [
            1.0
            2.0
        'three: {
            'a: 0x60
            'b: 0x61
        4.0: "four"

    maps = [
        {
            'id: 1
            'name: "one"
        {
            'id: 2
            'name: "two"
        {
            'id: 3
            'name: "three"

## Modules

`$` in a module scope denotes the module builder.
Setting a value to a field of `$` exports it.

    # foo.xm
    $add = @(x, y) x + y

    foo = Module("foo"
    foo.add(1, 2 # => 3

## Threads

    a = 0
    t0 = Thread(@
        i = 0
        while i < 100
            :a = a + i
            i = i + 1
    b = 0
    t1 = Thread(@
        i = 0
        while i < 100
            :b = b + i
            i = i + 1
    t0.join(
    t1.join(

## Fibers

    fm = Fiber.current(
    f0 = Fiber(@(x)
        x = f1(x + 1
        fm(x + 1
    f1 = Fiber(@(x) f2(x + 1
    f2 = Fiber(@(x) f0(x + 1
    f0(0 # => 4
