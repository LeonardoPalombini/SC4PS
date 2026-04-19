#include "fft_utils.h"

#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_complex_float.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* GSL interleaved complex layout: [re0, im0, re1, im1, ...] */
#define REAL(z, i) ((z)[2*(i)])
#define IMAG(z, i) ((z)[2*(i)+1])

/* -------------------------------------------------------------------------- */

int fft_is_power_of_two(size_t n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

/* -------------------------------------------------------------------------- */

FFTResult *fft_compute(const double *signal, size_t n, double sample_rate)
{
    if (!signal || n == 0 || sample_rate <= 0.0) {
        fprintf(stderr, "fft_compute: invalid arguments.\n");
        return NULL;
    }
    if (!fft_is_power_of_two(n)) {
        fprintf(stderr, "fft_compute: n=%zu is not a power of 2.\n", n);
        return NULL;
    }

    /* Allocate interleaved complex buffer and fill with real signal */
    double *data = calloc(2 * n, sizeof(double));
    if (!data) {
        fprintf(stderr, "fft_compute: out of memory (data buffer).\n");
        return NULL;
    }
    for (size_t i = 0; i < n; i++) {
        REAL(data, i) = signal[i];
        IMAG(data, i) = 0.0;
    }

    /* Allocate GSL wavetable and workspace */
    gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc(n);
    gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc(n);
    if (!wavetable || !workspace) {
        fprintf(stderr, "fft_compute: failed to allocate GSL structures.\n");
        free(data);
        if (wavetable) gsl_fft_complex_wavetable_free(wavetable);
        if (workspace) gsl_fft_complex_workspace_free(workspace);
        return NULL;
    }

    /* Execute forward FFT in-place */
    int rc = gsl_fft_complex_forward(data, /*stride=*/1, n, wavetable, workspace);
    gsl_fft_complex_wavetable_free(wavetable);
    gsl_fft_complex_workspace_free(workspace);
    if (rc != 0) {
        fprintf(stderr, "fft_compute: GSL FFT failed (code %d).\n", rc);
        free(data);
        return NULL;
    }

    /* Build one-sided result (DC through Nyquist) */
    size_t n_bins = n / 2 + 1;

    FFTResult *result = malloc(sizeof(FFTResult));
    if (!result) goto oom;
    result->n_bins     = n_bins;
    result->frequencies = malloc(n_bins * sizeof(double));
    result->magnitudes  = malloc(n_bins * sizeof(double));
    result->phases      = malloc(n_bins * sizeof(double));
    if (!result->frequencies || !result->magnitudes || !result->phases) goto oom;

    double freq_resolution = sample_rate / (double)n;

    for (size_t k = 0; k < n_bins; k++) {
        double re = REAL(data, k);
        double im = IMAG(data, k);

        result->frequencies[k] = k * freq_resolution;

        /* Normalise magnitude so it represents amplitude of the original signal.
         * One-sided bins (not DC / Nyquist) are doubled to account for the
         * negative-frequency mirror. */
        double mag = sqrt(re * re + im * im) / (double)n;
        if (k > 0 && k < n / 2)
            mag *= 2.0;
        result->magnitudes[k] = mag;

        result->phases[k] = atan2(im, re);
    }

    free(data);
    return result;

oom:
    fprintf(stderr, "fft_compute: out of memory (result struct).\n");
    free(data);
    if (result) {
        free(result->frequencies);
        free(result->magnitudes);
        free(result->phases);
        free(result);
    }
    return NULL;
}

/* -------------------------------------------------------------------------- */

int fft_write_csv(const FFTResult *result, const char *filepath)
{
    if (!result || !filepath) {
        fprintf(stderr, "fft_write_csv: invalid arguments.\n");
        return -1;
    }

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "fft_write_csv: cannot open '%s' for writing.\n", filepath);
        return -1;
    }

    fprintf(fp, "frequency_hz,magnitude,phase_rad\n");
    for (size_t i = 0; i < result->n_bins; i++) {
        fprintf(fp, "%.6f,%.10f,%.10f\n",
                result->frequencies[i],
                result->magnitudes[i],
                result->phases[i]);
    }

    fclose(fp);
    return 0;
}

/* -------------------------------------------------------------------------- */

void fft_result_free(FFTResult *result)
{
    if (!result) return;
    free(result->frequencies);
    free(result->magnitudes);
    free(result->phases);
    free(result);
}
