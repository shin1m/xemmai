#ifndef XEMMAI__MACRO_H
#define XEMMAI__MACRO_H

#define XEMMAI__MACRO__CONCATENATE_(x, y) x##y
#define XEMMAI__MACRO__CONCATENATE(x, y) XEMMAI__MACRO__CONCATENATE_(x, y)

#define XEMMAI__MACRO__L_(n) L##n
#define XEMMAI__MACRO__L(n) XEMMAI__MACRO__L_(n)

#define XEMMAI__MACRO__LQ__(n) L##n
#define XEMMAI__MACRO__LQ_(n) XEMMAI__MACRO__LQ__(#n)
#define XEMMAI__MACRO__LQ(n) XEMMAI__MACRO__LQ_(n)

#endif
