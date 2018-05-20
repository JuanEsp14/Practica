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

	int *max_min=(int*) malloc(sizeof(int)*2);
	int *promedioSend=(int*) malloc(sizeof(int));
	unsigned long *totalR=(unsigned long*) malloc (sizeof(unsigned long));
	MPI_Status status;


	int max=-1;
	int min=999;
	unsigned long total=0;
	int promedio;
	/* Inicializa Matriz A con números random del 0 al 9*/
	srand(time(NULL));
	// -- PARA PRUEBAS N CHICO: printf("Matriz A:\n"); 
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			A[i*N+j]=rand()%10;
			// -- PARA PRUEBAS N CHICO: printf("%d ", A[i*N+j]);
		}
		// -- PARA PRUEBAS N CHICO:printf("\n");
	}
	
	timetick = dwalltime();
	MPI_Scatter(A, (N/cantProcesos)*N, MPI_INT, A_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

	/* --PREGUNTAR: Es necesario guardar el valor de la 
	matriz en una variable por cada posición o es lo mismo? */
	for(i=0;i<N/cantProcesos;i++){
		for(j=0;j<N;j++){
			if(A_aux[i*N+j]>max)
				max=A_aux[i*N+j];	
			if(A_aux[i*N+j]<min)
				min=A_aux[i*N+j];
			total+=A_aux[i*N+j];
		}
	}		
	/* --CODIGO PARA PRUEBAS CON N CHICO--*/
	/*
	printf("Máximo del proceso: %d: %d \n", 0, max);
	printf("Mínimo del proceso: %d: %d \n", 0, min);
	printf("Total del proceso: %d: %d \n", 0, total);
	*/

	/* -- Acá tengo dudas si se puede enviar todo en un mensaje, porque max y min son
	int pero el promedio es unsigned long. A su vez no se si es correcto enviar max y 
	min en el mismo mensaje*/
	for(i=1;i<cantProcesos;i++){
		MPI_Recv(max_min,2,MPI_INT,i,99,MPI_COMM_WORLD, &status);
		if(max_min[0]>max)
			max=max_min[0];
		if(max_min[1]<min)
			min=max_min[1];
	}
	for(i=1;i<cantProcesos;i++){
		MPI_Recv(totalR,1,MPI_UNSIGNED_LONG,i,98,MPI_COMM_WORLD, &status);
		total+=totalR[0];
	}

	promedio=(int)total/(N*N);
	printf("Promedio: %d\n", promedio);
	printf("Mínimo: %d\n", min);
	printf("Máximo: %d\n", max);
	max_min[0]=max;
	max_min[1]=min;
	promedioSend[0]=promedio;

	MPI_Scatter(B, (N/cantProcesos)*N, MPI_INT, B_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	for(i=1;i<cantProcesos;i++){
		MPI_Send(max_min, 2, MPI_INT, i, 99, MPI_COMM_WORLD);
		MPI_Send(promedioSend, 1, MPI_INT, i, 98, MPI_COMM_WORLD);
	}	


	for(i=0;i<N/cantProcesos;i++){
		for(j=0; j<N;j++){
			if(A_aux[i*N+j]<promedio)
				B_aux[i*N+j]=min;
			if(A_aux[i*N+j]>promedio)
				B_aux[i*N+j]=max;
			if(A_aux[i*N+j]==promedio)
				B_aux[i*N+j]=promedio;
		}
	}

	MPI_Gather(B_aux, (N/cantProcesos)*N, MPI_INT, B, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	/*  --CODIGO PARA PRUEBAS CON N CHICO--*/
	/*
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			printf("%d ", B[i*N+j]);
		}	
		printf("\n");
	}
	*/
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);

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

	int *max_min=(int*) malloc(sizeof(int)*2);
	int *promedioSend=(int*) malloc(sizeof(int));
	unsigned long *totalR=(unsigned long*) malloc (sizeof(unsigned long));
	MPI_Status status;

	int max=-1;
	int min=999;
	unsigned long total=0;

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
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
		/* --CODIGO PARA PRUEBAS CON N CHICO--*/
	/*
	printf("Máximo del proceso: %d: %d \n", id, max);
	printf("Mínimo del proceso: %d: %d \n", id, min);
	printf("Total del proceso: %d: %d \n", id, total);
	*/
	max_min[0]=max;
	max_min[1]=min;
	totalR[0]=total;
	MPI_Send(max_min, 2, MPI_INT, 0, 99, MPI_COMM_WORLD);
	MPI_Send(totalR, 1, MPI_UNSIGNED_LONG, 0, 98, MPI_COMM_WORLD);	

	MPI_Scatter(B, (N/cantProcesos)*N, MPI_INT, B_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Recv(max_min,2,MPI_INT,0,99,MPI_COMM_WORLD, &status);
	MPI_Recv(promedioSend,1,MPI_INT,0,98,MPI_COMM_WORLD, &status);

	for(i=0;i<N/cantProcesos;i++){
		for(j=0; j<N;j++){
			if(A_aux[i*N+j]<promedioSend[0])
				B_aux[i*N+j]=max_min[1];
			if(A_aux[i*N+j]>promedioSend[0])
				B_aux[i*N+j]=max_min[0];
			if(A_aux[i*N+j]==promedioSend[0])
				B_aux[i*N+j]=promedioSend[0];
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



