#include <stdio.h>
#include <stdlib.h>

/* Time in seconds from some point in the past */
double dwalltime();

int main(int argc,char*argv[]){
 double *A, *resultado;
 int i,j,k;
 int check = 1; 
  double timetick;
 
 int N=atoi(argv[1]);
 
 //Aloca memoria para la matriz
  A=(double*)malloc(sizeof(double)*N*N);
  resultado=(double*)malloc(sizeof(double)*N*N);

//Inicializo matriz A
for(i=0;i<N;i++){
  for(j=0;j<N;j++){
    A[i*N+j]=1;
  }
}

timetick = dwalltime();
//multiplicación
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    resultado[i*N+j]=0;
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
