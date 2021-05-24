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

## Inline Field Index Cache

Since all the fields are settled on class definition and classes are immutable, structures of instances never change throughout their life time.

With that in mind, the index of the last accessed field is cached at each site of field accesses.

The cached index is used to shortcut searching the field at the same site.
