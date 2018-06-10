#ifndef __OMP_FUNCTIONS__
#define __OMP_FUNCTIONS__

extern void multYprom(int N, float distribuido, float promU,
              float parcialL, double * L, double *U
              double *A_aux, double * D_aux, doube * L_aux
              double * aux1, double * aux2, double *aux3);

extern void suma(int N, float distribuido, float promLU, double * M_aux,
              double * aux1, double * aux2, double *aux3);
#endif
