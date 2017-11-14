include config.mk

PROJ_UNITS_SUB_HEADERS		= $(shell find ./src/units/tlgrm-chat/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-u-tlgrm-chat.o: ./src/units/tlgrm-chat/tlgrm-chat.c \
				./src/units/tlgrm-chat/tlgrm-chat.h \
				./src/units/tlgrm-chat/tlgrm-chat-typedefs.h \
				./src/units/tlgrm-chat/tlgrm-chat-subunits.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(PROJ_DAL_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-u-tlgrm-chat.o ./src/units/tlgrm-chat/tlgrm-chat.c
