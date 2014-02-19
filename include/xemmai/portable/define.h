#ifndef XEMMAI__PORTABLE__DEFINE_H
#define XEMMAI__PORTABLE__DEFINE_H

#ifdef __GNUC__
#define XEMMAI__PORTABLE__EXPORT
#define XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
#define XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__PORTABLE__ALWAYS_INLINE __attribute__((always_inline))
#define XEMMAI__PORTABLE__FORCE_INLINE
#define XEMMAI__PORTABLE__DEFINE_EXPORT
#endif
#ifdef _MSC_VER
#define thread_local __declspec(thread)
#ifndef XEMMAI__PORTABLE__EXPORT
#define XEMMAI__PORTABLE__EXPORT __declspec(dllimport)
#endif
#define XEMMAI__PORTABLE__ALWAYS_INLINE
#define XEMMAI__PORTABLE__FORCE_INLINE __forceinline
#define XEMMAI__PORTABLE__DEFINE_EXPORT __declspec(dllexport)
#endif

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define NOMINMAX
#endif

#endif
