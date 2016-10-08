# Stack Usage


## Context

    [-2] = return value
    [-1] = self
    [0...v_arguments] = arguments...
    [v_arguments...v_privates] = variables...
    [v_privates...] = temporaries...


## Stack Usage by Instructions

    |: never throws
    !: always throws
    ?: might throw
       live objects within parentheses must be handled by callees.
    x: a live object
    X: live objects
    P: any primitive
    N: null
    B: boolean
    I: integer
    F: float

    JUMP
        |
        |
    BRANCH
        |x
        |
    TRY
        |
        |
    CATCH
        |x
        |
    FINALLY
        |
        |
    YRT
        |
        |
    THROW
        |x
        ?
    CLEAR
        |x
        |
    OBJECT_GET
        ?x
        |x
    OBJECT_GET_INDIRECT
        ?xx
        |x
    OBJECT_PUT
        ?xx
        |x
    OBJECT_PUT_CLEAR
        ?xx
        |
    OBJECT_PUT_INDIRECT
        ?xxx
        |x
    OBJECT_HAS
        |x
        |B
    OBJECT_HAS_INDIRECT
        |xx
        |B
    OBJECT_REMOVE
        ?x
        |x
    OBJECT_REMOVE_INDIRECT
        ?xx
        |x
    METHOD_GET
        |x
        ?
        |xx
    METHOD_BIND
        !Px
        --
        |xx
        |xx
        --
        |xx
        ?(  x)
        |xN
    GLOBAL_GET
        ?
        |x
    STACK_LET
        |x
        |x
    STACK_LET_CLEAR
        |x
        |
    STACK_GET
        |
        |x
    STACK_PUT
        |x
        |x
    STACK_PUT_CLEAR
        |x
        |
    SCOPE_GET
        |
        |x
    SCOPE_PUT
        |x
        |x
    SCOPE_PUT_CLEAR
        |x
        |
    LAMBDA
        |
        |x
    SELF
        |
        |x
    CLASS
        |x
        |x
    SUPER
        ?x
        |x
    NUL
        |
        |N
    BOOLEAN
        |
        |B
    INTEGER
        |
        |I
    FLOAT
        |
        |F
    INSTANCE
        |
        |x
    RETURN
        |x
        |N
    CALL
        |xxX
        ?( xX)
        |x
    CALL_WITH_EXPANSION
        ?xxX
        ?( xX)
        |x
    CALL_OUTER
        |  X
        ?( NX)
        |x
    GET_AT
        ! Px
        --
        ?( xx)
        |x
    SET_AT
        ! Pxx
        --
        ?( xxx)
        |x
    ##a_name##_L
        ?
        |x
    ##a_name##_V
        ?
        |x
    ##a_name##_T
        ? P
        |x
        --
        ?(x)
        |x
    ##a_name##_II
        ?
    ##a_name##_FI
        ?
    ##a_name##_LI
        ?
        |x
        --
        ?( xx)
        |x
    ##a_name##_VI
    ##a_name##_TI
    ##a_name##_IF
        ?
    ##a_name##_FF
        ?
    ##a_name##_LF
    ##a_name##_VF
    ##a_name##_TF
    ##a_name##_IL
    ##a_name##_FL
    ##a_name##_LL
    ##a_name##_VL
    ##a_name##_TL
    ##a_name##_IV
    ##a_name##_FV
    ##a_name##_LV
    ##a_name##_VV
    ##a_name##_TV
    ##a_name##_IT
    ##a_name##_FT
    ##a_name##_LT
    ##a_name##_VT
    ##a_name##_TT
        ? xx
        |x
        --
        ?( xx)
        |x
    SEND
        ! Px
        --
        ?( xx)
        |x
    CALL_TAIL
        |xxX
        |
    CALL_WITH_EXPANSION_TAIL
        ?xxX
        |
    CALL_OUTER_TAIL
        |  X
        |
    GET_AT_TAIL
        ?( xx)
        |
    SET_AT_TAIL
        ?( xxx)
        |
    ##a_name##_TAIL_L
        ?
        |
    ##a_name##_TAIL_V
        ?
        |
    ##a_name##_TAIL_T
        ?(x)
        |
    ##a_name##_TAIL_II
        ?
    ##a_name##_TAIL_FI
        ?
    ##a_name##_TAIL_LI
        ?
        |
        --
        ?( xx)
        |
    ##a_name##_TAIL_VI
    ##a_name##_TAIL_TI
    ##a_name##_TAIL_IF
        ?
    ##a_name##_TAIL_FF
        ?
    ##a_name##_TAIL_LF
    ##a_name##_TAIL_VF
    ##a_name##_TAIL_TF
    ##a_name##_TAIL_IL
    ##a_name##_TAIL_FL
    ##a_name##_TAIL_LL
    ##a_name##_TAIL_VL
    ##a_name##_TAIL_TL
    ##a_name##_TAIL_IV
    ##a_name##_TAIL_FV
    ##a_name##_TAIL_LV
    ##a_name##_TAIL_VV
    ##a_name##_TAIL_TV
    ##a_name##_TAIL_IT
    ##a_name##_TAIL_FT
    ##a_name##_TAIL_LT
    ##a_name##_TAIL_VT
    ##a_name##_TAIL_TT
        ? xx
        |
        --
        ?( xx)
        |
    SEND_TAIL
        ! Px
        --
        ?( xx)
        |
    END
        |
        |
    SAFE_POINT
        |
        |
    BREAK_POINT
        |
        |
