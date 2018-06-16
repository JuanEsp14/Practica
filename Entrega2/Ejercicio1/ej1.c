/**
- Compilar: mpicc -o mpi_ej1 mpi_ej1.c
- Ejecutar: mpirun -np CANTidAD DE PROCESOS mpi_ej1 N
**/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


/* Time in seconds from some point in the past */
double dwalltime();
void master(int N, int cantProcesos, int id);
void procesos(int N, int cantProcesos, int id);

int main(int argc, char **argv){
  int N, id, cantProcesos;
  double time;
  MPI_Init(&argc, &argv);
  N= atoi(argv[1]);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);
 //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Faltan argumentos:: N dimension de la matriz\n");
  }
  if(id==0){
    time = dwalltime();
    master(N, cantProcesos, id);
    printf("Tiempo proceso %d: %f \n", id, dwalltime() - time);
  }
  else{
    time = dwalltime();    
    procesos(N, cantProcesos, id);
    printf("Tiempo proceso %d: %f \n", id, dwalltime() - time);
  }
  MPI_Finalize();
  return 0;

}


void master(int N, int cantProcesos, int id){
  double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k, inicio, fin, tamanoBloque, distribuido;
  int check=1;
  float promL, promU, promLU, divide;
  unsigned long totalU, parcialU, totalL, parcialL;
  double timetick;
  double timeComunic;
  double time;

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



  //Inicializo promedios de las matrices U y L, totales, parciales, inicio, fin y tamaño de bloque
  promU = 0;
  parcialL=0;
  parcialU = 0;
  totalL = 0;
  totalU = 0;
  divide=0;
  inicio = 0;
  fin = 0;
  tamanoBloque = 0;
  distribuido = N/cantProcesos;

  timetick = dwalltime();
  time = dwalltime();
  MPI_Scatter(A, (distribuido)*N, MPI_DOUBLE, A_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Scatter(D, (distribuido)*N, MPI_DOUBLE, D_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Scatter(L, (distribuido)*N, MPI_DOUBLE, L_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Scatter(M, (distribuido)*N, MPI_DOUBLE, M_aux, (distribuido)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Bcast(U,(N/2)*(N+1), MPI_DOUBLE,0,MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);


  inicio = 0;
  tamanoBloque = ((N/2)*(N+1))/(cantProcesos);
  fin = tamanoBloque;  
    

  //Suma el total de la matriz triangular U
  //Cada proceso suma una parte
  for(i=inicio;i<fin;i++){
    for(j=i;j<N; j++){
      parcialU = parcialU + U[i+j*(j+1)/2]; 
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
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=i; k<N; k++){
          aux2[i*N+j]=aux2[i*N+j]+L_aux[i*N+k]*C[k+N*j];
        }

    }
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
  time = dwalltime();
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  time = dwalltime();
  MPI_Allreduce(&parcialU, &totalU, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  timeComunic += (dwalltime() -time);
  divide = 1.0/(N*N);
  promL = totalL*divide;
  promU = totalU*divide;
  promLU = promU*promL;

//Sumo los 3 valores
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      M_aux[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
    }
  }
//Multiplico por el promedio calculado
  for(i=0;i<N/cantProcesos;i++){
     for(j=0;j<N;j++){
       M_aux[i*N+j]=M_aux[i*N+j]*promLU;
    }
  }

//Reuno en M la matriz total
  time = dwalltime();
  MPI_Gather(M_aux, (N/cantProcesos)*N, MPI_DOUBLE, M, (N/cantProcesos)*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  timeComunic=+(dwalltime()-time);
  printf("Tiempo total en segundos %f \n", dwalltime() - timetick);
  printf("Tiempo de las comunicaciones en segundos %f\n", timeComunic);
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


void procesos(int N, int cantProcesos, int id){
  double *A,*B,*C,*D, *L, *M, *U, *A_aux, *D_aux, *L_aux, *M_aux, *aux1, *aux2, *aux3;
  int i, j, k, inicio, fin, tamanoBloque, distribuido;
  float promL, promU, promLU, divide;
  unsigned long totalU, parcialU, totalL, parcialL;

  //Inicializo promedios de las matrices U y L, totales, parciales, inicio, fin y tamaño de bloque
  promU = 0;
  parcialL = 0;
  totalL = 0;
  parcialU = 0;
  totalU = 0;
  divide = 0;
  inicio = 0;
  fin = 0;
  tamanoBloque = 0;
  distribuido = N/cantProcesos;


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

/*Si es el último proceso se le da todo el bloque restante*/
 tamanoBloque = ((N/2)*(N+1))/(cantProcesos);
  inicio = id*tamanoBloque;
  if( id == (cantProcesos-1)){
    fin = (N/2)*(N+1);
  }else{
    fin = (id+1)*tamanoBloque;     
  }

  //Suma el total de la matriz triangular U
  //Cada proceso suma una parte
  for(i=inicio;i<fin;i++){
    for(j=i;j<N; j++){
      parcialU = parcialU + U[i+j*(j+1)/2]; 
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
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
        aux2[i*N+j]=0;
        for(k=i; k<N; k++){
          aux2[i*N+j]=aux2[i*N+j]+L_aux[i*N+k]*C[k+N*j];
        }

    }
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
  MPI_Allreduce(&parcialL, &totalL, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&parcialU, &totalU, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  divide = 1.0/(N*N);
  promL = totalL*divide;
  promU = totalU*divide;
  promLU = promU*promL;

//Sumo los 3 valores
  for(i=0;i<N/cantProcesos;i++){
    for(j=0;j<N;j++){
      M_aux[i*N+j]=aux1[i*N+j]+aux2[i*N+j]+aux3[i*N+j];
    }
  }

//Multiplico por el producto de promedios LU  
  for(i=0;i<N/cantProcesos;i++){
     for(j=0;j<N;j++){
       M_aux[i*N+j]=M_aux[i*N+j]*promLU;
    }
  }
//Reuno en M la matriz total
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
