include config.mk

PROJ_DALS_SUB_HEADERS		= $(shell find ./src/dal/rdmn-issues/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-d-rdmn-issues.o: ./src/dal/rdmn-issues/rdmn-issues.c \
				./src/dal/rdmn-issues/rdmn-issues.h \
				./src/dal/rdmn-issues/rdmn-issues-typedefs.h \
				./src/dal/rdmn-issues/rdmn-issues-subdals.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_DALS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-d-rdmn-issues.o ./src/dal/rdmn-issues/rdmn-issues.c
