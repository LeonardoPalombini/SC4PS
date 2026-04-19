#ifndef FFT_UTILS_H
#define FFT_UTILS_H

#include <stddef.h>

/**
 * FFT result structure holding frequency, magnitude, and phase arrays.
 * All arrays are of length (n/2 + 1), covering the one-sided spectrum.
 */
typedef struct {
    double *frequencies;  /* Frequency bins in Hz              */
    double *magnitudes;   /* Magnitude of each frequency bin   */
    double *phases;       /* Phase (radians) of each bin       */
    size_t  n_bins;       /* Number of bins  = n/2 + 1         */
} FFTResult;

/**
 * Perform a 1D FFT on real-valued signal data.
 *
 * @param signal        Input signal samples (real-valued).
 * @param n             Number of samples (must be a power of 2).
 * @param sample_rate   Sampling frequency in Hz.
 * @return              Heap-allocated FFTResult, or NULL on error.
 *                      Caller must free with fft_result_free().
 */
FFTResult *fft_compute(const double *signal, size_t n, double sample_rate);

/**
 * Write an FFTResult to a CSV file.
 *
 * Columns: frequency_hz, magnitude, phase_rad
 *
 * @param result    FFTResult to serialise.
 * @param filepath  Destination file path.
 * @return          0 on success, -1 on error.
 */
int fft_write_csv(const FFTResult *result, const char *filepath);

/**
 * Free an FFTResult previously returned by fft_compute().
 */
void fft_result_free(FFTResult *result);

/**
 * Utility: return 1 if n is a power of 2, 0 otherwise.
 */
int fft_is_power_of_two(size_t n);

#endif /* FFT_UTILS_H */
