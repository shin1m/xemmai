# Object Traits

This describes several traits of builtin objects.

## Derivable or Not

Some builtin classes are not derivable.

## Immutable or Not

Some builtin objects are immutable.

Which means the fields of them can not be modified.

Thread model below is not applicable to immutable objects.

## Thread Model

Threads in xemmai are native threads.

It is expensive to syncronize all the accesses to object fields.

So, xemmai requires programmers some involvement to make this more efficient.

Each mutable object in xemmai is in one of two thread models: shared and owned.

### Shared

Objects in this thread model are shared accross threads.

Which means the fields of them can be freely accessed by any thread.

Instead, it is more expensive than in the owned thread model.

### Owned

Objects in this thread model are owned by a certain thread.

Which means the fields of them can be accessed only by the owning thread and it is more efficient than in the shared thread model.

Other threads can not access the fields, or doing so throws an exception.

### Switching to the Other Thread Model

Except for ones marked as Fixed is yes in the table below, mutable objects can switch to the other thread model.

`Object.own` switches the object to owned thread model.

The calling thread becomes the owning thread.

It can be called only if the object is in the shared thread model which means no thread owns the object.

`Object.share` does the opposite.

Only the owning thread of the object can call it.

## Table of Builtin Classes

    Name        Derivable  Fixed  Default    Implementation Notes
    Class       no         yes    shared     *1
    Structure   no         -      shared     *1
    Module      no         no     shared     *1
    Fiber       no         no     owned      *1
    Thread      no         yes    shared     *1
    Symbol      no         yes    immutable  *1
    Scope       no         -      shared     *1
    Code        no         -      shared     *1
    Method      no         yes    immutable  *1
    Lambda      no         no     shared     *1
    Native      no         no     shared     *1
    Null        no         yes    immutable  *1
    Boolean     no         yes    immutable  *1
    Lexer       no         no     owned      *1
    Parser      no         no     owned      *1
    Object      yes        no     owned      *2
    Tuple       yes        yes    immutable  *2
    Array       yes        no     owned      *2
    Dictionary  yes        no     owned      *2
    Bytes       yes        no     owned      *2
    Throwable   yes        no     owned      *3
    String      yes        yes    immutable  *3
    io.File     yes        no     owned      *3
    io.Reader   yes        no     owned      *3
    io.Writer   yes        no     owned      *3
    io.Path     yes        yes    immutable  *3
    Integer     yes        yes    immutable  *4
    Float       yes        yes    immutable  *4

* \*1 It overrides `f_instantiate` to skip normal instantiation process because it is not derivable.
* \*2 It overrides `f_construct` and manually constructs its native part.
* \*3 It overrides `f_construct` and uses `t_construct` to construct its native part.
* \*4 It overrides `f_construct` and uses `t_construct_with` to construct its native part.
