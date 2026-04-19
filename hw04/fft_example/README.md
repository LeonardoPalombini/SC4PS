# 1D FFT Analysis — C / GSL

A small C project that performs 1-D FFT analysis of signals using
[GNU Scientific Library (GSL)](https://www.gnu.org/software/gsl/).
Compiled using CMake (probably overkill but force of habit).

## Project layout

```
fft_project/
├── CMakeLists.txt   – build system
├── fft_utils.h      – FFT utility API
├── fft_utils.c      – FFT utility implementation
├── fft_example.c    – example: synthetic signal → FFT → CSV
└── README.md
```

## Dependencies

| Dependency | Ubuntu/Debian            | Fedora/RHEL              |
|------------|--------------------------|--------------------------|
| CMake ≥ 3.14 | `cmake`                | `cmake`                  |
| GSL        | `libgsl-dev`             | `gsl-devel`              |
| C compiler | `gcc` / `clang`          | `gcc` / `clang`          |

```bash
# Ubuntu / Debian
sudo apt install cmake libgsl-dev gcc

# Fedora / RHEL
sudo dnf install cmake gsl-devel gcc
```

## Build

```bash
mkdir build && cd build
cmake .. && make
```

## Run

```bash
# Default: 1000 Hz sample rate, 1024 samples, output → fft_output.csv
./build/fft_example

# Custom: 8000 Hz, 2048 samples, custom output file
./build/fft_example 8000 2048 my_result.csv
```

### Positional arguments

| Position | Default          | Description                          |
|----------|------------------|--------------------------------------|
| 1        | `1000`           | Sampling frequency (Hz)              |
| 2        | `1024`           | Number of samples (must be power of 2) |
| 3        | `fft_output.csv` | Output CSV file path                 |

## Output CSV

The output file has three columns:

```
frequency_hz,magnitude,phase_rad
0.000000,0.0000000042,0.7853981634
0.976563,0.0000123456,1.2345678901
...
```

Only the one-sided spectrum is written (DC through Nyquist), giving
`n/2 + 1` rows.

## API

```c
// Compute FFT of real signal; returns NULL on error.
FFTResult *fft_compute(const double *signal, size_t n, double sample_rate);

// Write result to CSV; returns 0 on success, -1 on error.
int fft_write_csv(const FFTResult *result, const char *filepath);

// Free result.
void fft_result_free(FFTResult *result);

// Returns 1 if n is a power of 2.
int fft_is_power_of_two(size_t n);
```

The `FFTResult` struct:

```c
typedef struct {
    double *frequencies;  // Hz, length n_bins
    double *magnitudes;   // normalised amplitude, length n_bins
    double *phases;       // radians, length n_bins
    size_t  n_bins;       // n/2 + 1
} FFTResult;
```

## Synthetic signal (example)

The example generates a sum of three sinusoids plus noise:

```
s(t) = 1.00·sin(2π·50t) + 0.50·sin(2π·120t) + 0.25·sin(2π·300t) + ε
```

The FFT should clearly recover peaks at 50 Hz, 120 Hz, and 300 Hz.

## Plotting with python

The generated FFT spectrum (.csv) can be plotted using a Python script.
First, the necessary packages must be installed
```
pip install numpy pandas matplotlib scipy
``` 

To get the plot:
```
python plot_fft.py [fft_output.csv] [--max-freq 500] [--no-phase] [--save out.png]
```
 
Arguments:
```
    csv_file        Path to the CSV file (default: fft_output.csv)
    --max-freq HZ   Truncate the x-axis at this frequency (default: full range)
    --no-phase      Do not plot the phase spectrum
    --save FILE     Save the figure to a file instead of displaying it
```