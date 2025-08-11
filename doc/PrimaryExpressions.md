# Primary Expressions

This page explains primary expressions.

    primary: variable
        | self symbol?
        | literal
        | '(' (indent? expression ((indent | ',') expression)*)? ')'?
        | lambda
        | break
        | continue
        | return
        | throw
        | getter
        | tester
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

## The `break` Expressions

    break: 'break' expression? ;

## The `continue` Expressions

    continue: 'continue' ;

## The `return` Expressions

    return: 'return' expression? ;

## The `throw` Expressions

    throw: 'throw' expression ;

## Getter Expressions

    getter: primary '.' ('@' | '^' | symbol | '(' expression ')'?) ;

## Tester Expressions

    tester: primary '.' '?' (symbol | '(' expression ')'?) ;

## Subscription Expressions

    subscription: primary '[' expression ']'? ;

## Call Expressions

    call: primary '(' expandable-expressions? ')'? ;

## Expandable Expressions

    expandable-expressions: indent? '*'? expression ((',' | indent) '*'? expression)* ;
