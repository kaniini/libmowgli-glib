include ../extra.mk

LIB_MAJOR = 0
LIB_MINOR = 0

PROG = test-gpollable

SRCS = test-gpollable.c

include ../buildsys.mk

CPPFLAGS += ${GLIB_CFLAGS} ${MOWGLI_CFLAGS} -I..
LIBS += ${GLIB_LIBS} ${MOWGLI_LIBS} -L.. -lmowgli-glib
