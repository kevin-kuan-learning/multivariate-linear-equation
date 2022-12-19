#include <stdio.h>
#include <string.h>

int test(const char * p1, const char* p2, int n) {
    int r = strncmp(p1, p2, n);
    fprintf(stdout, "%s %s %d -> %d\n", p1, p2, n, r);
    return r;
}

int main() 
{
    test("", "", 1);
    test("abc", "abcdef\n", 2); // 0
    test("abc", "abcdef\n", 3); // 0   -> n should be strlen(p1).
    test("abc", "abcdef\n", 4); // -1
    test("abc", "abcdef\n", 5); // -1
}

