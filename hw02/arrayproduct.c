#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc != 5)
	{
		fprintf(stderr, "Usage: %s <N (int)> <a (double)> <x (double)> <y (double)>\n", argv[0]);
		return 1;
    }

	int N = (int)strtol(argv[1], NULL, 10);
	double a = strtod(argv[2], NULL);
	double x = strtod(argv[3], NULL);
	double y = strtod(argv[4], NULL);

	printf("N =  %d\n", N);
	printf("a = %.2f\n", a);
	printf("x = %.2f\n", x);
	printf("y = %.2f\n", y);

	double *arrx = malloc(N * sizeof *arrx);
  	double *arry = malloc(N * sizeof *arry);
	double *arrd = malloc(N * sizeof *arrd);
		
	for(int i = 0; i < N; i++)
	{
		arrx[i] = x;
		arry[i] = y;
	}

	for(int i = 0; i < N; i++)
	{
		arrd[i] = a * arrx[i] + arry[i];
	}

	printf("arrx first elem: %.2f\n", arrx[0]);    	
	printf("arry first elem: %.2f\n", arry[0]);
	printf("arrd first elem: %.2f\n", arrd[0]);

	free(arrx);
	free(arry);
	free(arrd);

	return 0;
}
