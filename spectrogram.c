#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

struct timeval start, end;
int pllen;
int n;
int windows;
int overlap;
typedef double complex comp;
comp *S;
comp **spec;

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

comp *bitRev(comp *X){
  comp *Y = malloc(sizeof(comp)*n);
  for (int i=0; i<n; i++){
    Y[intRev(i)]=X[i];
  }
  return Y;
}

void FFT2(comp *Y, bool inv, int k, int m){
  if (m==1) return;
  FFT2(Y, inv, k, m/2);
  FFT2(Y, inv, k + m/2, m/2);
  comp omega_n;
  if (inv) omega_n = cexp(-2*M_PI*I / m);
  else omega_n = cexp(2*M_PI*I / m);
  comp omega = 1.0;
  for (int j=0; j<m/2; j++){
    comp t = omega * Y[k + j + m/2];
    comp u = Y[k + j];
    Y[k + j] = u + t;
    Y[k + j + m/2] = u - t;
    omega *= omega_n;
  }
}

comp *FFT(comp *X, bool inv){
  comp *Y = bitRev(X);
  FFT2(Y, inv, 0, n);
  return Y;
}

void *FFTs(void *args){
  int start=(long)args;
  for (int i=start; i<start+pllen && i<windows; i++){
    spec[i] = FFT(S+(i*n/overlap), false);
  }
}

int main(){
  gettimeofday(&start, NULL);
  //srand((unsigned int)start.tv_usec);
  int len;
  scanf("%d", &len);
  n = 4096;
  overlap=1;

  S = malloc(sizeof(comp) * len);
  for (int i=0;i<len; i++){
    double x;
    scanf("%lf", &x);
    S[i] = CMPLX(x, 0.0);
  }
  
  windows = len/n*overlap-(overlap-1);
  pllen = len/n*overlap/64;
  spec = malloc(sizeof(comp *) * windows);
  pthread_t *threads = malloc(sizeof(pthread_t) * 64);
  for (int i=0; i<windows; i+=pllen){
    pthread_create(&threads[i/pllen], NULL, FFTs, (void *)(long)i);
  }
  for (int i=0; i<64; i++){
    pthread_join(threads[i], NULL);
  }
  free(threads);

  for (int i=0; i<windows; i++){
    for (int j=0; j<n; j++){
      printf("%f\n", cabs(spec[i][j]));
    }
    free(spec[i]);
  }
  free(spec);

  gettimeofday(&end, NULL);
  //printf("%lus\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
  return 0;
}
