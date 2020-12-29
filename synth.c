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
int pllen2;
int n = 262144;
bool inv;
typedef double complex comp;
comp* X, *Y;

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

void *bitRev2(void *offset){
  for (int i=0; i<pllen2; i++){
    Y[intRev((long)offset+i)]=X[(long)offset+i];
  }
}

void bitRev(){
  Y = malloc(sizeof(comp)*n);
  pthread_t *threads = malloc(sizeof(pthread_t) * (n/pllen2));
  for (int i=0; i<n; i+=pllen2){
    pthread_create(&threads[i/pllen2], NULL, bitRev2, (void *)(long)i);
  }
  for (int i=0; i<n; i+=pllen2){
    pthread_join(threads[i/pllen2], NULL);
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
    comp t = omega * Y[k + j + m/2];
    comp u = Y[k + j];
    Y[k + j] = u + t;
    Y[k + j + m/2] = u - t;
    omega *= omega_n;
  }
  return NULL;
}


comp *FFT(comp *X2, bool inv2){
  pllen = n/40;
  pllen2 = n/64;
  X=X2;
  inv=inv2;
  bitRev();
  FFT2(toPtr(0, n));
  return Y;
}

typedef struct{
  double startY;
  double middleX;
  double middleY;
  double endY;
} envelope;

double getEnvelope(envelope *e, double x){
  if (x < e->middleX)
    return (e->middleY - e->startY) / e->middleX * x + e->startY;
  return (e->middleY - e->endY) / (1 - e->middleX) * (1-x) + e->endY;
}

int main(){
  gettimeofday(&start, NULL);
  srand((unsigned int)start.tv_usec);

  comp *F = malloc(sizeof(comp) * n);
  for (int i=0;i<n; i++){
    F[i] = CMPLX(0.0, 0.0);
  }
  int length=pow(2,18);
  int freq=440;
  int rate=44100;
  int freq2=freq*length/rate;
  int loops=length/2/freq2;
  for (int i=1;i<loops; i++){
    double phase = (double)rand()/RAND_MAX*2*M_PI;
    double mag = 100000.0/pow(2,(i-1));
    F[i*freq2]=CMPLX(mag*cos(phase),mag*sin(phase));
    F[n-(i*freq2)]=conj(F[i*freq2]);
    //freq2 = (int)(0.8*freq2);
  }

  envelope e;
  e.startY=0.1;
  e.middleX=0.2;
  e.middleY=1;
  e.endY=0;
  comp *S = FFT(F, false);
  for (int i=0;i<n; i++){
    S[i]*=getEnvelope(&e,(double)i/n);
    printf("%f\n", crealf(S[i]) / n);
    //printf("%f%+fi\n", crealf(S[i]) / n, cimagf(S[i]) / n);
    //printf("%f\n", cabs(y[i]));
  }
  gettimeofday(&end, NULL);
  //printf("%lus\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
  return 0;
}
