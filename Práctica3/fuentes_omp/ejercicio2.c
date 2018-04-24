//Ejercicio 2
#include "stdio.h" 	
#include "stdlib.h"
#include "omp.h"
#include "math.h"


int main(int argc,char*argv[]){
 double x,scale, resultado;
 int i;
 int numThreads = atoi(argv[2]);
 int N=atoi(argv[1]);
 omp_set_num_threads(numThreads);
 scale=2.78;
 x=0.0;
 resultado=0;

 #pragma omp parallel for private (x) shared (resultado)
 for(i=1;i<=N;i++){
	x= x + sqrt(i*scale) + 2*x;
 }
	#pragma omp critical
	resultado+=x;

 printf("\n Resultado: %f \n",resultado);

 return(0);
}

