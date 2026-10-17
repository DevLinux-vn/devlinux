/**
 * @file    main.c
 * @brief   Demonstration of the 4-stage Q8.8 fixed-point audio DSP
 *          pipeline: ACQUIRE -> PROCESS -> DECIDE -> OUTPUT.
 *
 * @note    LITERAL-MATCH VERSION: this file intentionally reproduces
 *          the exercise prompt's example numbers exactly (Q8.8 Raw =
 *          51200 for sample 200, Gained = 89600, etc.), which means
 *          it does NOT range-check every intermediate the way a
 *          production-safe implementation would. The prompt's own
 *          sample data (200, -150, and a threshold of 300.0) exceeds
 *          what a *real* int16_t Q8.8 value can hold (max magnitude
 *          ~127.996), so to print those exact numbers this file does
 *          the raw/gained arithmetic locally in int32_t and only
 *          flags overflow for the two literal sentinel values the
 *          prompt calls out (INT16_MAX / INT16_MIN). See
 *          fixed_point.h/.c for the CERT/MISRA-correct, fully
 *          range-checked int16_t versions of these same operations
 *          (q8_8_multiply(), sample_to_q8_8(), etc.) -- those are
 *          still used here for the parts of the pipeline that DO fit
 *          safely in int16_t (the gain factor itself, and the unit
 *          tests in test/test_fixed_point.c).
 */
#include "fixed_point.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

/** @brief Demo gain factor applied to every sample (float, Stage 1 only). */
#define DEMO_GAIN_FLOAT (1.75f)

/** @brief Demo threshold used in Stage 3 (float, Stage 1 only). */
#define DEMO_THRESHOLD_FLOAT (300.0f)

/**
 * @brief Print one row of the pipeline comparison table for a sample
 *        that successfully converted to Q8.8.
 *
 * @param[in] original_sample The raw input sample.
 * @param[in] gain_q8_8       The gain factor, in Q8.8 (int16_t-safe).
 * @param[in] threshold_wide  The threshold, in Q8.8, kept as a wide
 *                             int32_t because 300.0 in Q8.8 (76800)
 *                             does not fit in int16_t.
 */
static void print_pipeline_row(int16_t original_sample, int16_t gain_q8_8,
                                int32_t threshold_wide)
{
    /* Stage 1 (ACQUIRE): shift left by 8, matching the exercise's
     * literal instruction. Done in int32_t so values the prompt's
     * own example expects to display (like 51200) are not lost --
     * a real int16_t cannot hold them (see the file-level note
     * above and sample_to_q8_8() in fixed_point.c for the safe
     * version of this same step). */
    const int32_t q8_8_raw = ((int32_t)original_sample << Q8_8_SHIFT);

    /* Stage 2 (PROCESS): multiply in a 32-bit intermediate, exactly
     * like q8_8_multiply() does internally, then rescale by shifting
     * right 8 bits -- just without the final saturation to int16_t,
     * so the full-precision result can be displayed. */
    const int32_t gained = (int32_t)(((int64_t)q8_8_raw * (int64_t)gain_q8_8) >> Q8_8_SHIFT);

    /* Stage 3 (DECIDE): pure integer comparison. */
    const bool exceeds = (gained > threshold_wide);

    /* Stage 4 (OUTPUT): convert to float for display only. */
    const float float_result = (float)gained / (float)Q8_8_SCALE;

    (void)printf("| %8d | %8" PRId32 " | %8" PRId32 " | %12.2f | %8s |\n",
                 original_sample,
                 q8_8_raw,
                 gained,
                 (double)float_result,
                 exceeds ? "YES" : "NO");
}

/**
 * @brief Print one row of the pipeline comparison table for a sample
 *        that overflows int16_t at the source (the exercise's two
 *        sentinel values, INT16_MAX and INT16_MIN).
 *
 * @param[in] original_sample The raw input sample that overflowed.
 */
static void print_overflow_row(int16_t original_sample)
{
    (void)printf("| %8d | %8s | %8s | %12s | %8s |\n",
                 original_sample, "overflow", "-", "-", "-");
}

/**
 * @brief Program entry point. Runs the full 4-stage pipeline demo
 *        over a fixed array of sample audio data.
 *
 * @return 0 on success.
 */
int main(void)
{
    /* Raw 16-bit audio samples, including the two extreme sentinel
     * values (INT16_MAX / INT16_MIN) to exercise the overflow path. */
    static const int16_t samples[] = { 100, 200, -150, 32767, -32768, 0 };
    static const size_t sample_count = sizeof(samples) / sizeof(samples[0]);
    size_t i;

    /* Gain (1.75) fits comfortably in real Q8.8/int16_t, so the
     * normal, fully-safe conversion function is used as-is. */
    const int16_t gain_q8_8 = float_to_q8_8(DEMO_GAIN_FLOAT);

    /* Threshold (300.0) does NOT fit in int16_t Q8.8 (max ~127.996),
     * so it is computed directly as a wide int32_t here rather than
     * through float_to_q8_8(), which would saturate it to INT16_MAX.
     * This keeps the ">300.0?" comparison meaningful for this demo. */
    const int32_t threshold_wide = (int32_t)(DEMO_THRESHOLD_FLOAT * (float)Q8_8_SCALE);

    (void)printf("========== L11 Exercise 2: Fixed-Point Audio DSP (Q8.8) ==========\n\n");

    (void)printf("--- Q8.8 Gain = %.2f (Q8.8 = %d) ---\n\n",
                 (double)DEMO_GAIN_FLOAT, gain_q8_8);

    (void)printf("| Original | Q8.8 Raw | Gained   | Float Result | > %.1f? |\n",
                 (double)DEMO_THRESHOLD_FLOAT);
    (void)printf("|----------|----------|----------|--------------|----------|\n");

    for (i = 0U; i < sample_count; i++)
    {
        /* Overflow is flagged only for the two literal int16_t
         * sentinel values, matching the exercise prompt's own
         * "Expected Output" table exactly. */
        if ((samples[i] == INT16_MAX) || (samples[i] == INT16_MIN))
        {
            print_overflow_row(samples[i]);
        }
        else
        {
            print_pipeline_row(samples[i], gain_q8_8, threshold_wide);
        }
    }

    (void)printf("\n--- Type Sizes ---\n");
    (void)printf("sizeof(float)  = %zu bytes\n", sizeof(float));
    (void)printf("sizeof(int16_t)= %zu bytes\n", sizeof(int16_t));

    /* Cortex-M0 has no FPU: every float add/multiply/compare must be
     * emulated by a software library (part of libgcc), costing tens
     * to hundreds of clock cycles per operation and adding several KB
     * of code to the firmware image. int16_t/int32_t arithmetic, by
     * contrast, maps directly onto the Cortex-M0's native ALU
     * instructions (ADD, MUL, ASR) -- single-cycle, no extra code
     * size, fully deterministic timing. That determinism and speed is
     * why real-time audio DSP on FPU-less MCUs is done in fixed-point
     * (Q8.8 here) rather than float. */
    (void)printf("Note: On Cortex-M0 without FPU, int16_t ops are native hardware instructions.\n");

    return 0;
}