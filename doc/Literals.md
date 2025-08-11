# Literals

This page explains literals.

    literal: symbol-literal | null | boolean | integer | float | string | tuple | list | map ;

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

    tuple: '\'' '(' expandable-expressions? ')'? ;

## List Literals

    list: '[' expandable-expressions? ']'? ;

## Map Literals

    map: '{' pairs? '}'? ;
    pairs: indent? pair ((',' | indent) pair)* ;
    pair: expression ':' expression ;
