/* dTimeLasterror.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Argc, nTime, oTime;
  CHAR16 **Argv;
  EFI_TIME time;
  
  InitializeLib(ImageHandle, SystemTable);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);

  if (Argc == 2) {
    RT->GetTime(&time, NULL);
    nTime = time.Hour * 3600 + time.Minute * 60 + time.Second;
    if (Argv[1][0] == '0' && Argv[1][1] == 'x') {
      Argv[1]++;
      Argv[1]++;
      oTime = xtoi(Argv[1]);
    } else {
      oTime = Atoi(Argv[1]);
    }
    Print(L"Current Time: %04d-%02d-%02d %02d:%02d:%02d", time.Year, time.Month, time.Day, time.Hour, time.Minute, time.Second);
    if (oTime > 0) {
      Print(L" after %d seconds\n", nTime - oTime);
    }
    return nTime;
  } else {
    Print(L"Usage:\n");
    Print(L"       > dTimeLasterror 0\n");
    Print(L"       > dTimeLasterror %%lasterror%%\n");
    Print(L"       Current Time: 2017-10-05 19:16:34 after 2 seconds\n");
    Print(L"       > echo %%lasterror%%\n");
    Print(L"       0x10F33\n");
    return EFI_INVALID_PARAMETER;
  }
}
