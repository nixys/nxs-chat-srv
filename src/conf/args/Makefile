include config.mk

objs/nxs-chat-srv-conf-args.o: ./src/conf/args/args.c \
					./src/conf/args/args.h \
					$(PROJ_CORE_HEADERS) \
					$(PROJ_META_HEADERS) \
					$(PROJ_COLLECTOINS_HEADERS) \
					$(PROJ_DAL_HEADERS) \
					$(NXS_CORE_HEADERS)

					$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/args.o ./src/conf/args/args.c
