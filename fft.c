#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

int pllen = 300000;
int m;
typedef double complex comp;
typedef struct {
  comp *P;
  int len;
  bool inv;
} FFTArgs;

// Returns comp*
void *FFT(void *args){
  FFTArgs *A = args;
  if (A->len == 1) return A->P;
  comp omega_n = cexp(2*M_PI*I / A->len);
  if (A->inv) omega_n = (1.0/omega_n);
  comp omega = 1.0;
  comp *P_even = malloc(sizeof(comp) * A->len / 2);
  comp *P_odd = malloc(sizeof(comp) * A->len / 2);
  for (int k = 0; k < A->len / 2; k++){
    P_even[k] = A->P[2 * k];
    P_odd[k] = A->P[2 * k + 1];
  }
  free(A->P);
  FFTArgs *evenArgs = malloc(sizeof(FFTArgs));
  evenArgs->P = P_even;
  evenArgs->len = A->len/2;
  evenArgs->inv = A->inv;
  FFTArgs *oddArgs = malloc(sizeof(FFTArgs));
  oddArgs->P = P_odd;
  oddArgs->len = A->len/2;
  oddArgs->inv = A->inv;
  comp *y_even, *y_odd;
  if (A->len > pllen){
    pthread_t evenThread, oddThread;
    pthread_create(&evenThread, NULL, FFT, evenArgs);
    pthread_create(&oddThread, NULL, FFT, oddArgs);
    pthread_join(evenThread, (void *)&y_even);
    pthread_join(oddThread, (void *)&y_odd);
  }else{
    y_even = FFT(evenArgs);
    y_odd = FFT(oddArgs);
  }
  comp *y = malloc(sizeof(comp) * A->len);
  for (int k = 0; k < A->len / 2; k++){
    y[k] = y_even[k] + omega * y_odd[k];
    y[k + A->len/2] = y_even[k] - omega * y_odd[k];
    omega *= omega_n;
  }
  free(y_even);
  free(y_odd);
  free(A);
  return (void *)y;
}

int main(){
  scanf("%d", &m);
  comp *P = malloc(sizeof(comp) * m);
  for (int i=0;i<m; i++){
    double x;
    scanf("%lf", &x);
    P[i] = CMPLX(x, 0.0);
  }
  FFTArgs *A = malloc(sizeof(FFTArgs));
  A->P = P;
  A->len = m;
  A->inv = false;
  comp *y = FFT(A);
  /*
  for (int i=0;i<m; i++){
    printf("%f%+fi, ", crealf(y[i]), cimagf(y[i]));
  }
  printf("\n");
  */
  FFTArgs *B = malloc(sizeof(FFTArgs));
  B->P = y;
  B->len = m;
  B->inv = true;
  comp *y2 = FFT(B);
  /*
  for (int i=0;i<m; i++){
    printf("%f%+fi\n", crealf(y2[i]) / m, cimagf(y2[i]) / m);
  }
  */
  return 0;
}
