%define name urtconnector
%define version 0.2.0

Summary: Advanced Urban Terror launcher
Name: %{name}
Version: %{version}
Group: Amusements/Games
Prefix: /usr
Packager: Navrocky Vladislaw
Source: urtconnector.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-buildroot
Release: 1
AutoReq: off
License: GPL
Requires: qt >= 4.3.0

%description

Advanced UrbanTerror launcher program. Developed by members of two best russian clans Red*Army and Rus Devils Team.
This program uses Qt4 and is written in C++.

%prep

%setup -q -n urtconnector

%build
qmake
make
strip bin/*

%install
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

make INSTALL_ROOT=$RPM_BUILD_ROOT install

%preun
%post
%postun

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/bin/*
