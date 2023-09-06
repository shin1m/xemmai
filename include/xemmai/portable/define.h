#ifndef XEMMAI__PORTABLE__DEFINE_H
#define XEMMAI__PORTABLE__DEFINE_H

#ifdef __GNUC__
#define XEMMAI__PORTABLE__THREAD __thread
#define XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__PORTABLE__ALWAYS_INLINE __attribute__((always_inline))
#define XEMMAI__PORTABLE__FORCE_INLINE
#endif
#ifdef _MSC_VER
#define XEMMAI__PORTABLE__THREAD __declspec(thread)
#define XEMMAI__PORTABLE__ALWAYS_INLINE
#define XEMMAI__PORTABLE__FORCE_INLINE __forceinline
#endif

#ifdef _WIN32
#ifndef XEMMAI__PUBLIC
#define XEMMAI__PUBLIC __declspec(dllimport)
#endif
#define XEMMAI__EXPORT __declspec(dllexport)
#define XEMMAI__LOCAL
#define NOMINMAX
#include <windows.h>
#else
#define XEMMAI__PUBLIC __attribute__((visibility("default")))
#define XEMMAI__EXPORT
#define XEMMAI__LOCAL __attribute__((visibility("hidden")))
#endif

#ifdef __unix__
#ifndef XEMMAI__SIGNAL_SUSPEND
#define XEMMAI__SIGNAL_SUSPEND SIGRTMAX - 1
#endif
#ifndef XEMMAI__SIGNAL_RESUME
#define XEMMAI__SIGNAL_RESUME SIGRTMAX
#endif
#endif

#include <system_error>

#endif
