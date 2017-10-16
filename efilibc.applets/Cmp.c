/*
Implementation of CMP command in C by Ravish Kg
http://codetrays.blogspot.tw/2015/09/implementation-of-cmp-command-in-c.html
GNU-EFI porting as Cmp.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017
*/
#include <efi.h>
#include <efilib.h>

#include <efilibc/efilibc.h>
#include <efilibc/stdio.h>
#include <efilibc/string.h>

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN Argc;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  efilibc_init(ImageHandle);
  Argc = GetShellArgcArgv(ImageHandle, &Argv);
  
  char argv[3][80];
  unsigned int i, j;
  for (i=0;i<Argc;i++) {
    for (j=0;j<StrLen(Argv[i]);j++) {
      argv[i][j] = (char)Argv[i][j];
    }
    argv[i][j] = '\0';
  }

  if (Argc == 3) {
    FILE *file1, *file2;
    BOOLEAN stillsame = TRUE;
    char byte1 = '\0', byte2 = '\0';
    int line = 1, byte_count = 1;
    
    file1 = fopen(argv[1],"r");
    file2 = fopen(argv[2],"r");
    
    while (byte1 != (char)EOF && byte2 != (char)EOF) {
      byte1 = fgetc(file1);
      byte2 = fgetc(file2);
      if (byte1=='\n' || byte2 == '\n') {
        ++line;
      }
      if (byte1 == byte2) {
        ++byte_count;
      } else {
        stillsame = FALSE;
        break;
      }
    }
    fclose(file1);
    fclose(file2);
    
    if (stillsame) {
      return EFI_SUCCESS;
    } else {
      printf("%s %s differ: char %d, line %d\n", argv[1], argv[2], byte_count, line);
      return EFI_LOAD_ERROR;
    }
  } else {
    Print(L"Usage: Cmp FILE1 FILE2\n");
    return EFI_INVALID_PARAMETER;
  }
}
