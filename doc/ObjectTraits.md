# Object Traits

This describes several traits of builtin objects.

## Derivable or Not

Some builtin classes are not derivable.

## Sharable

Threads in xemmai are native threads.

Some objects such as lists and maps needs to be accessed exclusively.

It is expensive to lock them for every operation.

So, xemmai requires programmers some involvement to make this more efficient.

Lists/Maps are derived classes of Sharable.

Each Sharable object is in one of two states: owned and shared.

### Owned

An object in this state is owned by a certain thread.

Which means only the owning thread can operate on it and it is more efficient than in the shared state.

Other threads can not operate on it, or doing so throws an exception.

This is the default state.

### Shared

An object in this state is shared accross threads.

Which means any thread can operate on it.

Instead, it is more expensive than in the owned state because it is locked for each operation.

### Switching to the Other State

`Sharable.share` switches to the object to the shared state.

Only the owning thread of the object can call it.

`Sharable.own` does the opposite.

The calling thread becomes the owning thread.

It can be called only if the object is in the shared state which means no thread owns the object.

## Table of Builtin Classes

    Name       Derivable  Implementation Notes
    Object     yes        *2
    Class      no         *1
    Module     no         *1
    Fiber      no         *1
    Thread     no         *1
    Symbol     no         *1
    Native     no         *1
    Scope      no         *1
    Code       no         *1
    Lambda     no         *1
    Method     no         *1
    Throwable  yes        *3
    Null       no         *1
    Boolean    no         *1
    Integer    yes        *4
    Float      yes        *4
    String     no         *1
    Tuple      no         *1
    Sharable   no         *1
      List     yes        *2
      Map      yes        *2
    Bytes      no         *1
    Lexer      no         *1
    Parser     no         *1
    io.File    yes        *3
    io.Reader  yes        *3
    io.Writer  yes        *3
    io.Path    yes        *3

* \*1 It overrides `f_instantiate` to skip normal instantiation process because it is not derivable.
* \*2 It overrides `f_construct` and manually constructs its native part.
* \*3 It overrides `f_construct` and uses `t_construct` to construct its native part.
* \*4 It overrides `f_construct` and uses `t_construct_with` to construct its native part.
