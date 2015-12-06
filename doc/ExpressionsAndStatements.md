# Expressions and Statements

This page explains expressions and statements.


## Expressions

    expression: term | assignment | compound ;


### Term Expressions

See [TermExpressions](TermExpressions.md).


### Assignment Expressions

    assignment: (variable | self symbol | primary ('.' (symbol | '(' expression ')') | subscription)) '=' expression ;


### Compound Expressions

    compound: if | while | try ;


### The `if` Expression

    if: 'if' expression block ('else' 'if' expression block)* ('else' block)? ;


### The `while` Expression

    while: 'while' expression block ;


### The `for` Expression

    for: 'for' for-expressions? ';' expression? ';' for-expressions? block ;
    for-expressions: expression (',' expression)* ;


### The `try` Expression

    try: 'try' block (catch+ finally? | finally) ;
    catch: 'catch' expression symbol block ;
    finally: 'finally' block ;


## Blocks

    block: ':' statement | (indent statement+)? ;


## Statements

    statement: (term | assignment | compound | break | continue | return | throw) newline ;


### The `break` Statement

    break: 'break' expression? ;


### The `continue` Statement

    continue: 'continue' ;


### The `return` Statement

    return: 'return' expression? ;


### The `throw` Statement

    throw: 'throw' expression ;


## Module Definitions

    module: statement* ;
