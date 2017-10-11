Requires
========
gnu-efi-devel

Install & Usage
===============
	make
	make install
	cd MYPATH
	$EDITOR MYAPPLETS.c
	gnu-efi-applets-build MYAPPLETS.efi

References
==========
* Make.applets, Make.defaults, Make.rules:

	based on Makefile, Make.defaults, Make.rules from gnu-efi/apps by Hewlett-Packard Co.
* StallForKey.c:

	based on StallForKey.c from efi-sct/Application by Intel Corporation
* shellopt.c:

	based on shellopt.c by Finnbarr P. Murphy
* Banner.c:

	based on SYSVbanner.c by Brian Wallis <brw@jim.odr.oz>
* SetA.c:

	based on bc-robert.c by Robert D. Wei <robertmodesty@gmail.com>
* Assign.c, Color.c, Inkey.c, Input.c, PrintF.c, Var--Lasterror.c:

	made by Wei-Lun Chao <bluebat@member.fsf.org>

FIXME
=====
Setting values to shell variables in Assign.c only partially works.
This function can't be applied to SetA.c and Input.c yet.
