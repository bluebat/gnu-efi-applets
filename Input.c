/* Input.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  INTN Argc;
  CHAR16 **Argv, InputString[256] = L"";
  UINTN Index = 0;
  UINTN KeyEvent;
  EFI_INPUT_KEY Key;

  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);
  
  if (Argc == 2) {
    Print(L"%s", Argv[1]);
    
    while ((UINTN)Key.UnicodeChar != 13) {
      Key.ScanCode=SCAN_NULL;
      SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &KeyEvent);
      ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
      InputString[Index++] = Key.UnicodeChar;
      Print(L"%c", Key.UnicodeChar);
    }
    InputString[Index] = '\0';
    SystemTable->StdErr->OutputString(SystemTable->StdErr, InputString);
    return EFI_SUCCESS;    
  } else {
//    ShellSetEnvironmentVariable(Argv[2], InputString, TRUE);
    Print(L"Usage: Input PROMPT [ VARIABLE | 2>a FOOBAR ]\n");
    Print(L"Writing to variable not supported yet.\n");
    return EFI_INVALID_PARAMETER;
  }
}
