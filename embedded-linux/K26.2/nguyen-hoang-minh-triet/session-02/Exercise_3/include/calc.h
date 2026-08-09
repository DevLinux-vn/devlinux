#ifndef __INCLUDE_STR_H_
#define __INCLUDE_STR_H_

#pragma once

#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * @brief Adds two floating point numbers.
 * @param a The first number.
 * @param b The second number.
 * @return float
 */
float calc_add(float a, float b);

/*
 * @brief Subtracts two floating point numbers.
 * @param a The first number.
 * @param b The second number.
 * @return float
 */
float calc_sub(float a, float b);

/*
 * @brief Multiplies two floating point numbers.
 * @param a The first number.
 * @param b The second number.
 * @return float
 */
float calc_mul(float a, float b);

/*
 * @brief Divides two floating point numbers.
 * @param a The numerator.
 * @param b The denominator.
 * @return float
 */
float calc_div(float a, float b);

#ifdef __cplusplus
    }
#endif

#endif /*__INCLUDE_STR_H_*/

