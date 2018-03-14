FROM debian:9

RUN apt-get update \
	&& apt-get -y install gnupg \
	&& apt-key adv --fetch-keys http://packages.nixys.ru/packages.nixys.ru.gpg.key \
	&& echo "deb [arch=amd64] http://packages.nixys.ru/debian/ stretch main" > /etc/apt/sources.list.d/packages.nixys.ru.list \
	&& apt-get update \
	&& apt-get -y install nxs-chat-srv systemd

COPY docker-entrypoint.sh /

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/usr/sbin/nxs-chat-srv", "-c", "/etc/nxs-chat-srv/nxs-chat-srv.conf"]
