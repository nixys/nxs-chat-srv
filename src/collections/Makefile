include config.mk

default:
	for i in `find ./src/collections/  -mindepth 2 -name "Makefile"`; \
	do \
		make -f $$i || exit 1; \
	done
