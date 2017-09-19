include config.mk

PROJ_DALS_SUB_HEADERS		= $(shell find ./src/dal/db-issues/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-d-db-issues.o: ./src/dal/db-issues/db-issues.c \
				./src/dal/db-issues/db-issues.h \
				./src/dal/db-issues/db-issues-typedefs.h \
				./src/dal/db-issues/db-issues-subdals.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_DALS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-d-db-issues.o ./src/dal/db-issues/db-issues.c
