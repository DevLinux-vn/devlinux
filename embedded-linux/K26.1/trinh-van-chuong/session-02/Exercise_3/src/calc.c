#include "calc.h"

float addf(float a, float b) { return a + b; }
float subf(float a, float b) { return a - b; }
float mulf(float a, float b) { return a * b; }
float divf(float a, float b, int *error) {
    if (b == 0.0f) {
        *error = 1; // Đánh dấu lỗi chia cho 0
        return 0.0f;
    }
    *error = 0;
    return a / b;
}