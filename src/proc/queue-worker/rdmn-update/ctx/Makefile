include config.mk

objs/nxs-chat-srv-p-queue-worker-rdmn-update-ctx.o: ./src/proc/queue-worker/rdmn-update/ctx/rdmn-update-ctx.c \
				./src/proc/queue-worker/rdmn-update/ctx/rdmn-update-ctx.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_HEADERS) \
				$(PROJ_COLLECTOINS_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-p-queue-worker-rdmn-update-ctx.o ./src/proc/queue-worker/rdmn-update/ctx/rdmn-update-ctx.c
