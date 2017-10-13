/* Implementation of CMP command in C by Ravish Kg
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
  UINTN argc;
  CHAR16 **argv;
  
  InitializeLib(ImageHandle, SystemTable);
  efilibc_init(ImageHandle);
  argc = GetShellArgcArgv(ImageHandle, &argv);

  if (argc == 3) {
    FILE *file1, *file2;
    BOOLEAN samefiles = TRUE;
    char byte1, byte2;
    int line = 1, byte_count = 0;
    
    file1 = fopen((char *)argv[1],"r+");
    printf("01%s\n", (char *)argv[1]);
    printf("01%d\n", file1);
    file2 = fopen((char *)argv[2],"r+");
    printf("02%s\n", (char *)argv[2]);
    printf("02%d\n", file2);
    
    while (samefiles) {
      printf("1char %d, line %d\n", byte_count, line);
      byte1 = fgetc(file1);
      printf("2char %d, line %d, char1 %c\n", byte_count, line, byte1);
      byte2 = fgetc(file2);
      printf("3char %d, line %d, char1 %c, char2 %c\n", byte_count, line, byte1, byte2);
      if (byte1=='\n' || byte2 == '\n') {
        ++line;
      }
      if (byte1 == byte2 && byte1 != (char)EOF && byte2 != (char)EOF) {
        ++byte_count;
        printf("char %d, line %d, char1 %c, char2 %c\n", byte_count, line, byte1, byte2);
      } else {
        samefiles = FALSE;
      }
    }
    fclose(file1);
    fclose(file2);
    
    if (samefiles) {
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
