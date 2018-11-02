Name:	testlala	
Version:	1.0
Release:	1%{?dist}
Summary:	It is just a test

Group:		SA
License:	OpenSource
URL:		www.kernel.org
Source0:	%{name}-%{version}.tar.gz	

BuildRequires:	glibc-devel	
Requires:	ssh

%description
This is just a test lalalal


%prep
%setup -q


%build
make %{?_smp_mflags}


%install

%files
%doc


%changelog

