#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Time in seconds from some point in the past */
double dwalltime();



int main(int argc, char **argv){
	int N, i, j, k;
	int check=1;
	double timetick;
	N= atoi(argv[1]);
	int *A=(int*) malloc(sizeof(int)*N*N);
	int *B=(int*) malloc(sizeof(int)*N*N);
	int *C=(int*) malloc(sizeof(int)*N*N);
	//Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			A[i*N+j]=1;
			B[i+j*N]=1;
		}
	}  
	
	timetick = dwalltime();
	//multiplicación
	for (i=0; i<N; i++){
	  for (j=0; j<N; j++)  {
	    C[i*N+j] = 0;
	    for (k=0; k<N; k++)
	      C[i*N+j] += A[i*N+k] * B[k*N+j];
	  }
	}
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);

	//Verifica el resultado
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			check=check&&(C[i*N+j]==N);
		}
	}   

	if(check){
		printf("Multiplicacion de matrices resultado correcto\n");
	}else{
		printf("Multiplicacion de matrices resultado erroneo\n");
	}
	free(A);
	free(B);
	free(C);
    return 0;
}



/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}


