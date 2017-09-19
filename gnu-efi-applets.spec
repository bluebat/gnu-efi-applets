%global debug_package %{nil}

Name:		gnu-efi-applets
Version:	3.0.3
Release:	1
Summary:	Building Applets using GNU EFI
License:	GPL, PD, MIT
Group:		Development/System
URL:		https://github.com/bluebat/gnu-efi-applets
Source0:	https://github.com/bluebat/gnu-efi-applets/archive/v%{version}.tar.gz#/%{name}-%{version}.tar.gz
BuildRequires:	gnu-efi-devel

%description
Some missing applets for the EFI system.

%package devel
Summary:	Development files of package gnu-efi-applets
Requires:	gnu-efi-devel
BuildArch:	noarch

%description devel
Development Environment for building GNU EFI applets.

%prep
%setup -q

%build
make

%install
make DESTDIR=%{buildroot} install

%files
%doc README.md
/boot/efi/tools/*.efi

%files devel
%{_bindir}/%{name}-build
/usr/lib/%{name}

%changelog
* Tue Sep 19 2017 Wei-Lun Chao <bluebat@member.fsf.org> - 3.0.3
- First release
