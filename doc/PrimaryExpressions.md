# Primary Expressions

This page explains primary expressions.

    primary: variable
        | self symbol?
        | literal
        | '(' expression ')'
        | lambda
        | break
        | continue
        | return
        | throw
        | getter
        | tester
        | remover
        | subscription
        | call
        ;


## Variable References

    variable: ':'* symbol ;


## Self Expressions

    self: ':'* '$' ('@' | '^')* ;


## Literals

See [Literals](Literals.md).


## Lambda Expressions

    lambda: '@' ('(' arguments? ')')? body ;
    arguments: '*' symbol
        | symbol ((',' symbol)* | '=' expression) (',' symbol '=' expression)* (',' '*' symbol)?
        ;
    body: expression | (indent expression+)? ;


## The `break` Expressions

    break: 'break' expression? ;


## The `continue` Expressions

    continue: 'continue' ;


## The `return` Expressions

    return: 'return' expression? ;


## The `throw` Expressions

    throw: 'throw' expression ;


## Getter Expressions

    getter: primary '.' ('@' | '^' | symbol | '(' expression ')') ;


## Tester Expressions

    tester: primary '.' '?' (symbol | '(' expression ')') ;


## Remover Expressions

    remover: primary '.' '~' (symbol | '(' expression ')') ;


## Subscription Expressions

    subscription: primary '[' expression ']' ;


## Call Expressions

    call: primary '(' (indent? expressions)? ')'? ;
