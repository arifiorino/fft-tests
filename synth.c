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
int n;
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

comp *getNote(int freq, int rate){
  comp *F = malloc(sizeof(comp) * n);
  for (int i=0;i<n; i++){
    F[i] = CMPLX(0.0, 0.0);
  }
  int freq2=freq*n/rate;
  double harmonics[]={100000.0, 40000.0, 25000.0, 18000.0, 18000.0,
                18000.0, 15000.0, 12000.0, 11000.0, 9000.0, 5000.0};
  //int loops=n/2/freq2;
  for (int i=1;i<11; i++){
    double phase = (double)rand()/RAND_MAX*2*M_PI;
    double mag = harmonics[i];//100000.0/pow(2,(i-1));
    F[i*freq2]=CMPLX(mag*cos(phase),mag*sin(phase));
    F[n-(i*freq2)]=conj(F[i*freq2]);
  }
  comp *S = FFT(F, true);
  free(F);
  return S;
}

int main(){
  gettimeofday(&start, NULL);
  srand((unsigned int)start.tv_usec);

  n = pow(2,18);

  comp *S = getNote(440, 44100);
  /*
  comp *S2 = getNote(261, 44100);
  n*=2;
  comp *S = malloc(sizeof(comp)*n);
  for (int i=0;i<n/2; i++){
    S[i]=S1[i]/n;
    S[i+n/2]=S2[i]/n;
  }
  comp *F = FFT(S, false);
  for (int i=0;i<n; i++){
    printf("%f\n", creal(S1[i])/n);
    if (cabs(F[i])>5000000000)
      printf("%f\n", carg(F[i]));
    else printf("0\n");
  }
  */

  envelope e;
  e.startY=0.1;
  e.middleX=0.2;
  e.middleY=1;
  e.endY=0;
  for (int i=0;i<n; i++){
    S[i]*=getEnvelope(&e,(double)i/n);
    printf("%f\n", crealf(S[i]) / n);
    //printf("%f%+fi\n", crealf(S[i]) / n, cimagf(S[i]) / n);
    //printf("%f\n", cabs(F[i]));
  }
  gettimeofday(&end, NULL);
  //printf("%lus\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
  return 0;
}
