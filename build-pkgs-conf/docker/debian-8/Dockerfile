FROM debian:8

RUN apt-key adv --fetch-keys http://packages.nixys.ru/debian/repository.gpg.key && \
	echo "deb [arch=amd64] http://packages.nixys.ru/debian/ jessie main" > /etc/apt/sources.list.d/packages.nixys.ru.list && \
	apt-get update; apt-get install -y nxs-fw-libs-1.14 libhiredis-vip libhiredis-vip-dev

COPY docker-entrypoint.sh /

ENTRYPOINT ["/docker-entrypoint.sh"]
