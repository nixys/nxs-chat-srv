FROM debian:9

RUN apt-get update \
	&& apt-get -y install gnupg \
	&& apt-key adv --fetch-keys http://packages.nixys.ru/debian/repository.gpg.key \
	&& echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list \
	&& apt-get update \
	&& apt-get -y install systemd python3-pip python3-yaml \
	&& pip3 install jinja2-cli

RUN apt-get -y install nxs-chat-srv=0.3.1-deb9u1

## Set LOCALE to UTF8
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get install -y locales
RUN echo "ru_RU.UTF-8 UTF-8" > /etc/locale.gen && \
	locale-gen ru_RU.UTF-8 && \
	dpkg-reconfigure locales && \
	/usr/sbin/update-locale LANG=ru_RU.UTF-8

ENV LC_ALL ru_RU.UTF-8

COPY docker-entrypoint.sh /

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/usr/sbin/nxs-chat-srv", "-c", "/etc/nxs-chat-srv/nxs-chat-srv.conf"]
