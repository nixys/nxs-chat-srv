include config.mk

PROJ_UNITS_SUB_HEADERS		= $(shell find ./src/units/tlgrm-sendmessage/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-u-tlgrm-sendmessage.o: ./src/units/tlgrm-sendmessage/tlgrm-sendmessage.c \
				./src/units/tlgrm-sendmessage/tlgrm-sendmessage.h \
				./src/units/tlgrm-sendmessage/tlgrm-sendmessage-typedefs.h \
				./src/units/tlgrm-sendmessage/tlgrm-sendmessage-subunits.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(PROJ_DAL_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-u-tlgrm-sendmessage.o ./src/units/tlgrm-sendmessage/tlgrm-sendmessage.c
