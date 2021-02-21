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

    tuple: '\'' '(' (indent? expressions)? ')'? ;
    expressions: '*' expression | expression ((',' | indent) expression)* ((',' | indent) '*' expression)? ;

## Array Literals

    array: '[' (indent? expressions)? ']'? ;

## Dictionary Literals

    dictionary: '{' (indent? pairs)? '}'? ;
    pairs: pair ((',' | indent) pair)* ;
    pair: expression ':' expression ;
