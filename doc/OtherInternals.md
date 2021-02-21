# Other Internals

## Fat Values

Object references in xemmai are represented as fat values in C++:

    class t_value
    {
        t_object* v_p;
        union
        {
            bool v_boolean;
            intptr_t v_integer;
            double v_float;
        };
    };

Note that `v_p` is a tag and of `t_object*`, and its usage is tricky:

 * `0` means null.
 * `1` means boolean and `v_boolean` has its value.
 * `2` means integer and `v_integer` has its value.
 * `3` means float and `v_float` has its value.
 * Values greater than `3` means object reference and `v_p` **itself** is its value.

This is because the garbage collector runs on another thread and each object reference must be examined **atomically**.

Typical tag/value pair requires several steps to find out that it is an object reference and retrieve its value.

This is not sufficient with the concurrent garbage collector.

Using fat values reduces redundant increment/decrement operations.

This is desirable because increment/decrement operations are relatively expensive.

## Transferable Values (Obsoleted)

`t_transfer` is a derived class of `t_value`, and it also is a tricky stuff.

This implements move semantics for transferring object references without C++11 features.

Whenever an argument of `const t_transfer&` is explicitly/implicitly passed, the content of the argument is transferred and the argument is clearred as null.

Using move semantics reduces redundant increment/decrement operations.

This is desirable because increment/decrement operations are relatively expensive.

## Moving Values

As xemmai is migrating to C++11, the tricky `t_transfer` has been obsoleted.

Now, xemmai utilizes C++11 move semantics for `t_value` as much as possible.

Using move semantics reduces redundant increment/decrement operations.

This is desirable because increment/decrement operations are relatively expensive.

## Minimal Locks for Objects

Objects in the shared thread model are protected from simultaneous accesses from threads using locks they contain.

xemmai only provides a minimal lock mechanism which is a kind of spin locks without system calls.

Note that this is not a general purpose synchronization mechanism but just only for avoiding an engine crash.

Use standard synchronization mechanisms such as mutexes and condition variables to implement application logics.

## Hidden Structures

Almost every object in xemmai except for several immutable objects can have arbitrary fields as key/value pairs.

The so-called hidden structures or hidden classes is used to represent this dictionary structure.

Some of field accesses are optimized a little by utilizing this.

## Computed Goto

If available, the computed goto is used to optimize bytecode execution.

Actually the bytecode is a machine word code which means each code has 64-bit size on 64 bits system for example.

During compilation to bytecode, each instruction code is replaced with the address of the corresponding label.

And during execution of the bytecode, stepping to the next instruction is done by jumping to the value of the next instruction.

## Operand Based Instructions

The bytecode interpreter in xemmai is not stack based.

It is rather close to register based one.

For example, binary instructions have a couple of operands which are offsets from the base of the current stack frame.

These offsets can be determined during compilation to bytecode.

Operand based instructions tend to copy less object references than stack based ones do.

This is desirable because increment/decrement operations are relatively expensive.

## Thread Local Field Cache

Because threads in xemmai are preemptive, each thread has its own field cache in order to avoid expensive synchronizations.

Instead, acquire/release operations for the field cache are required in order to make changes to object visible to other threads.

This is modeled from native memory barriers for SMP.

As long as standard synchronization mechanisms such as mutexes and condition variables are used, these operations are called implicitly as appropriate.

## Inline Field Cache

In conjunction with the hidden structure, getting/putting object field are optimized using the inline field cache.

Getting object field has 3 states: initial, monomorphic, and megamorphic.

Putting object field has 4 states: initial, monomorphic add, monomorphic set, and megamorphic.

The first time a code is executed, it is in the initial state.

The second time the code is executed, the code is rewritten to move into the monomorphic state if the object is owned by a thread, otherwise the megamorphic state.

When the code is in the monomorphic state, if the object is owned by a thread and has the same hidden structure as previous, the field access is optimized as accessing an array element by the index.

Otherwise, the code is rewritten to move into the megamorphic.

When the code is in the megamorphic state, the field access is done in a normal way.
