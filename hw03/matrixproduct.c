#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if(argc != 4){
		fprintf(stderr, "Usage: %s <N (int)> <a (double)> <b (double)>\n", argv[0]);
		return 1;
    }

	int N = (int)strtol(argv[1], NULL, 10);
	double a = strtod(argv[2], NULL);
	double b = strtod(argv[3], NULL);

    double *A = malloc(N * N * sizeof *A);
    double *B = malloc(N * N * sizeof *B);
    double *C = malloc(N * N * sizeof *C);
    double *CC = malloc(N * N * sizeof *CC);


    //fill with identity
    int k = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            k = N * i + j;

            if (i == j){
                A[k] = a;
                B[k] = b; 
            }
            else{
                A[k] = 0;   
                B[k] = 0;
            }

            C[k] = 0;
            CC[k] = 0;
        }
        
    }

    printf("\nA[0,0]=%.2f\n", A[0]);
    printf("B[0,0]=%.2f\n", B[0]);

    clock_t start = clock();

    //multiply naively
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            for(int l = 0; l < N; l++){
                C[i*N+j] += A[i*N+l] * B[l*N+j];
            }
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nNaive time: %f ms\n", elapsed*1000.0);

    start = clock();

    //multiply efficiently
    for(int i = 0; i < N; i++){
        for(int l = 0; l < N; l++){
            for(int j = 0; j < N; j++){
                CC[N*i+j] += A[i*N+l] * B[l*N+j];
            }
        }
    }

    end = clock();
    elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Efficient time: %f ms\n", elapsed*1000.0);

    printf("\nC[0,0]=%.2f\n", C[0]);
    printf("CC[0,0]=%.2f\n\n", CC[0]);

    free(A);
    free(B);
    free(C);
    free(CC);

    return 0;
}