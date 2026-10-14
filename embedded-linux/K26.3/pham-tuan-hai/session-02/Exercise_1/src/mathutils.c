#include "../include/mathutils.h"

int math_add(int a, int b){
	return a + b;
}

int math_sub(int a, int b){
	return a - b;
}

int math_factorial(int n){
	if (n < 0) return 0;
	int fact = 1;
	for(int i = n; i >= 1; i--) {
  	fact*=i;
	}
	return fact;
}
