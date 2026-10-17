#ifndef CALC_H
#define CALC_H
double addf(double a, double b);
double subf(double a, double b);
double mulf(double a, double b);
int    divf(double a, double b, double *out);  // return 0=OK, -1=div-by-zero
#endif
