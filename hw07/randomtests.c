#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


static inline uint32_t lcg32_next(uint32_t *state){
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;
    *state = a * (*state) + c;
    return *state;
}

static inline double lcg32_next_double(uint32_t *state){
    return lcg32_next(state) / 4294967296.0;
}


double coinTossesTest(int n, const char* outfile){
    FILE* file = fopen(outfile, "w");
    fprintf(file, "tossIdx,frac\n");

    uint32_t state = 123456789u;
    int sum = 0;
    double frac = 0.;

    for(int i = 1; i < n+1; i++)
    {
        double rnd = lcg32_next_double(&state);
        if(rnd > 0.5) sum += 1;
        else sum += 0;

        frac = (double) sum / (double) i;

        int printfreq = (int) pow(10., floor( log10( (double)i ) ) - 1.);
        printfreq = (printfreq > 10) ? printfreq : 10;

        if(i % printfreq == 0) fprintf(file, "%d,%.10f\n", i, frac);
    }

    fclose(file);
    return frac;
}


double piTest(int n, const char* outfile){
    FILE* file = fopen(outfile, "w");
    fprintf(file, "nIter,error\n");

    uint32_t stateX = 123456789u;
    uint32_t stateY = 132547698u;
    int sum = 0;
    double err = 0.;

    double truePi = 3.141592653589793;

    for(int i = 1; i < n+1; i++){
        double x = lcg32_next_double(&stateX);
        double y = lcg32_next_double(&stateY);

        if(x*x + y*y < 1.0) sum += 1;

        int printfreq = (int) pow(10., floor( log10( (double)i ) ));
        printfreq = (printfreq > 10) ? printfreq : 10;

        if(i % printfreq == 0){
            double calcPi = 4.0 * (double) sum / (double) i;
            fprintf(file, "%d,%.15f\n", i, fabs(calcPi-truePi));
        }
    }

    fclose(file);
    return 4.0 * (double) sum / (double) n;
}


void testChangeVars(int n, const char* outfile){
    FILE* file = fopen(outfile, "w");

    uint32_t state = 123456789u;

    for(int i = 1; i < n+1; i++){
        double rnd = lcg32_next_double(&state);
        fprintf(file, "%.10f\n", i, rnd*rnd);
    }

    fclose(file);
    return;
}


void testInvTransform(int n, const char* outfile){
    FILE* file = fopen(outfile, "w");
    double lam = 1.5;

    uint32_t state = 123456789u;

    for(int i = 1; i < n+1; i++){
        double rnd = lcg32_next_double(&state);
        fprintf(file, "%.10f\n", i, -log(1-rnd)/lam);
    }

    fclose(file);
    return;
}



int main(int argc, char *argv[]){

    int nCoinTosses = 100000;
    int nPiIter = 100000;
    int nChangeVar = 100000;
    int nInvTransform = 100000;

    if (argc >= 2) nCoinTosses = atol(argv[1]);
    if (argc >= 3) nPiIter = atol(argv[2]);
    if (argc >= 4) nChangeVar = atol(argv[3]);
    if (argc >= 5) nInvTransform = atol(argv[4]);

    double tossFrac = coinTossesTest(nCoinTosses, "cointosses.csv");
    printf("Final toss fraction at N=%d: %.6f\n", nCoinTosses, tossFrac);

    double piCalc = piTest(nPiIter, "pierrors.csv");
    printf("Final pi value at N=%d: %.6f\n", nPiIter, piCalc);

    testChangeVars(nChangeVar, "changevars.csv");
    printf("Produced sample <changevar> N=%d\n", nChangeVar);

    testInvTransform(nInvTransform, "invtransform.csv");
    printf("Produced sample <invtransform> N=%d\n", nInvTransform);

    return 0;
}
