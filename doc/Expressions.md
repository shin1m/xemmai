# Expressions

This page explains expressions.

## Expressions

    expression: term | assignment | compound ;

### Term Expressions

See [TermExpressions](TermExpressions.md).

### Assignment Expressions

    assignment: (variable | self symbol | primary ('.' (symbol | '(' expression ')') | subscription)) '=' expression ;

### Compound Expressions

    compound: if | while | for | try ;

### The `if` Expression

    if: 'if' options body ('else' 'if' options body)* ('else' body)? ;

### The `while` Expression

    while: 'while' options body ;

### The `for` Expression

    for: 'for' options options options body ;

### Options

    options: expressions? (?newline | ';') ;

### The `try` Expression

    try: 'try' body (catch+ finally? | finally) ;
    catch: 'catch' expression symbol body ;
    finally: 'finally' body ;

## Bodies and Expressions

    body: expression | (indent expressions)* ;
    expressions: expression (',' expression)* ;

## Module Definitions

    module: expression* ;
