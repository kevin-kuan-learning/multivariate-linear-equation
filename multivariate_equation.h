
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DMAX 32


typedef struct Equation {
    int dimension;
    double coef[DMAX];
    double value;
} Equation;

int makeEquation(Equation *pe) ;

int setEquation(Equation *pe, int dimension, double *pcoef, double value) ;

int copyEquation(Equation *pe, Equation *eq2);


int printEquation(Equation *pe) ;

int readEquation(Equation *pe, char *s) ;

int mulEquation(Equation *pe, double scalar) ;

int addEquation(Equation *pe, Equation *pe2, double scalar);

int getEquationMinDimension(Equation *eq) ;


typedef struct System {
    int dimension;
    Equation **eq;
    int m;
} System;

int makeSystem(System *ps, Equation *(eq[]), int m) ;

int solveSystem(System *ps);

int printSystem(System *ps);

