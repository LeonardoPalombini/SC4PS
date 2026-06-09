#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>

#define MAX_SIZE 100

int read_values_from_file(const char* filename, float array[MAX_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    int count = 0;
    float temp;

    // Read values line by line
    while (fscanf(file, "%f", &temp) == 1) {
        if (count < MAX_SIZE) {
            array[count] = temp;
            count++;
        } else {
            // Found more than 100 values, print warning and break early
            printf("Warning: File contains more than %d values. Only the first %d were loaded.\n", MAX_SIZE, MAX_SIZE);
            break;
        }
    }

    fclose(file);
    return count; // Returns how many elements were actually stored
}


int main(int argc, char *argv[])
{
    // Parse optional command-line arguments
    float xs[MAX_SIZE];
    const char *infile = "legendre_x.txt";
    int ll = 10;
    int startL = 80;
    const char *outfilehead = "legendre_output";

    if (argc >= 2) infile = argv[1];
    if (argc >= 3) ll = atof(argv[2]);
    if (argc >= 4) startL = atol(argv[3]);
    if (argc >= 5) outfilehead = argv[4];

    int nvals = read_values_from_file(infile, xs);

    if (ll > startL) {
        fprintf(stderr, "Error: l must be smaller than startL\n");
        return EXIT_FAILURE;
    }

    char outfile[50];
    snprintf(outfile, sizeof(outfile), "%s_%d.csv", outfilehead, ll);

    FILE* file = fopen(outfile, "w");
    if (!file) {
        perror("Error opening file for writing");
        return EXIT_FAILURE;
    }
    
    fprintf(file, "x,fwd128,fwd32,bkw128,bkw32\n");


    for(int i = 0; i < nvals; i++)
    {
        float x = xs[i];
        printf("==> Calculating x = %.5f ...\n\n", x);
        fprintf(file, "%4f,", x);

        //exact numbers init
        mpfr_t x_128;
        mpfr_init2(x_128, 128); 
        mpfr_set_d(x_128, x, MPFR_RNDN);

        mpfr_t P_128;
        mpfr_init2(P_128, 128); 
        mpfr_set_d(P_128, 0.0, MPFR_RNDN);

        mpfr_t PL_128;
        mpfr_init2(PL_128, 128); 
        mpfr_set_d(PL_128, x, MPFR_RNDN);

        mpfr_t PL1_128;
        mpfr_init2(PL1_128, 128); 
        mpfr_set_d(PL1_128, 1.0, MPFR_RNDN);

        mpfr_t l_128;
        mpfr_init2(l_128, 128); 
        mpfr_set_d(l_128, 1.0, MPFR_RNDN);

        mpfr_t u_128;
        mpfr_init2(u_128, 128); 
        mpfr_set_d(u_128, 1.0, MPFR_RNDN);

        mpfr_t t_128;
        mpfr_init2(t_128, 128); 
        mpfr_set_d(t_128, 2.0, MPFR_RNDN);

        mpfr_t temp1;
        mpfr_init2(temp1, 128); 

        mpfr_t temp2;
        mpfr_init2(temp2, 128); 

        //standard numbers init
        float ldb;
        float P;
        float Pl = x, Pl1 = 1.;

        //forward iteration
        for(int l = 1; l < ll; l++)
        {
            //EXACT 128 bit
            //calculate P(l+1)
            mpfr_mul(temp1, t_128, l_128, MPFR_RNDN);
            mpfr_add(temp1, temp1, u_128, MPFR_RNDN);
            mpfr_add(temp2, l_128, u_128, MPFR_RNDN);
            mpfr_div(temp1, temp1, temp2, MPFR_RNDN);

            mpfr_mul(temp1, temp1, x_128, MPFR_RNDN);
            mpfr_mul(temp1, temp1, PL_128, MPFR_RNDN);

            mpfr_div(temp2, l_128, temp2, MPFR_RNDN);
            mpfr_mul(temp2, temp2, PL1_128, MPFR_RNDN);

            //update P(l) and P(l-1) for next iter
            mpfr_set(PL1_128, PL_128, MPFR_RNDN);
            mpfr_sub(PL_128, temp1, temp2, MPFR_RNDN);

            // l+1 for next iter
            mpfr_add(l_128, l_128, u_128, MPFR_RNDN);

            //STANDARD 32 bit
            ldb = (float)l;
            P = ((2. * ldb + 1.) * x * Pl - ldb * Pl1) / (ldb + 1.);
            Pl1 = Pl;
            Pl = P;

            // printf("L= %d 32 bit:  %.10f\n", l+1, Pl);
        }

        printf("Fwd iter %d 128 bit: ", ll); mpfr_printf("%.15Rg\n", PL_128);
        printf("Fwd iter %d 32 bit:  %.15f\n\n", ll, Pl);

        mpfr_fprintf(file, "%.15Rg", PL_128);
        fprintf(file, ",%.15f,", Pl);

        // backwards propagation
        mpfr_set_d(l_128, (double)startL, MPFR_RNDN);
        mpfr_set_d(PL_128, 1.0, MPFR_RNDN);
        mpfr_set_d(P_128, 0.0, MPFR_RNDN);
        P = 0.0;
        Pl = 1.0;

        mpfr_t result_128;
        mpfr_init2(result_128, 128); 
        float result;

        for(int l = startL; l > 0; l--)
        {
            if(l == ll)
            {
                mpfr_set(result_128, PL_128, MPFR_RNDN);
                result = Pl;
            }

            //EXACT 128 bit
            //calculate P(l-1)
            mpfr_mul(temp1, t_128, l_128, MPFR_RNDN);
            mpfr_add(temp1, temp1, u_128, MPFR_RNDN);
            mpfr_div(temp1, temp1, l_128, MPFR_RNDN);

            mpfr_mul(temp1, temp1, x_128, MPFR_RNDN);
            mpfr_mul(temp1, temp1, PL_128, MPFR_RNDN);

            mpfr_add(temp2, l_128, u_128, MPFR_RNDN);
            mpfr_div(temp2, temp2, l_128, MPFR_RNDN);
            mpfr_mul(temp2, temp2, P_128, MPFR_RNDN);

            mpfr_set(P_128, PL_128, MPFR_RNDN);
            mpfr_sub(PL_128, temp1, temp2, MPFR_RNDN);

            // l-1 for next iter
            mpfr_sub(l_128, l_128, u_128, MPFR_RNDN);

            //STANDARD 32 bit
            ldb = (float)l;
            Pl1 = (2. * ldb + 1.) / ldb * x * Pl - (ldb + 1.) / ldb * P;
            P = Pl;
            Pl = Pl1;
        }
        
        mpfr_div(result_128, result_128, PL_128, MPFR_RNDN);
        result /= Pl;
        
        printf("Bkw iter %d 128 bit: ", ll); mpfr_printf("%.15Rg\n", result_128);
        printf("Bkw iter %d 32 bit:  %.15f\n\n", ll, result);

        mpfr_fprintf(file, "%.15Rg", result_128);
        fprintf(file, ",%.15f\n", result);
    }

    fclose(file);
    return 0;
}