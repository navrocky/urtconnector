%define name urtconnector
%define version 0.7.1

Summary: Advanced Urban Terror launcher
Name: %{name}
Version: %{version}
Group: Amusements/Games
Prefix: /usr
Packager: Navrocky Vladislav
Source: urtconnector.tar.gz
BuildRoot: /var/tmp/%{name}-%{version}-buildroot
Release: 1
#AutoReq: off
License: GPL
BuildRequires: cmake boost-devel libqt4-devel gcc gcc-c++
%if %{defined fedora}
BuildRequires: sqlite-devel
%else
BuildRequires: sqlite3-devel
%endif
Requires: libqt4 >= 4.3.0 qstat >= 2 sqlite3
%if %{defined fedora}
Requires: sqlite
%else
Requires: sqlite3
%endif


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
%defattr(-, root, root) 
/usr/bin/*
/usr/share/icons/urtconnector.png
/usr/share/icons/hicolor
/usr/share/icons/hicolor/48x48
/usr/share/icons/hicolor/48x48/apps
/usr/share/icons/hicolor/48x48/apps/urtconnector.png
/usr/share/pixmaps/*
/usr/share/applications/*
/usr/share/urtconnector
/usr/share/urtconnector/*