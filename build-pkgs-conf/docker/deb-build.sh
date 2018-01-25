#!/bin/bash

P_NAME=$1
VER=$2

TPL_PATH="/tmp/build/deb-tpl"

(
	cd /tmp/build/${P_NAME}

	dh_make -p ${P_NAME}_${VER} --copyright gpl3 --single --templates ${TPL_PATH} --yes --createorig

	dpkg-buildpackage
)
