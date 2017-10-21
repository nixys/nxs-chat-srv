include config.mk

PROJ_DALS_SUB_HEADERS		= $(shell find ./src/dal/ra-queue/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-d-ra-queue.o: ./src/dal/ra-queue/ra-queue.c \
				./src/dal/ra-queue/ra-queue.h \
				./src/dal/ra-queue/ra-queue-typedefs.h \
				./src/dal/ra-queue/ra-queue-subdals.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_DALS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-d-ra-queue.o ./src/dal/ra-queue/ra-queue.c
