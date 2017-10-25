CC						=	cc
CFLAGS						=	-pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror -g -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2
CFLAGS32					=
LINK						=	$(CC)

ifeq ($(shell getconf LONG_BIT),32)
CFLAGS32					=	-D_FILE_OFFSET_BITS=64
endif

NXS_FW_VERSION					=	1.11

NXS_FW_CORE_INCLUDES_PATH			=	/usr/include/nxs-fw-libs
NXS_FW_CORE_LIBS_PATH				=	/usr/lib/nxs-fw-libs

USE_NXS_MODULES					=	-D USE_NXS_JSON -D USE_NXS_CURL -D USE_NXS_MYSQL -D USE_NXS_REST_API 

NXS_CORE_INCLUDES				=	-I $(NXS_FW_CORE_INCLUDES_PATH)/$(NXS_FW_VERSION)

PROJ_INCLUDES					=	-I ./src

NXS_CORE_HEADERS				=	$(shell find $(NXS_FW_CORE_INCLUDES_PATH)/$(NXS_FW_VERSION) -type f -name "*.h")
NXS_CORE_LINKS					=	-L$(NXS_FW_CORE_LIBS_PATH)/$(NXS_FW_VERSION) -lnxs-general -lnxs-cfg-json -lnxs-json -lnxs-curl -lnxs-mysql -lnxs-rest-api 

NXS_INCLUDES					=	-I/usr/include/libevent-nxs 
NXS_LINKS					=	-ljansson -lcurl -L/usr/lib/mysql -lmysqlclient -L/usr/lib/x86_64-linux-gnu/libevent-nxs -levent -lssl -lcrypto -levent_openssl 

NXS_USER_INCLUDES				=	
NXS_USER_LINKS					=	-lhiredis

PROJ_CORE_HEADERS				=	./src/nxs-chat-srv-core.h

PROJ_UNITS_HEADERS				=	./src/nxs-chat-srv-units.h \
								$(shell find ./src/units/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

PROJ_COLLECTIONS_HEADERS			=	./src/nxs-chat-srv-collections.h \
								$(shell find ./src/collections/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

PROJ_DAL_HEADERS				=	./src/nxs-chat-srv-dal.h \
								$(shell find ./src/dal/ -mindepth 2 -maxdepth 2 -type f -name "*.h")

PROJ_META_HEADERS				=	$(shell find ./src/meta/ -type f -name "*.h")

PROJ_CONF_HEADERS				=	./src/nxs-chat-srv-conf.h

PROJ_PROC_HEADERS				=	./src/nxs-chat-srv-proc.h \
								$(shell find ./src/proc/ -type f -name "*.h")
