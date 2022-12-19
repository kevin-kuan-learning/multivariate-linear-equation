#include "multivariate_equation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int _getline(char*buff, int maxlen, FILE *f) {
    char *s = fgets(buff, maxlen, f);
    if (!s) return -1; // ERR
    int len = strlen(s);
    if (len == (maxlen-1) && buff[len-1] != '\n') return -2; // LONG
    return len; // OK
}

int getline(char *buff, int maxlen, FILE* f) {
    enum {LONG, OK} state = OK;

    while (1) {
        int r = _getline(buff, maxlen, f);
        switch (state) {
            case OK:
                if (r == -2) {
                    state = LONG;
                    fprintf(stderr, "line is too long. truncate.\n");
                }
                else if (r == -1) {
                    return 0;
                }
                else {
                    return r;
                }
                break;
            case LONG:
                if (r >= 0) {
                    state = OK; // truncate last bit of too long line.
                }
                break;
            default:
                fprintf(stderr, "invalid state\n");
                return 0;
        }
    }
}

#define LINE_MAXLEN 4096

#define MAXEQ 16
Equation eq[MAXEQ];
int neq = 0;




int main()
{
    char line[LINE_MAXLEN];

    while (1) {
        int r = getline(line, LINE_MAXLEN, stdin);
        // fprintf(stdout, "%d %s\n--------\n", r, line);
        if (r <= 0) {
            break;
        }

        if (strncmp(line, "a", 1) == 0) {
            if (neq < MAXEQ) {
                readEquation(eq + neq, line + 1);
                neq++;
            }
            else {
                fprintf(stderr, "Equations reached limit\n");
            }
        }
        else if (strncmp(line, "p", 1) == 0) {
            fprintf(stdout, "neq %d\n", neq);
            for (int i = 0; i < neq; i++) {
                printEquation(eq + i);
            }
        }
        else if (strncmp(line, "x", 1) == 0) {
            neq = 0;
            fprintf(stdout, "neq %d\n", neq);
        }
        else {
            fprintf(stdout, "invalid command.\n");
        }
    }
}


