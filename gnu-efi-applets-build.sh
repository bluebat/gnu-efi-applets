#!/bin/sh
if [ $# -eq 0 ] ; then
  echo "Building applets for GNU-EFI"
  echo "Usage: gnu-efi-applets-build [ Bar.efi | Bar.a | Bar.o ] .."
  exit 1
else
  make -f /usr/lib/gnu-efi-applets/Make.applets MAKEPATH=/usr/lib/gnu-efi-applets "$@"
fi