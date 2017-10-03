/* PrintF.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Argc;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);

  if (Argc == 3) {
    Print (Argv[1], Argv[2]);
    return EFI_SUCCESS;
  } else {
    Print(L"Usage: PrintF FORMAT VALUE\n");
    return EFI_INVALID_PARAMETER;
  }
}
