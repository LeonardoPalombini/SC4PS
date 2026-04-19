/**
 * fft_example.c
 *
 * Demonstrates the fft_utils API.
 *
 * Usage:
 *   ./fft_example [sample_rate_hz] [n_samples] [output.csv]
 *
 * Defaults:
 *   sample_rate = 1000 Hz
 *   n_samples   = 1024   (must be power of 2)
 *   output      = fft_output.csv
 *
 * The synthetic signal is a sum of three sinusoids:
 *   - 50 Hz  at amplitude 1.0
 *   - 120 Hz at amplitude 0.5
 *   - 300 Hz at amplitude 0.25
 * plus a small amount of Gaussian-like noise.
 */

#include "fft_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple LCG-based noise in [-1, 1]
static double rand_noise(void)
{
    return ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

/* -------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    // Parse optional command-line arguments
    double sample_rate = 1000.0;
    size_t n = 1024;
    const char *outfile = "fft_output.csv";

    if (argc >= 2) {
        sample_rate = atof(argv[1]);
        if (sample_rate <= 0.0) {
            fprintf(stderr, "Error: sample_rate must be > 0 (got '%s').\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    if (argc >= 3) {
        long v = atol(argv[2]);
        if (v <= 0) {
            fprintf(stderr, "Error: n_samples must be > 0 (got '%s').\n", argv[2]);
            return EXIT_FAILURE;
        }
        n = (size_t)v;
        if (!fft_is_power_of_two(n)) {
            fprintf(stderr, "Error: n_samples must be a power of 2 (got %zu).\n", n);
            return EXIT_FAILURE;
        }
    }
    if (argc >= 4) {
        outfile = argv[3];
    }

    // Nyquist check
    double nyquist = sample_rate / 2.0;
    printf("Configuration\n");
    printf("  Sample rate : %.1f Hz\n", sample_rate);
    printf("  Samples (n) : %zu\n",     n);
    printf("  Nyquist     : %.1f Hz\n", nyquist);
    printf("  Output file : %s\n\n",    outfile);

    // Build synthetic signal
    srand((unsigned)time(NULL));

    // Component frequencies and amplitudes
    typedef struct { double freq; double amp; } Component;
    Component components[] = {
        { 50.0,  1.00 },
        { 120.0, 0.50 },
        { 305.2, 0.25 },
    };
    size_t n_components = sizeof(components) / sizeof(components[0]);

    // Warn if any component exceeds Nyquist
    for (size_t c = 0; c < n_components; c++) {
        if (components[c].freq > nyquist) {
            fprintf(stderr,
                "Warning: component %.1f Hz exceeds Nyquist (%.1f Hz) "
                "and will alias.\n",
                components[c].freq, nyquist);
        }
    }

    double *signal = malloc(n * sizeof(double));
    if (!signal) {
        fprintf(stderr, "Error: out of memory.\n");
        return EXIT_FAILURE;
    }

    double dt           = 1.0 / sample_rate;
    double noise_level  = 0.2;

    for (size_t i = 0; i < n; i++) {
        double t = i * dt;
        double s = 0.0;
        for (size_t c = 0; c < n_components; c++) {
            s += components[c].amp * sin(2.0 * M_PI * components[c].freq * t);
        }
        s += noise_level * rand_noise();
        signal[i] = s;
    }

    printf("Signal generated: %zu samples, %.4f s duration.\n",
           n, n * dt);
    printf("Components: ");
    for (size_t c = 0; c < n_components; c++) {
        printf("%.0f Hz (A=%.2f)%s",
               components[c].freq,
               components[c].amp,
               c + 1 < n_components ? " + " : "");
    }
    printf(" + %.2f noise\n\n", noise_level);

    // Compute FFT
    FFTResult *result = fft_compute(signal, n, sample_rate);
    free(signal);

    if (!result) {
        fprintf(stderr, "FFT computation failed.\n");
        return EXIT_FAILURE;
    }

    printf("FFT complete: %zu frequency bins (resolution %.4f Hz/bin).\n",
           result->n_bins,
           sample_rate / (double)n);

    // Write CSV
    if (fft_write_csv(result, outfile) == 0) {
        printf("\nFull spectrum written to '%s'.\n", outfile);
    } else {
        fprintf(stderr, "Failed to write CSV.\n");
        fft_result_free(result);
        return EXIT_FAILURE;
    }

    fft_result_free(result);
    return EXIT_SUCCESS;
}
