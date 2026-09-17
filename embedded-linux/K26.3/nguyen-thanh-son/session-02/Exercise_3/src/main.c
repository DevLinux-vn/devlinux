#include <math.h>
#include <stdio.h>
#include "calc.h"
#include "logger.h"

#define MSG_LEN 128

struct operation {
    const char *name;
    char symbol;
    float (*fn)(float, float);
    float a;
    float b;
};

int main(void)
{
    const struct operation ops[] = {
        { "add", '+', calc_add, 12.5f,  7.25f },
        { "sub", '-', calc_sub, 10.0f,  3.5f  },
        { "mul", '*', calc_mul,  4.0f,  2.5f  },
        { "div", '/', calc_div, 9.0f,   3.0f  },
        { "div", '/', calc_div, 5.0f,   0.0f  },   /* division by zero */
    };
    const int total = sizeof(ops) / sizeof(ops[0]);
    int errors = 0;
    char msg[MSG_LEN];

    log_write("Program started");

    printf("===== Calculator summary =====\n");
    printf("Time: ");
    log_timestamp();

    for (int i = 0; i < total; i++)
    {
        const struct operation *op = &ops[i];
        float result = op->fn(op->a, op->b);

        if (isnan(result))
        {
            snprintf(msg, sizeof(msg), "%s: %.2f %c %.2f -> division by zero",
                     op->name, op->a, op->symbol, op->b);
            log_error(msg);
            printf("  %.2f %c %.2f = ERROR (division by zero)\n",
                   op->a, op->symbol, op->b);
            errors++;
        }
        else
        {
            snprintf(msg, sizeof(msg), "%s: %.2f %c %.2f = %.2f",
                     op->name, op->a, op->symbol, op->b, result);
            log_write(msg);
            printf("  %.2f %c %.2f = %.2f\n",
                   op->a, op->symbol, op->b, result);
        }
    }

    snprintf(msg, sizeof(msg), "Program finished: %d operations, %d errors",
             total, errors);
    log_write(msg);

    printf("------------------------------\n");
    printf("Operations: %d | Success: %d | Errors: %d\n",
           total, total - errors, errors);
    printf("Log written to %s\n", LOG_FILE);

    return 0;
}
