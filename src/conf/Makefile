include config.mk

PROJ_CONF_SUB_HEADERS	= $(shell find ./src/conf/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

default: objs/nxs-chat-srv-conf.o

	for i in `find ./src/conf/  -mindepth 2 -name "Makefile"`; \
	do \
		make -f $$i || exit 1; \
	done

objs/nxs-chat-srv-conf.o: ./src/conf/conf.c \
					./src/conf/conf.h \
					$(PROJ_CORE_HEADERS) \
					$(PROJ_META_HEADERS) \
					$(PROJ_CONF_SUB_HEADERS) \
					$(PROJ_COLLECTOINS_HEADERS) \
					$(PROJ_DAL_HEADERS) \
					$(NXS_CORE_HEADERS)

					$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-conf.o ./src/conf/conf.c
