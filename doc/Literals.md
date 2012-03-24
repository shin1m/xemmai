# Literals

This page explains literals.

    target: symbol-literal | null | boolean | integer | float | string | tuple | array | dictionary ;


## Symbol Literals

    symbol-literal: '\'' symbol ;


## Null Literals

    null: 'null' ;


## Boolean Literals

    boolean: 'true' | 'false' ;


## Integer Literals

    integer: digit+ ;


## Float Literals

    float: digit+ '.' digit* ;


## String Literals

    string: '"' escaped-string '"' ;


## Tuple Literals

    tuple: '\'' '(' expressions? ')' ;
    expressions: '*' expression | expression (',' expression)* (',' '*' expression)? ;


## Array Literals

    array: '[' expressions? ']' ;


## Dictionary Literals

    dictionary: '{' pairs? '}' ;
    pairs: pair (',' pair)* ;
    pair: expression ':' expression ;
