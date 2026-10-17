#include "mathutils.h"

int math_add(int a, int b){
    return a+b;
}
int math_sub(int a, int b){
    return a - b;
}
int math_factorial(int n){
    int result = 1;
    for(int i = 1; i <= n; i++){
        result *= i;
    }
    return result;
}