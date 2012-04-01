include extra.mk

LIB_MAJOR = 0
LIB_MINOR = 0

SHARED_LIB = libmowgli-glib.a
STATIC_LIB = ${LIB_PREFIX}mowgli-glib${LIB_SUFFIX}

DISTCLEAN = config.h

SRCS = mowgli-gpollable.c

INCLUDES = mowgli-glib.h

include buildsys.mk

CPPFLAGS += ${GLIB_CFLAGS} ${MOWGLI_CFLAGS}
LIBS += ${GLIB_LIBS} ${MOWGLI_LIBS}
