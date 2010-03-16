%define name urtconnector
%define version 0.2.0

Summary: Advanced Urban Terror launcher
Name: %{name}
Version: %{version}
Group: Amusements/Games
Prefix: /usr
Packager: Navrocky Vladislav
Source: urtconnector.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-buildroot
Release: 1
AutoReq: off
License: GPL
Requires: qt >= 4.3.0, cmake >= 2.6, qstat >= 2.6

%description

Advanced UrbanTerror launcher program. Developed by members of russian clans =Xaoc=, Red*Army and Rus Devils Team.
This program uses Qt4 and is written in C++.

%prep

%setup -q -n urtconnector

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=$RPM_BUILD_ROOT/usr ..
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT

cd build
make install/strip

%preun
%post
%postun

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/bin/*
