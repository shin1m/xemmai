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

    if: if-phrase block? ('else' if-phrase block?)* ('else' block?)? ;
    if-phrase: 'if' expression ;


### The `while` Expression

    while: while-phrase block? ;
    while-phrase: 'while' expression ;


### The `for` Expression

    for: for-phrase block? ;
    for-phrase: 'for' for-expressions? ';' expression? ';' for-expressions? ;
    for-expressions: expression (',' expression)* ;


### The `try` Expression

    try: 'try' block? (catch+ finally? | finally) ;
    catch: 'catch' expression symbol block? ;
    finally: 'finally' block? ;


## Blocks

    block: indent statement+ ;


## Statements

    statement: (term | assignment | compound | break | continue | return | throw) (if-phrase | while-phrase | for-phrase)* newline ;


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
