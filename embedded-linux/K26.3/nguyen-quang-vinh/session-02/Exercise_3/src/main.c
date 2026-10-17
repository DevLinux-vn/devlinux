#include <stdio.h>
#include <math.h>

#include "calc.h"
#include "logger.h"

#define MSG_BUFSIZE 100
#define EXIT_ERROR 1

int main(void)
{
	float a;
	float b;

	float add_result;
	float sub_result;
	float mul_result;
	float div_result;

	char msg[MSG_BUFSIZE];

	printf("Nhap a: ");
	if (scanf("%f", &a) != 1) {
		fprintf(stderr, "Invalid input for a\n");
		return EXIT_ERROR;
	}

	printf("Nhap b: ");
	if (scanf("%f", &b) != 1) {
		fprintf(stderr, "Invalid input for b\n");
		return EXIT_ERROR;
	}

	log_timestamp();

	add_result = calc_add(a, b);
	printf("%.2f + %.2f = %.2f\n", a, b, add_result);

	snprintf(msg, sizeof(msg),
		 "Addition: %.2f + %.2f = %.2f",
		 a, b, add_result);

	log_write(msg);

	sub_result = calc_sub(a, b);
	printf("%.2f - %.2f = %.2f\n", a, b, sub_result);

	snprintf(msg, sizeof(msg),
		 "Subtraction: %.2f - %.2f = %.2f",
		 a, b, sub_result);

	log_write(msg);

	mul_result = calc_mul(a, b);
	printf("%.2f * %.2f = %.2f\n", a, b, mul_result);

	snprintf(msg, sizeof(msg),
		 "Multiplication: %.2f * %.2f = %.2f",
		 a, b, mul_result);

	log_write(msg);

	div_result = calc_div(a, b);

	if (isnan(div_result)) {
		printf("Error: division by zero\n");
		log_error("Division by zero");
	} else {
		printf("%.2f / %.2f = %.2f\n", a, b, div_result);

		snprintf(msg, sizeof(msg),
			 "Division: %.2f / %.2f = %.2f",
			 a, b, div_result);

		log_write(msg);
	}

	printf("\nCalculation completed.\n");

	return 0;
}