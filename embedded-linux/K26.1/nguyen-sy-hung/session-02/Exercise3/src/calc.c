#include "calc.h" 
double add(double lhs, double rhs) { return lhs + rhs; } 
double sub(double lhs, double rhs) { return lhs - rhs; } 
double mul(double lhs, double rhs) { return lhs * rhs; } 
double divf(double lhs, double rhs) 
{
     if (rhs == 0.0) 
     { return 0.0; } 
     return lhs / rhs; 
}