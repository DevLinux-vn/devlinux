/**
 * @file    main.c
 * @brief   Demonstration program for the IEEE-754 Float Inspector &
 *          Safety Checker exercise.
 *
 * @details Demonstrates:
 *          1. Bit-layout inspection of representative float values.
 *          2. Classification of special IEEE-754 values.
 *          3. The pitfall of exact float equality vs. epsilon compare.
 *          4. The `memcmp` trap between +0.0f and -0.0f.
 *          5. The float loop-counter trap vs. a safe integer counter.
 */
#include "float_inspector.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Program entry point. Runs all demonstration sections and
 *        prints results in a fixed, human-readable format.
 *
 * @return 0 on success (always succeeds for this demo program).
 */
int main(void)
{
    (void)printf("========== L11 Exercise 1: IEEE-754 Float Inspector ==========\n\n");

    /* ---------------------------------------------------------- */
    /* Section 1: Bit Layout Inspector                            */
    /* ---------------------------------------------------------- */
    (void)printf("--- Bit Layout ---\n");
    inspect_float(1.0f);
    inspect_float(-2.5f);
    inspect_float(0.0f);
    inspect_float(-0.0f);
    (void)printf("\n");

    /* ---------------------------------------------------------- */
    /* Section 2: Special Value Classification                    */
    /* ---------------------------------------------------------- */
    (void)printf("--- Special Value Classification ---\n");

    /* CERT FLP32-C: division by literal zero is well-defined for
     * floats (produces +/-Infinity or NaN), unlike integer division. */
    (void)printf("1.0 / 0.0   => %s\n", classify_float(1.0f / 0.0f));
    (void)printf("-1.0 / 0.0  => %s\n", classify_float(-1.0f / 0.0f));
    /* Intentional: 0.0f / 0.0f is used deliberately here to produce
     * a NaN for the classifier demo, not a copy-paste mistake. */
    // cppcheck-suppress duplicateExpression
    (void)printf("0.0 / 0.0   => %s\n", classify_float(0.0f / 0.0f));

    /* MISRA Directive 4.11: sqrtf() of a negative number is
     * mathematically undefined for reals; sqrtf() reports this via
     * NaN rather than crashing, but production code should validate
     * the input domain (e.g. `if (x >= 0.0f) { ... }`) before
     * calling sqrtf() in the first place. The call below is used
     * ONLY to demonstrate that classify_float() correctly detects
     * the resulting NaN -- it is intentionally out-of-domain. */
    // cppcheck-suppress invalidFunctionArg
    (void)printf("sqrt(-1.0)  => %s\n", classify_float(sqrtf(-1.0f)));
    (void)printf("-0.0        => %s\n", classify_float(-0.0f));
    (void)printf("42.0        => %s\n", classify_float(42.0f));
    (void)printf("\n");

    /* ---------------------------------------------------------- */
    /* Section 3: Epsilon Comparison                               */
    /* ---------------------------------------------------------- */
    (void)printf("--- Epsilon Comparison ---\n");
    {
        /* `volatile` forces the additions to actually happen at
         * runtime in single-precision, rather than letting the
         * compiler constant-fold the literals at higher (double)
         * precision, which would hide the rounding error we want to
         * demonstrate.
         *
         * NOTE: On this platform/compiler, 0.1f + 0.2f happens to
         * round to exactly the same bits as the literal 0.3f in
         * single precision (this is a rounding *coincidence*, not a
         * guarantee -- it is NOT portable across compilers/CPUs).
         * 1.1f + 2.2f vs. 3.3f is used here instead because it
         * reliably demonstrates the raw `==` trap in float
         * precision on all common platforms. */
        volatile float a = 1.1f;
        volatile float b = 2.2f;
        volatile float sum = a + b;
        volatile float target = 3.3f;
        const float epsilon = 1e-6f;

        /* MISRA Rule 13.3: avoid embedding the raw == comparison in
         * side-effect-bearing expressions; compute booleans first. */
        const bool exact_equal = (sum == target);
        const bool epsilon_equal = float_equal(sum, target, epsilon);

        (void)printf("1.1 + 2.2 == 3.3 ? %s  (raw == fails!)\n",
                     exact_equal ? "YES" : "NO");
        (void)printf("float_equal(1.1+2.2, 3.3, 1e-6) ? %s  (epsilon passes)\n",
                     epsilon_equal ? "YES" : "NO");
    }
    (void)printf("\n");

    /* ---------------------------------------------------------- */
    /* Section 4: The memcmp Trap                                  */
    /* ---------------------------------------------------------- */
    (void)printf("--- memcmp Trap ---\n");
    {
        const float pos_zero = 0.0f;
        const float neg_zero = -0.0f;
        const bool math_equal = (pos_zero == neg_zero);
        const int32_t memcmp_result = memcmp(&neg_zero, &pos_zero, sizeof(float));

        (void)printf("-0.0 == 0.0 ? %s (math equality)\n",
                     math_equal ? "YES" : "NO");
        (void)printf("memcmp(-0.0, 0.0) == 0 ? %s (different bit patterns!)\n",
                     (memcmp_result == 0) ? "YES" : "NO");
    }
    (void)printf("\n");

    /* ---------------------------------------------------------- */
    /* Section 5: Float Loop Counter Trap                          */
    /* ---------------------------------------------------------- */
    (void)printf("--- Float Loop Counter Trap ---\n");
    {
        /* MISRA Rule 14.1 (violated intentionally, for demonstration
         * only): a loop counter shall not have essentially floating
         * type, because repeated += accumulates rounding error and
         * can make the actual iteration count diverge from what a
         * human would expect by reading the loop header. */
        uint32_t float_loop_count = 0U;
        volatile float f;

        /* `volatile` prevents the compiler from folding this whole
         * loop away at compile time; we want the *actual* runtime
         * single-precision rounding behavior of the target
         * platform, which is exactly the point of the trap: the
         * iteration count is not portably predictable by reading
         * the loop header. On some platforms/optimization levels
         * this yields 11 iterations (0.1f accumulates slightly
         * *under* 1.0 near the end); on others (as on this build)
         * it yields exactly 10, because 0.1f's rounding error
         * happens to push the running sum slightly *over* 1.0 one
         * step earlier. Either way, relying on "it'll be 10" is
         * unsafe -- MISRA Rule 14.1 forbids float loop counters
         * for exactly this reason.
         *
         */
        // NOLINTNEXTLINE(clang-analyzer-security.FloatLoopCounter)
        for (f = 0.0f; f < 1.0f; f += 0.1f)
        {
            float_loop_count++;
        }

        (void)printf("Float loop ran %u times (not guaranteed to be 10!)\n", float_loop_count);

        /* Safe alternative: use an integer counter as the loop
         * control variable, and derive the float value (if needed)
         * from the integer index inside the loop body. */
        {
            uint32_t integer_loop_count = 0U;
            uint32_t i;

            for (i = 0U; i < 10U; i++)
            {
                const float derived_value = (float)i * 0.1f;
                (void)derived_value; /* Value available if needed. */
                integer_loop_count++;
            }

            (void)printf("Integer loop ran %u times (correct)\n", integer_loop_count);
        }
    }

    return 0;
}