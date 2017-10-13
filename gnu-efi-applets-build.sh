#!/bin/sh
if `getconf LONG_BIT` = 64 ; then
  LIBDIR=/usr/lib64
else
  LIBDIR=/usr/lib
fi
if [ $# -eq 0 ] ; then
  echo "Building applets for GNU-EFI"
  echo "Usage: gnu-efi-applets-build Foo.[efi|a|o] .. [LIBEXTRA=$LIBDIR/efilibc.a]"
  exit 1
else
  make -f $LIBDIR/gnu-efi-applets/Make.applets MAKEPATH=$LIBDIR/gnu-efi-applets "$@"
fi
