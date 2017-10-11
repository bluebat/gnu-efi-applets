#!/bin/sh
if [ $# -eq 0 ] ; then
  echo "Building applets for GNU-EFI"
  echo "Usage: gnu-efi-applets-build Foo.[efi|a|o] .. [LIBEXTRA=/usr/lib64/efilibc.a]"
  exit 1
else
  make -f /usr/lib/gnu-efi-applets/Make.applets MAKEPATH=/usr/lib/gnu-efi-applets "$@"
fi