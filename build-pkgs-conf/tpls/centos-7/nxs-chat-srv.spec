Name:           nxs-chat-srv
Version:        $$VERSION$$
Release:        $$BUILD_VERSION$$%{?dist}
Summary:        Nixys chat bot that integrates Telegram and Redmine

Group:          Applications/Communications
License:        GPL3
URL:            https://nixys.ru
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  nxs-fw-libs-1.14 libhiredis-vip libhiredis-vip-devel redhat-lsb
Requires:       libevent-nxs openssl-libs jansson libcurl mariadb-libs


%description
 Nixys chat bot that integrates Telegram and Redmine


%prep
%setup -q


%post
ldconfig
%systemd_post nxs-chat-srv.service


%postun
%systemd_preun nxs-chat-srv.service
rm -rf /var/log/nxs-chat-srv/
rm -rf /var/spool/nxs-chat-srv/


%build
make


%install
mkdir -p $RPM_BUILD_ROOT/usr/sbin
mkdir -p $RPM_BUILD_ROOT/etc/nxs-chat-srv/ssl
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d
mkdir -p $RPM_BUILD_ROOT/lib/systemd/system
mkdir -p $RPM_BUILD_ROOT/usr/share
mkdir -p $RPM_BUILD_ROOT/var/log/nxs-chat-srv
mkdir -p $RPM_BUILD_ROOT/var/spool/nxs-chat-srv/tlgrm
mkdir -p $RPM_BUILD_ROOT/var/spool/nxs-chat-srv/rdmn
cp ./objs/nxs-chat-srv $RPM_BUILD_ROOT/usr/sbin
cp -r ./pkg/general/etc/nxs-chat-srv $RPM_BUILD_ROOT/etc
cp ./pkg/general/etc/logrotate.d/nxs-chat-srv $RPM_BUILD_ROOT/etc/logrotate.d
cp ./pkg/general/lib/systemd/system/nxs-chat-srv.service $RPM_BUILD_ROOT/lib/systemd/system
cp -r ./pkg/general/usr/share/nxs-chat-srv $RPM_BUILD_ROOT/usr/share


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
/usr/sbin/nxs-chat-srv
%dir %attr(0750, root, root) /etc/nxs-chat-srv
%dir %attr(0750, root, root) /etc/nxs-chat-srv/ssl
%attr(0640, root, root) /etc/nxs-chat-srv/nxs-chat-srv.conf
%dir %attr(0750, root, root) /var/log/nxs-chat-srv
%dir %attr(0750, root, root) /var/spool/nxs-chat-srv
/etc/logrotate.d/nxs-chat-srv
/lib/systemd/system/nxs-chat-srv.service
/usr/share/nxs-chat-srv


%changelog
