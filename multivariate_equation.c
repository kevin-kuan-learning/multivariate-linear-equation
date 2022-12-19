// Goal:
// - read & print equations from stdin
// - dynamic allocate coefficient array, free & check for errors
// - substitution
// - gaussian elimination
// - moore-penrose
// - take care of floaing-point operations

#include "multivariate_equation.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DMAX 32
#define EPSILON 0.000001

#define ABS(a) ((a>0)? a : -a)
#define EQUAL_DBL(a,b)  (ABS(a-b)<EPSILON)

int makeEquation(Equation *pe) {
    pe->dimension = 0;
    memset(pe->coef, 0, sizeof(double) * DMAX);
    pe->value = 0;
    return 1;
}

int setEquation(Equation *pe, int dimension, double *pcoef, double value) {

    if (dimension < 0) {
        return 0;
    }

    pe->dimension = dimension;
    pe->value = value;

    if (pcoef) {
        double *p = pe->coef, *p2 = pcoef;
        for (int i = 0; i < dimension; ++i, ++p, ++p2) {
            *p = *p2;
        }
    }
    else {
        memset(pe->coef, 0, sizeof(double) * dimension);
    }
    return 1;
}

int copyEquation(Equation *pe, Equation *eq2)
{
    return setEquation(pe, eq2->dimension, eq2->coef, eq2->value);
}

// TODO. populate string then print.
int printEquation(Equation *pe) {
    double *p = pe->coef;
    int i = 0;
    if (pe->dimension >= 0) {
        printf("(%f)x%d", *p, i);
    }
    ++p;
    for (i = 1; i < pe->dimension; ++i, ++p) {
        printf(" + (%f)x%d", *p, i);
    }
    printf(" = %f\n", pe->value);

    return 1;
}

int readEquation(Equation *pe, char *s) {
    makeEquation(pe);

    char *start, *end;
    enum {COEF, VALUE} state_coef = COEF;
    enum {DELIM, NUMBER} state_char = DELIM;

    while (*s) {
        if (*s == '=') {
            state_coef = VALUE;
        }
        else if (*s == '.' || ('0' <= *s && *s <= '9')) {
            if (state_char == DELIM) {
                double c = atof(s);
                if (state_coef == COEF) {
                    if (pe->dimension < DMAX) {
                        pe->coef[pe->dimension++] = c;
                    }
                }
                else {
                    pe->value = c;
                }
                state_char = NUMBER;
            }
        }
        else {
            if (state_char == NUMBER) {
                state_char = DELIM;
            }
        }
        s++;
    }

    return 1;
}

int mulEquation(Equation *pe, double scalar) {
    for (int i = 0; i < pe->dimension; ++i) {
        pe->coef[i] *= scalar;
    }
    pe->value *= scalar;
    return 1;
}

int addEquation(Equation *pe, Equation *pe2, double scalar) {
    int d = min(pe->dimension, pe2->dimension);
    for (int i = 0; i < d; ++i) {
        pe->coef[i] += pe2->coef[i] * scalar;
        if (EQUAL_DBL(pe->coef[i], 0)) {
            pe->coef[i] = 0;
        }
    }
    pe->value += pe2->value * scalar;
    return 1;
}

int getEquationMinDimension(Equation *eq) {
    for (int i = 0; i < eq->dimension; i++) {
        if (eq->coef[i] != 0) return i;
    }
    return DMAX;
}

int makeSystem(System *ps, Equation *(eq[]), int m) {
    int maxd = 0;
    for (int i = 0; i < m; ++i) {
        maxd = max(maxd, eq[i]->dimension);
    }
    for (int i = 0; i < m; ++i) {
        Equation *pe = eq[i];
        for (int d = pe->dimension; d < maxd; ++d) {
            pe->coef[d] = 0;
        }
        pe->dimension = maxd;
    }

    ps->dimension = maxd;
    ps->eq = eq;
    ps->m = m;
    return 1;
}

int swap(Equation **p, Equation **q) {
    if (p == q) 
        return 0;
    Equation *x = *p;
    *p = *q;
    *q = x;
    return 1;
}

int solveSystem(System *ps) {

    // TODO: dimensions vs m (equation count)
    // if 
    for (int i = 0; i < ps->m; ++i) {
        int mind = DMAX;
        int j;
        for (j = i; j < ps->m; ++j) {
            Equation **ppj = ps->eq + j;
            int d = getEquationMinDimension(*ppj);
            if (d == i) {
                mind = d;
                break;
            }
            else if (d < mind) {
                mind = d;
            }
        }
        if (mind == DMAX || j >= ps->m) {
            // premature end. the other equations contain no coefficient.
            break;
        }

        swap(ps->eq + i, ps->eq + j);
        double scalar = 1.0 / (ps->eq[i])->coef[mind];
        mulEquation(ps->eq[i], scalar);
        for (int j = 0; j < ps->m; ++j) {
            if (i == j) continue;
            if (EQUAL_DBL(ps->eq[j]->coef[mind], 0)) continue;
            double s = - (ps->eq[j])->coef[mind];
            addEquation(ps->eq[j], ps->eq[i], s);
        }
    }

    // check if 0 = 1 equations exist -> no solution
    // check if 1 coef on 1 value match for all equations

    return 1;
}

int printSystem(System *ps) 
{
    for (int i = 0; i < ps->m; ++i) {
        printEquation(ps->eq[i]);
    }
    return 1;
}

int test_system_base(double **A, double v[], int rows, int cols)
{
    Equation **eq;
    eq = malloc(sizeof(Equation*) * rows);
    for (int r = 0; r < rows; r++) {
        eq[r] = malloc(sizeof(Equation));
        makeEquation(eq[r]);
        memcpy(eq[r]->coef, A[r], sizeof(double) * cols);
        eq[r]->dimension = cols;
        eq[r]->value = v[r];
    }

    System s;
    makeSystem(&s, eq, rows);
    printf("============================\n");
    printSystem(&s);
    printf("----------------------------\n");
    solveSystem(&s);
    printSystem(&s);
    printf("============================\n");
    
    for (int r = 0; r < rows; r++) {
        free(eq[r]);
    }
    free(eq);
    return 1;
}

// test case.
// - more than 1 equation contains coef = 0.

int test_system1()
{
#define ROWS 1
#define COLS 1
    double A[ROWS][COLS] = {0};
    double v[ROWS] = {0};

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);
    return 1;
#undef ROWS
#undef COLS
}


int test_system1p2()
{
#define ROWS 2
#define COLS 1
    double A[ROWS][COLS] = {
        {0},
        {1}
    };
    double v[ROWS] = {
        0,
        2
    };

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);    
    return 1;
    
#undef ROWS
#undef COLS
}

int test_system2()
{
#define ROWS 2
#define COLS 2
    double A[ROWS][COLS] = {
        {1, 1},
        {2, 5}
    };
    double v[ROWS] = {
        3,
        12
    };

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);    return 1;
#undef ROWS
#undef COLS
}

int test_system3()
{
#define ROWS 3
#define COLS 3
    double A[ROWS][COLS] = {
        {1, 1, 1},
        {1, 3, 2},
        {2, 1, 1}
    };
    double v[ROWS] = {
        0,
        -1,
        1
    };

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);    return 1;
#undef ROWS
#undef COLS
}

int test_system4()
{
#define ROWS 3
#define COLS 2
    double A[ROWS][COLS] = {
        {1, 1},
        {1, 3},
        {2, 1}
    };
    double v[ROWS] = {
        2,
        8,
        1
    };

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);    return 1;
#undef ROWS
#undef COLS
}


int test_system5()
{
#define ROWS 3
#define COLS 4
    double A[ROWS][COLS] = {
        {1, 1, 1, 2},
        {1, 3, 2, 4},
        {2, 1, 1, 3},
    };
    double v[ROWS] = {
        2,
        8,
        1
    };

    double *pA[ROWS];
    for (int i = 0; i < ROWS; ++i) {
        pA[i] = A[i];
    }
    test_system_base(pA, v, ROWS, COLS);    return 1;
#undef ROWS
#undef COLS
}

int main()
{
    test_system1();
    test_system1p2();
    test_system2();
    test_system3();
    test_system4();
    test_system5();
    return 1;
}