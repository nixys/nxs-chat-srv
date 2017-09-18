include config.mk

PROJ_PROCS_SUB_HEADERS		= $(shell find ./src/proc/queue-worker/rdmn-update/win-issue-updated/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-p-queue-worker-rdmn-update-win-issue-updated.o: ./src/proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated.c \
				./src/proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated.h \
				./src/proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated-subproc.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_HEADERS) \
				$(PROJ_COLLECTOINS_HEADERS) \
				$(PROJ_PROC_SUB_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-p-queue-worker-rdmn-update-win-issue-updated.o ./src/proc/queue-worker/rdmn-update/win-issue-updated/win-issue-updated.c
