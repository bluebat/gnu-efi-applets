%global debug_package %{nil}

Name:		gnu-efi-applets
Version:	3.0.11
Release:	9
Summary:	Building Applets using GNU-EFI
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

%description devel
Development Environment for building GNU-EFI applets.

%prep
%setup -q

%build
make

%install
make DESTDIR=%{buildroot} install

%files
%doc README.md efilibc/README.efilibc
/boot/efi/tools/*.efi

%files devel
%{_bindir}/%{name}-build
%{_libdir}/%{name}
%{_libdir}/efilibc.a
%{_includedir}/efilibc

%changelog
* Sun May 15 2022 Wei-Lun Chao <bluebat@member.fsf.org> - 3.0.11
- Rebuild for Fedora
