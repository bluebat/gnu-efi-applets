/* extra.c by Wei-Lun Chao <bluebat@member.fsf.org>, 2017. GPL. */

#include <stdio.h>

char *gets(char * str) {
    char ch;
    int i=0;
    while( (ch = getchar()) != '\r'   &&   ch != (char)EOF )
    {
        putchar(ch);
        str[i] = ch;
        ++i;
    }
    putchar('\n');
    str[i] = '\0';
    return str;
}
