# Legendre polynomials calculation

C script to check the numerical stability of Legendre foward and backward recurrence

## Prerequisites

High precision compunting library:

```
sudo dnf install mpfr-devel
```

## Compilation
```
gcc legendrestability.c -o legendre -lmpfr
```

## Running

Arguments:
- infile: input file name "file.txt" containing the x values at which polynomials are computed
- l: order of the polynomial
- L: start order for the backward recurrence
- outfile: output file base name "file", will result in a filename of this format "file_l.csv" (encodes the value of l)

To run:

```
./legendre <infile.txt> <l> <L> <outfile>
```

## Plotting

```
python pyplotting.py <results.csv>
```