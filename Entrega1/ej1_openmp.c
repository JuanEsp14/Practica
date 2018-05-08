#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Time in seconds from some point in the past */
double dwalltime();


int main(int argc,char*argv[]){
 double *A,*resultado;
 int i,j,k,N;
 int check=1;
 double timetick;

 //Controla los argumentos al programa
  if (argc < 3){
   printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
   return 0;
  }
  N=atoi(argv[1]);
  int numThreads=atoi(argv[2]);
  omp_set_num_threads(numThreads);
 
   
 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  resultado=(double*)malloc(sizeof(double)*N*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=1;
      resultado[i*N+j]=0;
   }
  }   

  timetick = dwalltime();
 //Realiza la multiplicacion


//collapse(3) no anda con 2048
#pragma omp parallel for collapse(2) shared(A, resultado) private(i,j,k)
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    for(k=0;k<N;k++){
	     resultado[i*N+j]= resultado[i*N+j] + A[i*N+k]*A[k+j*N];
    }
   }
  }   
  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(resultado[i*N+j]==N);
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(A);
 free(resultado);
 return(0);
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


