# xemmai [![Build Status](https://secure.travis-ci.org/shin1m/xemmai.png)](http://travis-ci.org/shin1m/xemmai)

xemmai is a dynamic programming language.
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

 * [Walk Through](/shin1m/xemmai/blob/master/doc/WalkThrough.md)
 * [Expressions and Statements](/shin1m/xemmai/blob/master/doc/ExpressionsAndStatements.md)
 * [Object Traits](/shin1m/xemmai/blob/master/doc/ObjectTraits.md)


## Implementation Details

 * [Garbage Collection](/shin1m/xemmai/blob/master/doc/GarbageCollection.md)
 * [Other Internals](/shin1m/xemmai/blob/master/doc/OtherInternals.md)


## License

The MIT License (MIT)

Copyright (c) 2008-2012 Shin-ichi MORITA

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
