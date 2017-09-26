VERSION = 3.0.3
DESTDIR =
PREFIX = /usr
PACKAGE = gnu-efi-applets
APPLETS = Banner.efi Color.efi Inkey.efi Input.efi SetA.efi StallForKey.efi

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS), 64)
  LIBDIR = $(PREFIX)/lib64
else
  LIBDIR = $(PREFIX)/lib
endif

build:
	make -f Make.applets $(APPLETS)

clean:
	make -f Make.applets clean

install: build
	install -Dm755 $(PACKAGE)-build.sh $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build 
	install -d $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	install -m644 Make.* $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	install -d $(DESTDIR)/boot/efi/tools
	install -m755 $(APPLETS) $(DESTDIR)/boot/efi/tools

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build
	rm -rf $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	for i in $(APPLETS) rm -f $(DESTDIR)/boot/efi/tools/$$i; done

rpm: $(PACKAGE).spec
	rsync -aC --delete . $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION)
	tar czf $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz -C $(HOME)/rpmbuild/SOURCES $(PACKAGE)-$(VERSION)
	rpmbuild -ta $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz
