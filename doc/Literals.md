# Literals

This page explains literals.

    target: symbol-literal | null | boolean | integer | float | string | tuple | list | map ;

## Symbol Literals

    symbol-literal: '\'' symbol ;

## Null Literals

    null: 'null' ;

## Boolean Literals

    boolean: 'true' | 'false' ;

`null` and `false` are falsy. The other values are all truthy.

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
