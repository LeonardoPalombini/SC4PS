# HTF5 usage exercise

An example usage of the HFT5 filr format to store data.
The script performs a linear combination of two vectors $X,Y$ of length $len$:

$Z=aX+Y$

where $X_i = x$ and $Y_i = y$, and $a,x,y$ are floating point numbers. The operation can be performed in chunks of size $chu$.

The parameters are specified in a text file in this way:
```
#this is a comment
len=10
chu=2
a=1.0
x=1.0
y=1.0
```
The input parse is done via the INI library.

The result is written to a HTF5 file. (...)

## Compilation
```
gcc -o product_htf5 product_htf5.c ini.c -lm -lhdf5
```

## Usage
```
./product_htf5 inputs.txt output.h5
```

## Result check
```
h5dump output.h5
```