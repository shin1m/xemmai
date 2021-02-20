AC_INIT([xemmai], [202102])
AM_INIT_AUTOMAKE([foreign nostdinc dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([src/main.cc])

DESCRIPTION="A dynamic programming language"
AC_SUBST([DESCRIPTION])

if test "${CXXFLAGS+set}" != set; then
	CXXFLAGS=
fi
AC_PROG_CXX
AC_PROG_RANLIB

AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING([--enable-debug], [turn on debugging]),
	[case "${enableval}" in
	  yes) debug=true ;;
	  no) debug=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	 esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AC_ARG_ENABLE(
	[profile],
	AS_HELP_STRING([--enable-profile], [turn on profiling]),
	[case "${enableval}" in
	  yes) profile=true ;;
	  no) profile=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
	 esac],
	[profile=false]
)
AM_CONDITIONAL([PROFILE], [test x$profile = xtrue])

AC_CONFIG_HEADERS([include/xemmai/portable/configure.h])
AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	include/Makefile
	pkgconfig/Makefile
	pkgconfig/xemmai-${VERSION}.pc:pkgconfig/xemmai.pc.in
	pkgconfig/xemmai-${VERSION}-uninstalled.pc:pkgconfig/xemmai-uninstalled.pc.in
	src/Makefile
	modules/Makefile
	test/Makefile
	test/container/Makefile
	test/callback/Makefile
	test/enum/Makefile
])
AC_OUTPUT
