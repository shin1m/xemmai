# Garbage Collection

xemmai has a modified version of David F. Bacon's [Recycler](http://www.research.ibm.com/people/d/dfb/papers/Bacon03Pure.pdf).

Below are modifications.


## Increment/Decrement Queues

Each mutator thread has its own increment/decrement queues.

They are single producer single consumer circular queues.

This makes memory synchronization between mutators and collector simple.


## Scanning Stacks

There is no special treatment for scanning stacks.

Language level stacks are scanned as part of `Fiber` object.

Other references including those on native stacks are tracked by smart pointers.


## Frequency of Cycle Collection

The cycle collection runs only when an increased number of live objects has exceeded a certain threshold.

The key point is that the list of root candidates is represented by an intrusive doubly linked list.

This avoids the overflow of the list.
And this also allows the decrement phase removing objects from the list and freeing them immediately when their reference counts have reached to zero.

Therefore, the cycle collection can be skipped as long as the number of live objects does not increase.


## Scanning Object Graphs

Scanning object graphs is done non-recursively.


## Increment Operation

Increment operation does not recursively scan blacks.

Scanning blacks on every increment operation turned out to be expensive in the implementation.

Therefore, the increment operation switched to marking just only its target object as black.
