include config.mk

PROJ_DALS_SUB_HEADERS		= $(shell find ./src/dal/db-waitings/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-d-db-waitings.o: ./src/dal/db-waitings/db-waitings.c \
				./src/dal/db-waitings/db-waitings.h \
				./src/dal/db-waitings/db-waitings-typedefs.h \
				./src/dal/db-waitings/db-waitings-subdals.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_DALS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-d-db-waitings.o ./src/dal/db-waitings/db-waitings.c
