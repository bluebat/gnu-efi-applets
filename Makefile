VERSION = 3.0.3
DESTDIR =
PREFIX = /usr
PACKAGE = gnu-efi-applets
APPLETS = Banner Color Inkey Input SetA StallForKey

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS), 64)
  LIBDIR = $(PREFIX)/lib64
else
  LIBDIR = $(PREFIX)/lib
endif

build:
	make -f Make.applets

clean:
	make -f Make.applets

install: build
	install -Dm755 $(PACKAGE)-build.sh $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build 
	install -d $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	install -m644 Make.* $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	install -d $(DESTDIR)/boot/efi/tools
	install -m755 *.efi $(DESTDIR)/boot/efi/tools

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PACKAGE)-build
	rm -rf $(DESTDIR)$(PREFIX)/lib/$(PACKAGE)
	for i in $(APPLETS) rm -f $(DESTDIR)/boot/efi/tools/$$i.efi; done

rpm: $(PACKAGE).spec
	rsync -aC --delete . $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION)
	tar czf $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz -C $(HOME)/rpmbuild/SOURCES $(PACKAGE)-$(VERSION)
	rpmbuild -ta $(HOME)/rpmbuild/SOURCES/$(PACKAGE)-$(VERSION).tar.gz
