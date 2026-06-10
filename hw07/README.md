# Random number generator exercises

The "randomtest.c" script runs a series of random number test exercises:

- coin toss: computes the fraction of heads in a coin toss experiment, up to N tosses
- pi numerical calculation: computes the error on the numerical pi value, up to N iterations
- numerical PDF 1: starting from a uniformly distributed set of numbers $y \in [0,1]$, produces the set made of the squares of the values; This corresponds to the PDF: $1/2\sqrt(y)$
- numerical PDF 2: produces a set of real numbers $y$ reflecting the PDF $\lambda e^{-\lambda y}$, using the inverse CDF technique

## Compilation

```
gcc randomtests.c -o randomtests -lm
```

## Run

```
./randomtests <n. of coin tosses> <n. of pi iterations> <n. samples PDF1> <n. samples PDF2>
```

Produces 4 standard CSV output files containing the results (one for each exercise). The default number of iterations is $10^5$ for all.

## Reading the results

The results can be plotted using:

```
python plottests.py
```

Produces 5 plots in PNG format:

- cointosses.png :  trend of heads fraction as a function of the number of coin tosses
- pierrors.png : trend of pi value error as a function of the number of iterations
- changevars.png : PDF1 compared to its analytical PDF
- invtransform.png : PDF2 compared to its analytical PDF
- empiricalcdf.png : CDF2 produced from the PDF2 sample set, compared to its analytical CDF