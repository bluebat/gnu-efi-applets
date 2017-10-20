/*
Implementation of CMP command in C by Ravish Kg
http://codetrays.blogspot.tw/2015/09/implementation-of-cmp-command-in-c.html
GNU-EFI porting as Cmp.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017
*/
#include <efi.h>
#include <efilib.h>

#include <efilibc/efilibc.h>
#include <efilibc/stdlib.h>
#include <efilibc/stdio.h>
#include <efilibc/string.h>

EFI_HANDLE IH;
EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN argc;
  CHAR16 **Argv;
  
  InitializeLib(ImageHandle, SystemTable);
  efilibc_init(ImageHandle);
  IH = ImageHandle;
  argc = GetShellArgcArgv(ImageHandle, &Argv);

  char **argv;
  unsigned int i, j;
  argv = malloc(argc * sizeof(char *));
  for(i=0; i<argc; i++) {
    *(argv+i) = malloc((StrLen(Argv[i])+1) * sizeof(char));
    for (j=0; j<StrLen(Argv[i]); j++) {
      argv[i][j] = (char)Argv[i][j];
    }
    argv[i][j] = '\0';
  }

  if (argc == 3) {
    FILE *file1, *file2;
    BOOLEAN stillsame = TRUE;
    char byte1 = '\0', byte2 = '\0';
    int line = 1, byte_count = 1;
    
    if ((file1 = fopen(argv[1],"r")) == NULL)
    {
        printf("Can't open %s\n", argv[1]);
        return 2;
    }
    if ((file2 = fopen(argv[2],"r")) == NULL)
    {
        printf("Can't open %s\n", argv[2]);
        return 2;
    }
    
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
      exit(0);
    } else {
      printf("%s %s differ: char %d, line %d\n", argv[1], argv[2], byte_count, line);
      exit(1);
    }
  } else {
    printf("Usage: Cmp FILE1 FILE2\n");
    exit(2);
  }
}
