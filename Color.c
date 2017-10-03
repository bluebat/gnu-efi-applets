/* Color.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Argc, TextAttr;
  CHAR16 **Argv;

  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);

  if (Argc > 1) {
    if (Argc > 2) {
      TextAttr = EFI_TEXT_ATTR(Atoi(Argv[1]), Atoi(Argv[2]));
    } else {
      TextAttr = EFI_TEXT_ATTR(Atoi(Argv[1]), SystemTable->ConOut->Mode->Attribute >> 4);
    }
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, TextAttr);
    return EFI_SUCCESS;
  } else {
    Print(L"Usage: Color TEXT_COLOR [BACKGROUND_COLOR]\n");
    return EFI_INVALID_PARAMETER;
  }
}
