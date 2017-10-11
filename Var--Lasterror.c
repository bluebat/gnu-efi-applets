/* Var--Lasterror.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Argc, Value;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);

  if (Argc == 2) {
    if (Argv[1][0] == '0' && Argv[1][1] == 'x') {
      Argv[1]++;
      Argv[1]++;
      Value = xtoi(Argv[1]) - 1;
    } else {
      Value = Atoi(Argv[1]) - 1;
    }
    return Value;
  } else {
    Print(L"Usage:\n");
    Print(L"       > set var 100\n");
    Print(L"       > Var--Lasterror %%var%%\n");
    Print(L"       > echo %%lasterror%%\n");
    Print(L"       0x63\n");
    return EFI_INVALID_PARAMETER;
  }
}
