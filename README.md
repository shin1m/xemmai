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


## Implementation Details

 * [Garbage Collection](/shin1m/xemmai/blob/master/doc/GarbageCollection.md)
 * [Other Internals](/shin1m/xemmai/blob/master/doc/OtherInternals.md)
