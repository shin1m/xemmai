# Term Expressions

This page explains term expressions.

## Primary Expressions

See [PrimaryExpressions](PrimaryExpressions.md).

## Unary Expressions

    unary: ('+' | '-' | '!' | '~')* primary ;

## Multiplicative Expressions

    multiplicative: unary (('*' | '/' | '%') unary)* ;

## Additive Expressions

    additive: multiplicative (('+' | '-') multiplicative)* ;

## Shift Expressions

    shift: additive (('<<' | '>>') additive)* ;

## Relational Expressions

    relational: shift (('<' | '>' | '<=' | '>=') shift)* ;

## Equality Expressions

    equality: relational (('==' | '!=') relational)* ;

## And Expressions

    and: equality ('&' equality)* ;

## Xor Expressions

    xor: and ('^' and)* ;

## Or Expressions

    or: xor ('|' xor)* ;

## And Also Expressions

    and_also: or ('&&' or)* ;

## Or Else Expressions

    or_else: and_also ('||' and_also)* ;

## Extension Expressions

    extension: or_else ('::' or_else)* ;

## Conditional Expressions

    conditional: extension ('?' conditional ':' conditional)? ;
