include config.mk

PROJ_PROCS_SUB_HEADERS		= $(shell find ./src/proc/rest-api/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-p-rest-api.o: ./src/proc/rest-api/rest-api.c \
				./src/proc/rest-api/rest-api.h \
				./src/proc/rest-api/rest-api-subproc.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_HEADERS) \
				$(PROJ_COLLECTOINS_HEADERS) \
				$(PROJ_PROC_SUB_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-p-rest-api.o ./src/proc/rest-api/rest-api.c
