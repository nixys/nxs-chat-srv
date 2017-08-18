include config.mk

objs/nxs-chat-srv-conf-file-ini.o: ./src/conf/file-ini/file-ini.c \
						./src/conf/file-ini/file-ini.h \
						$(PROJ_CORE_HEADERS) \
						$(PROJ_META_HEADERS) \
						$(PROJ_COLLECTOINS_HEADERS) \
						$(PROJ_DAL_HEADERS) \
						$(NXS_CORE_HEADERS)

						$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-conf-file-ini.o ./src/conf/file-ini/file-ini.c
