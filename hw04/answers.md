# Exercise answers

## Part 1

- FFT shows the frequency spectrum of a signal
- Sampling rate = rate at which the signal is read-out, i.e. the time granularity of the data; Nyquist frequency = given a sampling rate, the NF is the largest frequency signal you can exactly reconstruct (sampling / 2); frequency resolution = granularity of the output FFT data (sampling / Npoints)
- DFT algorithms are mathematically formulated with uniformly sampled intervals

## Part 2

- Files in output:
```
coupled_oscillators_spectrum.csv
coupled_oscillators_time.csv
good_sampling_signal.csv
good_sampling_spectrum.csv
short_record_signal.csv
short_record_spectrum.csv
undersampled_signal.csv
undersampled_spectrum.csv
```

- sampling code:
```
sampling_demo.c
```

- physics code:
```
coupled_oscillators_fft.c
```

- good sampling output:
```
  sampling rate = 512.0 Hz, Nyquist = 256.0 Hz, duration = 1.000 s
  FFT frequency resolution = 1.000 Hz
  dominant spectral peaks:
    peak 1: f =   50.000 Hz, amplitude = 1.0000
    peak 2: f =  120.000 Hz, amplitude = 0.7000
```

## Part 3

- :heavy_check_mark: 
- 64 Hz
- 120 Hz is above the NF, so it is undersampled
- An undersampled high frequency can produce the same signal set as a well-sampled lower frequency (typical example of videos of helicopter rotors)

## Part 4

- :heavy_check_mark: 
- 512 Hz, 64 samples = 0.125 s
- 1 / 0.125s = 8 Hz
- the two frequencies are closer than the algorithm frequency spacing
- acquisition time, it needs to be at least >> 0.2 s (1/(freq. diff))

## Part 5

- EOM:
```
m x1'' = -(kwall + kc) x1 + kc x2
m x2'' =  kc x1 - (kwall + kc) x2
```
- Two identical masses, each on a spring to a wall, connected to each other by a coupling spring.
- In phase (oscillate together), out of phase (oscillate oppositely)
- They represent the frequencies associated to the two modes
- To check correct sampling rate and duration (not only)

## Part 6

- sampling_spectra.png fig 2
- sampling_spectra.png fig 3 (broad peak and points separated of 8 hz)
- very tiny difference
- frequency resolution of around 0.01 Hz

## Part 7 Option C

- Try to reach frequency resolution limit: kwall = 100 N/m, kc = 0.5 N/m, freq diff < 0.01 → peaks not resolved
- Try to reach aliasing (should onset for k>1000, Niquist abt 4 Hz): kwall=kc=2000 N/m, theo frequencies about 7 Hz and 12 Hz, measured frequencies about 0.9 Hz and 3.7 Hz.

## Part 8

I learned about the Nyquist theorem and its consequences in aliasing effects. From the POV of the software organization, I had a simple example of Make compilation.
