include config.mk

PROJ_UNITS_SUB_HEADERS		= $(shell find ./src/units/tlgrm-editmessagetext/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

objs/nxs-chat-srv-u-tlgrm-editmessagetext.o: ./src/units/tlgrm-editmessagetext/tlgrm-editmessagetext.c \
				./src/units/tlgrm-editmessagetext/tlgrm-editmessagetext.h \
				./src/units/tlgrm-editmessagetext/tlgrm-editmessagetext-typedefs.h \
				./src/units/tlgrm-editmessagetext/tlgrm-editmessagetext-subunits.h \
				$(PROJ_CORE_HEADERS) \
				$(PROJ_META_HEADERS) \
				$(PROJ_UNITS_SUB_HEADERS) \
				$(PROJ_COLLECTIONS_HEADERS) \
				$(PROJ_DAL_HEADERS) \
				$(NXS_CORE_HEADERS)

				$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) $(PROJ_INCLUDES) $(NXS_INCLUDES) $(USE_NXS_MODULES) -o objs/nxs-chat-srv-u-tlgrm-editmessagetext.o ./src/units/tlgrm-editmessagetext/tlgrm-editmessagetext.c
