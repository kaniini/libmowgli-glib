AC_INIT([libmowgli-glib], [0.1], [nenolod@dereferenced.org])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PATH_TOOL(AR, ar)
PKG_CHECK_MODULES(MOWGLI, [libmowgli-2])
PKG_CHECK_MODULES(GLIB, [glib-2.0 gobject-2.0])
BUILDSYS_SHARED_LIB
BUILDSYS_INIT
BUILDSYS_TOUCH_DEPS
AC_CONFIG_FILES([buildsys.mk extra.mk libmowgli-glib.pc])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
