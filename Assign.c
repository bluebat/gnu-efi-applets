/* Assign.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

#define SHELL_VARIABLE_GUID \
   (EFI_GUID) {0x158def5a, 0xf656, 0x419c, {0xb0,0x27,0x7a,0x31,0x92,0xc0,0x79,0xd2}}
#define BUFSIZE 64

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  INTN Argc;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);

  if (Argc == 3) {
    EFI_GUID gEfiGlobalVariableGuid = SHELL_VARIABLE_GUID;
//    EFI_GUID vendorGuid = NullGuid;
    CHAR16 buffer[BUFSIZE];
    CHAR16 *bufptr = buffer;
//    CHAR16 *value;
    UINTN bufsize = 0;

    ZeroMem(bufptr, BUFSIZE);
    StrCat(buffer, Argv[1]);
    bufsize = StrLen(buffer) * 2;
//    LibDeleteVariable(Argv[2], &gEfiGlobalVariableGuid);
    LibSetVariable(Argv[2], &gEfiGlobalVariableGuid, bufsize, bufptr);
//    value = LibGetVariable(Argv[2], &vendorGuid);
//    Print(L"%s\n", Argv[1]);
//    Print(L"%s\n", value);
//    FreePool(value);
    return EFI_SUCCESS;
  } else {
    Print(L"Usage:\n");
    Print(L"       > set -d var\n");
    Print(L"       > Assign value var\n");
    Print(L"       > set var\n");
    Print(L"       > echo %%var%%\n");
    Print(L"       value\n");
    return EFI_INVALID_PARAMETER;
  }    
}
