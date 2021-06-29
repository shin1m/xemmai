# Garbage Collection

xemmai has a modified version of David F. Bacon's [Recycler](http://www.research.ibm.com/people/d/dfb/papers/Bacon03Pure.pdf).

Below are modifications.

## Increment/Decrement Queues

Each mutator thread has its own increment/decrement queues.

They are single producer single consumer circular queues.

This makes memory synchronization between mutators and collector simple.

## Scanning Stacks

Stacks are scanned conservatively.

Only the pointers that differ from the last epoch are checked as candidates.

Other references are tracked by smart pointers.

## Frequency of Cycle Collection

The cycle collection runs only when an increased number of live objects has exceeded a certain threshold.

The key point is that the list of root candidates is represented as an intrusive doubly linked list.

This avoids the overflow of the list.
And this also allows the decrement phase to remove objects from the list and free them immediately when their reference counts have reached to zero.

Therefore, the cycle collection can be skipped unless the number of live objects increases.

## Scanning Object Graphs

Scanning object graphs is done non-recursively by using an intrusive linked list.

## No Scanning Blacks on Each Increment/Decrement Operation

Increment/decrement operations do not recursively scan blacks.

Scanning blacks on every increment/decrement operation turned out to be expensive in the implementation.

The increment/decrement operations are changed to mark just only their target objects as black/purple respectively.

## Refurbish

There is a case in which the reference count of an orange object is decremented to zero by the cyclic decrement, where a red object in a garbage cycle has the last reference to an orange object in the other candidate cycle.

Given the following candidate cycles:

     -> C1 -> C0 -> *
    |___|

Where
* They were found in the order of C0 and C1 by the find cycles.
* C1 has the last references to the acyclic portion of C0.

They are tested in the order of C1 and C0 by the free cycles.

If C1 passed tests and decremented reference counts in C0,
then C0 has objects with the reference count zero
which must be released even if C0 failed tests.

The refurbish is changed to buffer them as garbage.

After the free cycles is completed, the buffered garbage objects are released.
