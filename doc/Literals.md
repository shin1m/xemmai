# Literals

This page explains literals.

    target: symbol-literal | null | boolean | integer | float | string | tuple | list | map ;

## Symbol Literals

    symbol-literal: '\'' symbol ;

## Null Literals

    null: 'null' ;

## Boolean Literals

    boolean: 'true' | 'false' ;

`false` is an alias for `null`.
The only falsy value is `null`. The other values are all truthy.
`true` is the value that is just truthy.

## Integer Literals

    integer: digit+ ;

## Float Literals

    float: digit+ '.' digit* ;

## String Literals

    string: '"' escaped-string '"' ;

## Tuple Literals

    tuple: '\'' '(' (indent? expressions)? ')'? ;
    expressions: '*' expression | expression ((',' | indent) expression)* ((',' | indent) '*' expression)? ;

## List Literals

    list: '[' (indent? expressions)? ']'? ;

## Map Literals

    map: '{' (indent? pairs)? '}'? ;
    pairs: pair ((',' | indent) pair)* ;
    pair: expression ':' expression ;
