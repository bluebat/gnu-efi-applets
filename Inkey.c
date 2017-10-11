/* Inkey.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);

  UINTN KeyEvent;
  EFI_INPUT_KEY Key;
  UINT16 KeyCode;
  
  Key.ScanCode=SCAN_NULL;
  SystemTable->ConIn->Reset(SystemTable->ConIn, FALSE);
  SystemTable->BootServices->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &KeyEvent);
  SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);

  if (Key.ScanCode == SCAN_NULL) {
    KeyCode = (UINT16)Key.UnicodeChar;
    if (KeyCode < 24) {
      return KeyCode+16;
    } else {
      return KeyCode;
    }
  } else {
    return Key.ScanCode;
  }
}
