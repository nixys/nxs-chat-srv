FROM debian:9 as build-env

RUN apt-get update; apt-get install -y gnupg && \
        apt-key adv --fetch-keys http://packages.nixys.ru/debian/repository.gpg.key && \
        echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list && \
        apt-get update; apt-get install -y nxs-fw-libs-1.14 libhiredis-vip libhiredis-vip-dev

WORKDIR /usr/src/nxs-project
ADD ./ /usr/src/nxs-project

RUN make

FROM debian:9

RUN apt-get update \
	&& apt-get -y install gnupg \
	&& apt-key adv --fetch-keys http://packages.nixys.ru/debian/repository.gpg.key \
	&& echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list \
	&& apt-get update \
	&& apt-get -y install python3-pip python3-yaml libevent-nxs libssl1.0.2 libjansson4 libcurl3 libmariadbclient18 \
	&& pip3 install jinja2-cli

## Set LOCALE to UTF8
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get install -y locales
RUN echo "ru_RU.UTF-8 UTF-8" > /etc/locale.gen && \
	locale-gen ru_RU.UTF-8 && \
	dpkg-reconfigure locales && \
	/usr/sbin/update-locale LANG=ru_RU.UTF-8

ENV LC_ALL ru_RU.UTF-8

RUN sed -i 's/mozilla\/DST_Root_CA_X3.crt/\!mozilla\/DST_Root_CA_X3.crt/g' /etc/ca-certificates.conf; update-ca-certificates

# Copy binary from `build-env`
COPY --from=build-env /usr/src/nxs-project/objs/nxs-chat-srv /

# Add config template
COPY deploy/conf/nxs-chat-srv.conf.j2 /

RUN mkdir /labels
COPY deploy/conf/labels/en.json /labels
COPY deploy/conf/labels/ru.json /labels

COPY deploy/docker/app/docker-entrypoint.sh /

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/nxs-chat-srv", "-c", "/nxs-chat-srv.conf"]
