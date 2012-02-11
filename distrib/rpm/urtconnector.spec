%define name urtconnector
%define version 0.8.0

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
BuildRequires: cmake gcc-c++ boost-devel phonon-devel
%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires: qt-devel sqlite-devel
%else
BuildRequires: libqt4-devel sqlite3-devel
%endif
Requires: qstat >= 2.12


%description

Advanced UrbanTerror launcher and server browser program. Developed by members of russian clans =Xaoc=, Red*Army and Rus Devils Team.

%prep

%setup -q -n urtconnector

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=$RPM_BUILD_ROOT/usr ..
make %{?_smp_mflags}

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
