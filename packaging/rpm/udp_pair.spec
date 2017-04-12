Name:           udp_pair
Version:        0.1.1
Release:        1%{?dist}
Summary:        A very simple C object that wraps the BSD socket API to provide a UDP socket pair abstraction

License:        Unlicense
URL:            https://github.com/vietjtnguyen/udp_pair
Source0:        https://github.com/vietjtnguyen/udp_pair/archive/%{version}.tar.gz

BuildRequires:  cmake

%description
A very simple C object that wraps the BSD socket API to provide a UDP socket
pair abstraction. Basically say you have two known endpoints that need to talk
to each other over UDP. You know the port you are receiving on and the
address/port of the other end point. You can define a UDP pair object that
represents your end point and simply send/receive for that set up.

%package        devel
Summary:        Development files for %{name}

%description    devel
The %{name}-devel package contains the header files for developing applications
that use %{name}.

%prep
%setup -q

%build
mkdir build
pushd build
  %cmake -DCMAKE_CXX_FLAGS="-g" ..
make %{?_smp_mflags}
popd

%install
pushd build
  %make_install
popd

%check
pushd build
  ctest -V %{?_smp_mflags}
popd

%files
%{_bindir}/udpcat
%{_libdir}/libudp_pair.so.*

%files devel
%{_includedir}/udp_pair.h
%{_libdir}/libudp_pair.so

%changelog
* Tue Apr 11 2017 Viet The Nguyen <vietjtnguyen@gmail.com> - 0.1.1-1
- Initial packaging
