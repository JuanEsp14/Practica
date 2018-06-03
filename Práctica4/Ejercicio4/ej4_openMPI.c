#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

/* Time in seconds from some point in the past*/
double dwalltime();

void master(int N, int cantProcesos);
void proceso(int N, int cantProcesos);

int main(int argc, char **argv){

  int i, id, cantProcesos, N;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);

  //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Falta un argumento:: N que indica el tamaño de la matriz \n");
    return 0;
  }

  N=atoi(argv[1]);

  if(id==0){
    master(N, cantProcesos);
  }
  else{
    proceso(N, cantProcesos);
  }

  MPI_Finalize();
  return 0;
}

/*****************************************************************/

void master(int N, int cantProcesos) {
  int i, j, k, l, iMin procesosActuales, finalizar = 0, merge = 0;
  int *arreglo=(int*) malloc(sizeof(int)*N);
  int *arreglo2=(int*) malloc(sizeof(int)*N);
  int *arreglo_aux=(int*) malloc(sizeof(int)*N);
  double timetick;
  MPI_Status status;

  /* Inicializo el arreglo con un random */
  for (i = 0; i < N; i++) {
      arreglo[i] = (rand() % 500); //Valor entre 0 y 499
  }

  procesosActuales = cantProcesos;

  timetick = dwalltime();

  for(i = 0; finalizar != 0; i++){
    desde = (N/procesosActuales)*miID;
    hasta = desde + N/procesosActuales;
    //Enviar a cada proceso su parte
    for( j = 0; j < procesosActuales; j++){
      
    }
    //Acomodo mi parte
    if (merge == 0){
      for(j = desde; j < hasta; j++){
        iMin = j;
        for (k = j+1; k < hasta; k++){
          if(arreglo[k] < arreglo[iMin])
            iMin=k;
        }
        aux = arreglo[j];
        arreglo[j] = arreglo[iMin];
        arreglo[iMin] = aux;
      }
      merge=1;
    }else{
      k = desde;
      l = hasta * 2;
      //Pido arreglo
      //Merge entre los dos arreglos
      for(j = desde; j < hasta; j++){
        if(arreglo[k] < arreglo2[l]){
          arrAux[j] = arreglo[k];
          k++;
        }else{
          arrAux[j] = arreglo2[l];
          l++;
        }
      }
    }
    //Evaluo qué procesos siguen trabajando
    procesosActuales = cantProcesos/2;
    if(procesosActuales < 0){
      finalizar = 1;
    }
    //Acomodo los punteros
  }


  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  free(arreglo);
  free(arreglo_aux);
  free(arreglo2);
}

void proceso(int N, int cantProcesos) {
  int id, i, j, k, l, iMin procesosActuales, finalizar = 0;
  int *arreglo=(int*) malloc(sizeof(int)*N);
  int *arreglo_aux=(int*) malloc(sizeof(int)*N);
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  procesosActuales = cantProcesos;
  for(i = 0; finalizar != 0; i++){
    desde = (N/procesosActuales)*miID;
    hasta = desde + N/procesosActuales;
    //Recibir mi parte a cada proceso su parte
    //Acomodo mi parte
    for(j = desde; j < hasta; j++){
      iMin = j;
      for (k = j+1; k < hasta; k++){
        if(arreglo[k] < arreglo[iMin])
        iMin=k;
      }
      aux = arreglo[j];
      arreglo[j] = arreglo[iMin];
      arreglo[iMin] = aux;
    }
    //Evaluo si sigo trabajando
    procesosActuales = cantProcesos/2;
    if(procesosActuales < id){
      finalizar = 1;
    }
    //Envio mi parte
  }
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
