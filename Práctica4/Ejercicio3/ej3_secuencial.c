#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZESTRING 25
/* Time in seconds from some point in the past
Analizar el problema de frontera cuando se divide el arreglo en el paralelo*/
double dwalltime();

typedef struct{
  char palabra[SIZESTRING];
  int contador;
}Lista;

//Funcion encargada de almacenar la estructura en la lista
Lista *AddToList(Lista *pLista, int *posListaPalabras,int largoPalabra, char *palabra){
  int encontrada=0;

  for(int j = 0; j < *posListaPalabras; j++){
    if(strcmp(pLista[j].palabra, palabra) == 0){
      pLista[j].contador++;
      encontrada=1;
    }
  }
  if(encontrada == 0){
    if(pLista == NULL)
    pLista = malloc(sizeof(Lista));
    else
    pLista = realloc(pLista,sizeof(Lista)*(*posListaPalabras+1));

    //Inicializo el contenido del vector
    memset(pLista[*posListaPalabras].palabra,0,largoPalabra);
    pLista[*posListaPalabras].contador = 1;
    strncpy(pLista[*posListaPalabras].palabra, palabra ,largoPalabra);
    *posListaPalabras += 1;
  }
  return pLista;
}

int main(int argc, char **argv){

  FILE *archivo;
  int largoPalabra=0, i, posListaPalabras=0, N=100;
  double timetick;
  char palabra[SIZESTRING];

  //Controla los argumentos al programa
  if (argc < 2){
   printf("\n Falta un argumento:: N que indica el tamaño del texto \n");
   return 0;
  }

   N=atoi(argv[1]);

  char oracion[N];

  //Creamos lista para las palabras
  Lista *pListaPalabras = NULL;
  //Inicializo la palabra con ceros
  memset(palabra,0,SIZESTRING);

  archivo = fopen("ejercicio3.txt","r");

  if (archivo == NULL){
    printf("\nError de apertura del archivo. \n\n");
  }else{
    fgets(oracion, N, archivo);
  }
  fclose(archivo);

  timetick = dwalltime();

  for(i = 0; i < strlen(oracion); i++){
    //Verifico si se cambió de palabra o si se está por terminar el texto para
    //elminar el \0
    if(oracion[i] != ' ' && i < strlen(oracion)-1){
      palabra[largoPalabra] = oracion[i];
      largoPalabra++;
    }else{
      pListaPalabras = AddToList(pListaPalabras, &posListaPalabras, largoPalabra,palabra);
      largoPalabra = 0;
      memset(palabra,0,SIZESTRING);
    }
  }

  for(i=0; i < posListaPalabras; i++){
    printf("%s: %d veces\n", pListaPalabras[i].palabra, pListaPalabras[i].contador);
  }

  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  free(pListaPalabras);

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
