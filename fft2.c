//FFT in-place
#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

struct timeval start, end;
int pllen;
int pllen2;//131072;
int n;
bool inv;
typedef double complex comp;
comp* A, *a;

int intRev(int x){
  x = (x & 0xFFFF0000) >> 16 | (x & 0x0000FFFF) << 16;
  x = (x & 0xFF00FF00) >>  8 | (x & 0x00FF00FF) <<  8;
  x = (x & 0xF0F0F0F0) >>  4 | (x & 0x0F0F0F0F) <<  4;
  x = (x & 0xCCCCCCCC) >>  2 | (x & 0x33333333) <<  2;
  x = (x & 0xAAAAAAAA) >>  1 | (x & 0x55555555) <<  1;
  int logn = (int)log2((double)n);
  x = ((unsigned int)x) >> (32 - logn);
  return x;
}

void *bitRev2(void *X){
  for (int k=0; k<pllen2; k++){
    A[intRev((long)X+k)]=a[(long)X+k];
  }
}

void bitRev(comp *a2){
  a=a2;
  A = malloc(sizeof(comp)*n);
  pthread_t *threads = malloc(sizeof(pthread_t) * (n/pllen2));
  for (int k=0; k<n; k+=pllen2){
    pthread_create(&threads[k/pllen2], NULL, bitRev2, (void *)(long)k);
  }
  for (int k=0; k<n; k+=pllen2){
    pthread_join(threads[k/pllen2], NULL);
  }
  free(threads);
}

void *toPtr(int k, int m){
  return (void *)((((long)k) << 32) | m);
}

void *FFT2(void *args){
  int m = (long)args;
  if (m==1) return NULL;
  int k = (((long)args) >> 32);
  if (m > pllen){
    pthread_t tA, tB;
    pthread_create(&tA, NULL, FFT2, toPtr(k, m/2));
    pthread_create(&tB, NULL, FFT2, toPtr(k + m/2, m/2));
    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
  }else{
    FFT2(toPtr(k, m/2));
    FFT2(toPtr(k + m/2, m/2));
  }

  comp omega_n;
  if (inv) omega_n = cexp(-2*M_PI*I / m);
  else omega_n = cexp(2*M_PI*I / m);
  comp omega = 1.0;
  for (int j=0; j<m/2; j++){
    comp t = omega * A[k + j + m/2];
    comp u = A[k + j];
    A[k + j] = u + t;
    A[k + j + m/2] = u - t;
    omega *= omega_n;
  }
  return NULL;
}


comp *FFT(comp *a, bool inv2){
  bitRev(a);
  inv=inv2;
  FFT2(toPtr(0, n));
  return A;
}

int main(){
  scanf("%d", &n);
  pllen = n/40;
  pllen2 = n/64;
  comp *P = malloc(sizeof(comp) * n);
  for (int i=0;i<n; i++){
    double x;
    scanf("%lf", &x);
    P[i] = CMPLX(x, 0.0);
  }
  gettimeofday(&start, NULL);
  comp *y = FFT(P, false);
  for (int i=0;i<n; i++){
    //printf("%f%+fi\n", crealf(y[i]), cimagf(y[i]));
    printf("%f\n", cabs(y[i]));
  }
  printf("\n");
  comp *y2 = FFT(y, true);
  /*
  for (int i=0;i<n; i++){
    printf("%f%+fi\n", crealf(y2[i]) / n, cimagf(y2[i]) / n);
  }
  */
  gettimeofday(&end, NULL);
  //printf("%lus\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
  return 0;
}
