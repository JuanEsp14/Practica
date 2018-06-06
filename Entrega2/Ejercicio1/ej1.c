/**
- Compilar: mpicc -o mpi_ej1 mpi_ej1.c
- Ejecutar: mpirun -np CANTIDAD DE PROCESOS mpi_ej1 N
**/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


/* Time in seconds from some point in the past */
double dwalltime();
void master(int N, int cantProcesos);
void procesos(int N, int cantProcesos);

int main(int argc, char **argv){
  int N, id, cantProcesos;
  N= atoi(argv[1]);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);
 //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Faltan argumentos:: N dimension de la matriz\n");
  }
  if(id==0)
    master(N, cantProcesos);
  else
    procesos(N, cantProcesos);
    MPI_Finalize();
    return 0;

}


void master(int N, int cantProcesos){
  double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k;
  int check=1;
  float promL, promU, promLU, divide;
  int parcialL;
  unsigned long totalL;
  double timetick;




  divide=0;

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  L=(double*)malloc(sizeof(double)*N*N);
  M=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N/2)*(N+1)); //se alocal sólo los 1
  A_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  D_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  L_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  M_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux1=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux2=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux3=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);

//Inicializa las matrices A, B,C, D  en 1
//Inicializa la matriz L con unos en el triangulo inferior y ceros en el triangulo superior.
//Inicializa la matriz U con unos en el triangulo superior y ceros en el triangulo inferior.
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
	     A[i*N+j]=1;
	     B[i+j*N]=1;
	     C[i+j*N]=1;
       D[i*N+j]=1;

       M[i*N+j]=0;
       if(j>=i)
        U[i+j*(j+1)/2]=1; //no se almacenan los ceros

       if(i>=j){
        L[i*N+j]=1;
       }else{
        L[i*N+j]=0;
       }
    }
  }


//Inicializo promedios de las matrices U y L
  promU = 0;
  parcialL=0;

  timetick = dwalltime();

  MPI_Scatter(A, (N/cantProcesos)*N, MPI_DOUBLE, A_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(D, (N/cantProcesos)*N, MPI_DOUBLE, D_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(L, (N/cantProcesos)*N, MPI_DOUBLE, L_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(M, (N/cantProcesos)*N, MPI_DOUBLE, M_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(U,(N/2)*(N+1), MPI_DOUBLE,0,MPI_COMM_WORLD);


  //Suma el total de la matriz triangular U
  for(i=0;i<N;i++){
    for(j=i;j<N; j++){
      promU = promU + U[i+j*(j+1)/2]; //suma todos
    }
  }
  //Suma el total de la matriz inferior L
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N; j++){
      parcialL = parcialL + L_aux[i*N+j]; //suma sólo la parte que le toca
    }
  }
  //Multiplica D_aux*U
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux3[i*N+j]=0;
        for(k=0; k<=j; k++){
            aux3[i*N+j]=aux3[i*N+j]+D_aux[i*N+k]*U[k+j*(j+1)/2];
        }
    }
  }
  //Multiplica L_aux*C
  printf("L*C\n");
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=0; k<j; k++){
          aux2[i*N+j]=aux2[i*N+j]+L_aux[i*N+k]*C[k+N*j];
        }
        printf("%f ", aux2[i*N+j]);

    }
    printf("\n");
  }
  //Multiplica A_aux*B
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux1[i*N+j]=0;
        for(k=0; k<N; k++){
          aux1[i*N+j]=aux1[i*N+j]+A_aux[i*N+k]*B[k+N*j];
        }
    }
  }


  

//Calculo los promedios
  divide = 1.0/(N*N);
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  promL = totalL*divide;
  printf("Promedio L master: %f\n",promL );
  promU = promU*divide;
  printf("Promedio U master: %f\n", promU);
  promLU = promU*promL;
  printf("promLU master: %f\n", promLU );

//Sumo los 3 valores
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      M_aux[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
    }
  }
  printf("Matriz aux:\n");
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      printf("%f ", M_aux[i*N+j] );
    }
    printf("\n");
  }
  
  for(i=0;i<N/cantProcesos;i++){
     for(j=0;j<N;j++){
       M_aux[i*N+j]=M_aux[i*N+j]*promLU;
    }
  }
  MPI_Gather(M_aux, (N/cantProcesos)*N, MPI_DOUBLE, M, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  printf("Tiempo en segundos %f \n", dwalltime() - timetick);
 
  printf("Matriz resultado:\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("%f ", M[i*N+j] );
    }
    printf("\n");
  }

  free(A);
  free(B);
  free(C);
  free(D);
  free(L);
  free(U);
  free(M);
  free(M_aux);
  free(A_aux);
  free(D_aux);
  free(L_aux);
  free(aux1);
  free(aux2);
  free(aux3);
}


void procesos(int N, int cantProcesos){
 double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k;
  float promL, promU, promLU, divide;
  unsigned long totalL;
  int parcialL;

//Inicializo promedios de las matrices U y L
  promU = 0;
  parcialL=0;


  divide=0;

 //Aloca memoria para las matrices
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  U=(double*)malloc(sizeof(double)*(N/2)*(N+1)); //se alocal sólo los 1
  A_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  D_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  L_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  M_aux=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux1=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux2=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);
  aux3=(double*)malloc(sizeof(double)*(N/cantProcesos)*N);

  MPI_Scatter(A, (N/cantProcesos)*N, MPI_DOUBLE, A_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(D, (N/cantProcesos)*N, MPI_DOUBLE, D_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(L, (N/cantProcesos)*N, MPI_DOUBLE, L_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(M, (N/cantProcesos)*N, MPI_DOUBLE, M_aux, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  MPI_Bcast(U,(N/2)*(N+1), MPI_DOUBLE,0,MPI_COMM_WORLD);


  //Suma el total de la matriz triangular U
  for(i=0;i<N;i++){
    for(j=i;j<N; j++){
      promU = promU + U[i+j*(j+1)/2]; //suma todos
    }
  }
  //Suma el total de la matriz inferior L
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N; j++){
      parcialL = parcialL + L_aux[i*N+j]; //suma sólo la parte que le toca
    }
  }
  //Multiplica D_aux*U
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux3[i*N+j]=0;
        for(k=0; k<=j; k++){
            aux3[i*N+j]=aux3[i*N+j]+D_aux[i*N+k]*U[k+j*(j+1)/2];
        }
    }
  }
  //Multiplica L_aux*C
  printf("L*C\n");
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=0; k<j; k++){
          aux2[i*N+j]=aux2[i*N+j]+L_aux[i*N+k]*C[k+N*j];
        }
        printf("%f ", aux2[i*N+j]);

    }
    printf("\n");
  }
  //Multiplica A_aux*B
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux1[i*N+j]=0;
        for(k=0; k<N; k++){
          aux1[i*N+j]=aux1[i*N+j]+A_aux[i*N+k]*B[k+N*j];
        }
    }
  }


  

//Calculo los promedios
  divide = 1.0/(N*N);
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  promL = totalL*divide;
  printf("Promedio L: %f\n",promL );
  promU = promU*divide;
  printf("Promedio U: %f\n", promU);
  promLU = promU*promL;
  printf("promLU: %f\n", promLU );

//Sumo los 3 valores
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      M_aux[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
    }
  }
  printf("Matriz aux:\n");
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      printf("%f ", M_aux[i*N+j] );
    }
    printf("\n");
  }
  
  for(i=0;i<N/cantProcesos;i++){
     for(j=0;j<N;j++){
       M_aux[i*N+j]=M_aux[i*N+j]*promLU;
    }
  }
  MPI_Gather(M_aux, (N/cantProcesos)*N, MPI_DOUBLE, M, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  free(B);
  free(C);
  free(U);
  free(A_aux);
  free(D_aux);
  free(L_aux);
  free(M_aux);
  free(aux1);
  free(aux2);
  free(aux3);
}

/****************************************************************************/

#include <sys/time.h>

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}
