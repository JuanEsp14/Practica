/**
- Se asume matriz cuadrada y tamaño múltiplo de la cantidad de procesos
- Compilar: mpicc -o mpi_ej1 mpi_ej1.c
- Ejecutar: mpirun -np CANTIDAD DE PROCESOS mpi_ej1 N
**/
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

/* Time in seconds from some point in the past */
double dwalltime();

void master(int N, int cantProcesos);
void procesos(int N, int cantProcesos);

int main(int argc, char **argv){
	int N;
	N= atoi(argv[1]);
    int id, cantProcesos;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);
  	

    if(id==0){
    	master(N, cantProcesos);
    }
    else 
    	procesos(N, cantProcesos);
    MPI_Finalize();
    return 0;
}

void master(int N, int cantProcesos){
	int i, j, k;
	unsigned long cant;
	int check=1;
	double timetick;

	int *A=(int*) malloc(sizeof(int)*N*N);
	int *B=(int*) malloc(sizeof(int)*N*N);
	int *C=(int*) malloc(sizeof(int)*N*N);
	int *A_aux=(int*) malloc(sizeof(int)*(N/cantProcesos)*N);
	int *C_aux=(int*) malloc(sizeof(int)*(N/cantProcesos)*N);



	 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			A[i*N+j]=1;
			B[i+j*N]=1;
		}
	}   
	timetick = dwalltime();
	/**Divide a la matriz A por la cantidad de procesos, y envia
	a cada proceso y a él mismo una cantidad N/cantProcesos de filas**/
	MPI_Scatter(A, (N/cantProcesos)*N, MPI_INT, A_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	cant=N*N;
	//Envia a todos los procesos la matriz B entera (cantidad de elementos de tipo int NxN)
	MPI_Bcast(B,cant, MPI_INT,0,MPI_COMM_WORLD);

	//Multiplicación de la parte de la matriz que le toca (en este caso la primera)
	for (i=0; i<N/cantProcesos; i++){
	  for (j=0; j<N; j++)  {
	    C_aux[i*N+j] = 0;
	    for (k=0; k<N; k++)
	      C_aux[i*N+j] += A_aux[i*N+k] * B[k*N+j];
	  }
	}
	/** Cada proceso envia al master su porción de matriz resultado, se unifica en
	la matriz resultante C en órden según el id del proceso que envía**/
	MPI_Gather(C_aux, (N/cantProcesos)*N, MPI_INT, C, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
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
	
		
}

void procesos(int N, int cantProcesos){
	int i, j, k;
	int *B=(int*) malloc(sizeof(int)*N*N);
	int *C_aux=(int*) malloc(sizeof(int)*(N/cantProcesos)*N);
	int *A_aux=(int*) malloc(sizeof(int)*(N/cantProcesos)*N);
	//A y C se declaran para que no de error de compilación en Scatter y Gatter respectivamente, por eso no se alocan
	int **A;
	int **C;

	int id;
	unsigned long cant;

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	//Recibe la parte de la matriz A y la guarda en A_aux
	MPI_Scatter(A, (N/cantProcesos)*N, MPI_INT, A_aux, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);
	cant=N*N;
	//Recibe la matriz B entera
	MPI_Bcast(B,cant, MPI_INT,0,MPI_COMM_WORLD);

	//Multiplicación de la porción de matriz que le corresponde
	for (i=0; i<N/cantProcesos; i++){
	  for (j=0; j<N; j++)  {
	    C_aux[i*N+j] = 0;
	    for (k=0; k<N; k++)
	      C_aux[i*N+j] += A_aux[i*N+k] * B[k*N+j];
	  }
	}
	
	/** Cada proceso envia al master su porción de matriz resultado, se unifica en
	la matriz resultante C en órden según el id del proceso que envía**/
	MPI_Gather(C_aux, (N/cantProcesos)*N, MPI_INT, C, (N/cantProcesos)*N, MPI_INT, 0, MPI_COMM_WORLD);

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


