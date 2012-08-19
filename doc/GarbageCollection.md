# Garbage Collection

xemmai has a modified version of David F. Bacon's [Recycler](http://www.research.ibm.com/people/d/dfb/papers/Bacon03Pure.pdf).

Below are modifications.


## Increment/Decrement Queues

Each mutator thread has its own increment/decrement queues.

This is in order to avoid heavy synchronization on queueing references across threads.

Increment/Decrement queues are fixed size ring buffers.


## Memory Synchronization

Memory synchronization of each queue is done by context switching of native threads.

The queueing operation itself does not emit any memory barrier instruction and only expects that the compiler does not reorder the instructions of the operation.

Instead, it is assumed that context switching involves memory barriers and memory orders are preserved between before and after context switching.

On memory synchronization, all synchronizers wake up all together, not in order.


## Scanning Stacks

There is no special treatment for scanning stacks.

Language level stacks are scanned as part of `Fiber` object.

Other references including those on native stacks are tracked by smart pointers.


## Frequency of Cycle Collection

Cycle collection only runs every certain epochs.

The list of root candidates is represented by an intrusive single linked list.

So the overflow of the list does not have to be cared about even if cycle collections are skipped.


## Increment Operation

Increment operation does not recursively scan blacks.

Scanning blacks on every increment operation turned out to be expensive in the implementation.

So the increment operation switched to marking just only its target object as black.
