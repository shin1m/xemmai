# Primary Expressions

This page explains primary expressions.

    primary: variable
        | self symbol?
        | literal
        | '(' expression ')'
        | lambda
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

    lambda: '@' ('(' arguments? ')')? (indent statement+)?
        | '@' '(' arguments? ')' expression
        ;
    arguments: '*' symbol
        | symbol ((',' symbol)* | '=' expression) (',' symbol '=' expression)* (',' '*' symbol)?
        ;


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
