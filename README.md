# xemmai [![Build Status](https://travis-ci.org/shin1m/xemmai.svg?branch=master)](https://travis-ci.org/shin1m/xemmai)

xemmai is a dynamic programming language.
Its syntax is based on the off-side rule.
Closing parentheses, brackets, and braces can be omitted where they are not necessary in conjunction with indentation.

Here is how it looks like:

    system = Module("system"
    print = system.out.write_line

    hanoi = @(towers, move)
        step = @(height, from, via, to)
            height > 1 && step(height - 1, from, to, via
            move(from, to
            height > 1 && step(height - 1, via, from, to
        step(towers[0].size(), towers[0], towers[1], towers[2]

    towers = '(
        [1, 2, 3
        [
        [
    print(towers
    hanoi(towers, @(from, to)
        to.unshift(from.shift(
        print(towers

Which shows:

    '([1, 2, 3], [], [])
    '([2, 3], [], [1])
    '([3], [2], [1])
    '([3], [1, 2], [])
    '([], [1, 2], [3])
    '([1], [2], [3])
    '([1], [], [2, 3])
    '([], [], [1, 2, 3])

It has lambda closures, classes, exceptions, modules, threads, and fibers.

It is implemented in C++ and has C++ friendly API to implement extension modules.
It aims to be a general purpose glue language.

It runs on linux, windows, and hopefully other unix systems.

Internally, codes are executed on a bytecode interpreter.
It also has a concurrent garbage collector to utilize multiple processors.


## How to Build

### On Linux:

    autoreconf -is
    ./configure
    make
    make install

### On Windows:

    msbuild xemmai.sln


## How to Run Tests

### On Linux:

    make check

### On Windows:

    msbuild test/test.proj


## Documentation

 * [Walk Through](doc/WalkThrough.md)
 * [Expressions](doc/Expressions.md)
 * [Object Traits](doc/ObjectTraits.md)


## Implementation Details

 * [Garbage Collection](doc/GarbageCollection.md)
 * [Other Internals](doc/OtherInternals.md)


## License

The MIT License (MIT)

Copyright (c) 2008-2019 Shin-ichi MORITA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
