#ifndef __INCLUDE_MATHUTILS_H_
#define __INCLUDE_MATHUTILS_H_

#pragma once

#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif


/*
 * Function: math_add
 * ------------------
 * Adds two integers.
 *
 * a: first integer
 * b: second integer
 *
 * Returns: the sum of a and b
 */
int math_add(int a, int b);

/*
 * Function: math_sub
 * ------------------
 * Subtracts two integers.
 *
 * a: first integer
 * b: second integer
 *
 * Returns: the difference of a and b
 */
int math_sub(int a, int b);

/*
 * Function: math_factorial
 * ------------------------
 * Calculates the factorial of an integer.
 *
 * n: the integer for which to calculate the factorial
 *
 * Returns: -1 if n is negative, -2 if n is greater than 12 (to prevent overflow), or the factorial of n otherwise
 */
int math_factorial(int n);

#ifdef __cplusplus
    }
#endif

#endif /*__INCLUDE_MATTHUTILS_H_*/