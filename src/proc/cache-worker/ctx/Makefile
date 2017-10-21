include config.mk

objs/nxs-chat-srv-p-cache-worker-ctx.o: ./src/proc/cache-worker/ctx/cache-worker-ctx.c \
				./src/proc/cache-worker/ctx/cache-worker-ctx.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_HEADERS) \
				$(PROJ_COLLECTOINS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-p-cache-worker-ctx.o ./src/proc/cache-worker/ctx/cache-worker-ctx.c
