FROM debian:8

RUN apt-key adv --fetch-keys http://packages.nixys.ru/packages.nixys.ru.gpg.key

RUN echo "deb [arch=amd64] http://packages.nixys.ru/debian/ jessie main" > /etc/apt/sources.list.d/packages.nixys.ru.list

RUN apt-get update; apt-get install -y nxs-fw-libs-1.12 libhiredis-vip libhiredis-vip-dev
