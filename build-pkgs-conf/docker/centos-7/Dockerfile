FROM centos:7

ADD packages.nixys.ru.repo /etc/yum.repos.d/packages.nixys.ru.repo
RUN rpm --import http://packages.nixys.ru/centos/repository.gpg.key && \
	yum install -y nxs-fw-libs-1.13 libhiredis-vip libhiredis-vip-devel redhat-lsb
RUN rpmdev-setuptree
