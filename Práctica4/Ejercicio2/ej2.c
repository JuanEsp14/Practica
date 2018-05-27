#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

/* Time in seconds from some point in the past */
double dwalltime();
void master(int N, int cantProcesos);
void procesos(int N, int cantProcesos);


int main(int argc, char **argv){
	int N, i, j, k, id, cantProcesos;
	N= atoi(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

	if(id==0)
		master(N, cantProcesos);
	else
		procesos(N, cantProcesos);
    MPI_Finalize();
    return 0;

}

void master(int N, int cantProcesos){
	int i, j;
	double timetick;
	int *A=(int*) malloc(sizeof(int)*N*N);
	int *B=(int*) malloc(sizeof(int)*N*N);
	int *A_aux=(int*) malloc(sizeof(int)*N*N);
	int *B_aux=(int*) malloc(sizeof(int)*N*N);


	MPI_Status status;

	/*Inicializa la matriz A con valores random del 0 al 9*/
	srand(time(NULL));
	//-- Para probar con N chico: printf("Matriz A:\n"); 
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j]=rand()%10;
			//-- Para probar con N chico: printf("%d ", A[i*N+j]);
		}
		//-- Para probar con N chico: printf("\n");
	}
	/*Variables locales al proceso*/
	int max=-1;
	int min=999;
	unsigned long total=0;
	int promedio;

	/*Variables utilzadas para el calculo total*/
	int maxT, minT;
	unsigned long totalT;


	
	timetick = dwalltime();
	/*Divide matriz A dando una parte a cada proceso que almacenará en A_aux*/
	MPI_Scatter(A, (N/cantProcesos)*N, MPI_INT, A_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);


	/*Calcula máximo, mínimo y suma total de la parte que le tocó al proceso*/
	for(i=0;i<N/cantProcesos;i++){
		for(j=0;j<N;j++){
			if(A_aux[i*N+j]>max)
				max=A_aux[i*N+j];	
			if(A_aux[i*N+j]<min)
				min=A_aux[i*N+j];
			total+=A_aux[i*N+j];
		}
	}		

	/*Calcula el máximo total (maxT) entre los máximos locales (max)*/
	MPI_Allreduce(&max, &maxT, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	/*Calcula el mínimo total (minT) entre los mínimos locales (min)*/
	MPI_Allreduce(&min, &minT, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	/*Calcula la suma total (totalT) entre las sumas locales (total)*/
	MPI_Allreduce(&total, &totalT, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	
	//Calcula el promedio general
	promedio=(int)totalT/(N*N);
	
	printf("Promedio: %d\n", promedio);
	printf("Mínimo: %d\n", minT);
	printf("Máximo: %d\n", maxT);
	
	//Divide la matriz B en partes guardadas en B_aux
	MPI_Scatter(B, (N/cantProcesos)*N, MPI_INT, B_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

	/*Si A en la posición evaluada es menor al promedio, se asigna en B el mínimo general,
	si es mayor al promedio se asigna el máximo general y si es igual al promedio general
	se asigna el promedio*/
	
	for(i=0;i<N/cantProcesos;i++){
		for(j=0; j<N;j++){
			if(A_aux[i*N+j]<promedio)
				B_aux[i*N+j]=minT;
			if(A_aux[i*N+j]>promedio)
				B_aux[i*N+j]=maxT;
			if(A_aux[i*N+j]==promedio)
				B_aux[i*N+j]=promedio;
		}
	}

	/* Se reunen en B todas las partes divididas en B_aux*/
	MPI_Gather(B_aux, (N/cantProcesos)*N, MPI_INT, B, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

	
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);
	/*-- Para probar con N chico: 
    printf("Matriz:B: \n");
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			printf("%d ", B[i*N+j]);
		}	
		printf("\n");
	}
	--*/
	
    free(A);
    free(B);
    free(A_aux);
    free(B_aux);
    
}

void procesos(int N, int cantProcesos){
	int i,j, id;
	int *A;
	int *B;
	int *A_aux=(int*) malloc(sizeof(int)*N*N);
	int *B_aux=(int*) malloc(sizeof(int)*N*N);


	MPI_Status status;

	int max=-1;
	int min=999;
	unsigned long total=0;
	int promedio;
	int maxT, minT;
	unsigned long totalT;
	MPI_Scatter(A, (N/cantProcesos)*N, MPI_INT, A_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

	for(i=0;i<N/cantProcesos;i++){
		for(j=0;j<N;j++){
			if(A_aux[i*N+j]>max)
				max=A_aux[i*N+j];	
			if(A_aux[i*N+j]<min)
				min=A_aux[i*N+j];
			total+=A_aux[i*N+j];
		}
	}	



	MPI_Allreduce(&max, &maxT, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(&min, &minT, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	MPI_Allreduce(&total, &totalT, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	promedio=(int)totalT/(N*N);
	
	
	MPI_Scatter(B, (N/cantProcesos)*N, MPI_INT, B_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

	
	for(i=0;i<N/cantProcesos;i++){
		for(j=0; j<N;j++){
			if(A_aux[i*N+j]<promedio)
				B_aux[i*N+j]=minT;
			if(A_aux[i*N+j]>promedio)
				B_aux[i*N+j]=maxT;
			if(A_aux[i*N+j]==promedio)
				B_aux[i*N+j]=promedio;
		}
	}

	MPI_Gather(B_aux, (N/cantProcesos)*N, MPI_INT, B, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	free(A_aux);
    free(B_aux);

	
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