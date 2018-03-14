#!/bin/bash

P_NAME=$1
VER=$2

(
	cd /tmp/build
	
	chown -R root: /tmp/build
	
	tar zcfv ${P_NAME}-${VER}.tar.gz ${P_NAME}-${VER}
	
	rpmbuild --define "_rpmdir /tmp/build/" --define "_srcrpmdir /tmp/build/" --define "_sourcedir /tmp/build/" --target=x86_64 -ba /tmp/build/${P_NAME}.spec
	
	cp /tmp/build/x86_64/* /tmp/build/
	
	rm -rf /tmp/build/x86_64/
	
	rm ${P_NAME}-${VER}.tar.gz
)
