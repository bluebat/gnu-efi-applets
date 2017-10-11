VERSION = 3.0.3
DESTDIR =
PREFIX = /usr
PACKAGE = gnu-efi-applets

SRCS = $(wildcard *.c)
APPLETS = $(SRCS:.c=.efi)

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS), 64)
  LIBDIR = $(PREFIX)/lib64
else
  LIBDIR = $(PREFIX)/lib
endif

build:
	cd efilibc; make -f ../Make.applets MAKEPATH=.. efilibc.a
	make -f Make.applets Cmp.efi LIBEXTRA=efilibc/efilibc.a
	make -f Make.applets $(APPLETS)

clean:
	cd efilibc; make -f ../Make.applets MAKEPATH=.. clean
	make -f Make.applets clean

install: build
	install -Dm755 $(PACKAGE)-build.sh $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build 
	install -d $(DESTDIR)$(LIBDIR)/$(PACKAGE)
	install -m644 Make.* $(DESTDIR)$(LIBDIR)/$(PACKAGE)
	install -d $(DESTDIR)/boot/efi/tools
	install -m755 $(APPLETS) $(DESTDIR)/boot/efi/tools
	install -m644 efilibc/efilibc.a $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(PREFIX)/include/efilibc
	install -m644 efilibc/inc/*.h $(DESTDIR)$(PREFIX)/include/efilibc
	install -d $(DESTDIR)$(PREFIX)/include/efilibc/sys
	install -m644 efilibc/inc/sys/*.h $(DESTDIR)$(PREFIX)/include/efilibc/sys

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build
	rm -rf $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	for i in $(APPLETS) rm -f $(DESTDIR)/boot/efi/tools/$$i; done
	rm -f $(DESTDIR)$(LIBDIR)/efilibc.a
	rm -rf $(DESTDIR)$(PREFIX)/include/efilibc

rpm: $(PACKAGE).spec
	rsync -aC --delete . $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION)
	tar czf $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz -C $(HOME)/rpmbuild/SOURCES $(PACKAGE)-$(VERSION)
	rpmbuild -ta $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz
