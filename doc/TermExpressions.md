# Term Expressions

This page explains term expressions.

## Primary Expressions

See [PrimaryExpressions](PrimaryExpressions.md).

## Unary Expressions

    unary: ('+' | '-' | '!' | '~')* primary ;

Negate operator (`!`) returns `true` if the operand is falsy, returns `false` otherwise.

## Multiplicative Expressions

    multiplicative: unary (('*' | '/' | '%') unary)* ;

## Additive Expressions

    additive: multiplicative (('+' | '-') multiplicative)* ;

## Shift Expressions

    shift: additive (('<<' | '>>') additive)* ;

## Relational Expressions

    relational: shift (('<' | '>' | '<=' | '>=') shift)* ;

Relational, equality, and identity operators return either `true` or `false`.

## Equality Expressions

    equality: relational (('==' | '!=') relational)* ;

## Identity Expressions

    identity: equality (('===' | '!==') equality)* ;

## And Expressions

    and: identity ('&' identity)* ;

Bitwise operators (`&`, `^`, and `|`) do bitwise operations if both the operands are integers, do boolean operations if the left operand is boolean, throws an exception otherwise.

## Xor Expressions

    xor: and ('^' and)* ;

## Or Expressions

    or: xor ('|' xor)* ;

## And Also Expressions

    and_also: or ('&&' or)* ;

Evaluates the left operand, returns it if it is falsy, evaluates the right operand and returns it otherwise.

## Or Else Expressions

    or_else: and_also ('||' and_also)* ;

Evaluates the left operand, returns it if it is truthy, evaluates the right operand and returns it otherwise.

## Conditional Expressions

    conditional: or_else ('?' conditional ':' conditional)? ;
