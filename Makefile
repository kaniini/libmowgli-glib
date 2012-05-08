include extra.mk

LIB_MAJOR = 0
LIB_MINOR = 0

STATIC_LIB = libmowgli-glib.a
SHARED_LIB = ${LIB_PREFIX}mowgli-glib${LIB_SUFFIX}

DISTCLEAN = config.h

SRCS = mowgli-gpollable.c

INCLUDES = mowgli-glib.h

include buildsys.mk

CPPFLAGS += ${GLIB_CFLAGS} ${MOWGLI_CFLAGS}
LIBS += ${GLIB_LIBS} ${MOWGLI_LIBS}

install-extra:
	i="libmowgli-glib.pc"; \
	${INSTALL_STATUS}; \
	if ${MKDIR_P} ${DESTDIR}${libdir}/pkgconfig && ${INSTALL} -m 644 $$i ${DESTDIR}${libdir}/pkgconfig/$$i; then \
		${INSTALL_OK}; \
	else \
		${INSTALL_FAILED}; \
	fi

uninstall-extra:
	i="libmowgli-glib.pc"; \
	if [ -f ${DESTDIR}${libdir}/pkgconfig/$$i ]; then \
		if rm -f ${DESTDIR}${libdir}/pkgconfig/$$i; then \
			${DELETE_OK}; \
		else \
			${DELETE_FAILED}; \
		fi \
	fi
