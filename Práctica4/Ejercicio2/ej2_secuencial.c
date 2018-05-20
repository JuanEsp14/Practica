#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Time in seconds from some point in the past */
double dwalltime();



int main(int argc, char **argv){
	int N, i, j, k;
	double timetick;
	N= atoi(argv[1]);
	int *A=(int*) malloc(sizeof(int)*N*N);
	int *B=(int*) malloc(sizeof(int)*N*N);
	int max=-1;
	int min=999;
	unsigned long promedio;
	/* Inicializa Matriz A con números random del 0 al 9*/
	srand(time(NULL));
	printf("Matriz A:\n"); 
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j]=rand()%10;
			printf("%d ", A[i*N+j]);
		}
		printf("\n");
	}
	
	timetick = dwalltime();
	/* --PREGUNTAR: Es necesario guardar el valor de la 
	matriz en una variable por cada posición o es lo mismo? */
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			if(A[i*N+j]>max)
				max=A[i*N+j];	
			if(A[i*N+j]<min)
				min=A[i*N+j];
			promedio+=A[i*N+j];
		}
	}		
	
	promedio=promedio/(N*N);
	printf("Promedio: %d\n", promedio);
	printf("Mínimo: %d\n", min);
	printf("Máximo: %d\n", max);

	for(i=0;i<N;i++){
		for(j=0; j<N;j++){
			if(A[i*N+j]<promedio)
				B[i*N+j]=min;
			if(A[i*N+j]>promedio)
				B[i*N+j]=max;
			if(A[i*N+j]==promedio)
				B[i*N+j]=promedio;
			printf("%d ", B[i*N+j]);
		}
		printf("\n");
	}
	
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);

    free(A);
    free(B);
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


