# Literals

This page explains literals.

    literal: symbol-literal | null | boolean | integer | float | string | tuple | list | map ;

## Symbol Literals

    symbol-literal: '\'' symbol ;

## Null Literals

    null: 'null' ;

## Boolean Literals

    boolean: 'true' | 'false' ;

xemmai has no boolean type.
The only falsy value is `null`. The other values are all truthy.
`true` is an alias for `1.0`, which is float not integer `1` because bitwise operators have to distinguish integer operations and boolean operations.
`false` is an alias for `null`.

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
