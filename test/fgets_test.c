#include <stdio.h>
#include <string.h>

#define MAXLEN 16
char buff[MAXLEN];

int main()
{
    char *s;
    int i;
    s = fgets(buff, MAXLEN, stdin);
    if (!s) {
        fprintf(stdout, "s is NULL\n");
        return 0;
    }
    i = strlen(s);
    fprintf(stdout, "%p %d %s ", s, i, s);
    fprintf(stdout, "%d %d\n", s[i-2], s[i-1]);
}

// 123456789abcde\n. input 14 chars | newline. 
// strlen = 15. s[13] = 101 'e'. s[14] = 10 '\n'
/// s[strlen(s) - 1] == '\n'

// 123456789abcdef\n. input 15 chars | newline.
// strlen = 15. s[13] = 101 'e'. s[14] = 102 'f'
// newline is missing from string. s[strlen(s) - 1] != '\n'

// \n. input newline.
// strlen = 1. s[0] = 10 '\n'.
// s[strlen(s) - 1] == '\n'


// EOF. empty string
// strlen exception. s is NULL

// 1. input 1 chars.
// strlen = 1. s[0] = 49 '1'
// s[strlen(s) - 1] == '\n'

// empty file. empty string.
// strlen exception. s is NULL


// if s exists. strlen(s) is always > 0!
// else s is NULL.
