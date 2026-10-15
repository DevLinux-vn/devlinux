#include <stdio.h>
#include <math.h>
#include "calc.h"
#include "logger.h"

int main() {

	log_timestamp();
	log_write("Open main");

	float a = 10.0;
	float b = 2.5;
	float c = 0.0;

	printf("=== KET QUA TINH TOAN ===\n");

	float sum = calc_add(a, b);
	printf("ADD: %.2f + %.2f = %.2f\n", a, b, sum);
	log_write("Da thuc hien phep cong\n");

	float sub = calc_sub(a, b);
	printf("SUB: %.2f - %.2f = %.2f\n", a, b ,sub);
	log_write("Da thuc hien phep tru\n");


	float mul = calc_mul(a, b);
	printf("MUL: %.2f * %.2f = %.2f\n", a, b ,mul);
	log_write("Da thuc hien phep nhan\n");

	//Chia hop le
	float div = calc_div(a, b);
	printf("DIV: %.2f / %.2f = %.2f\n", a, b, div);
	log_write("Da thuc hien phep chia\n");

	//Chia khong hop le
	float err_div = calc_div(a, c);
	if (isnan(err_div)) {
		printf("DIV: %.2f / %.2f = ERROR / 0\n", a, c);
		log_error("Phat hien loi chia cho 0");
	}

	printf("================================");
	printf("SUCCESS!\n");

	return 0;
}
