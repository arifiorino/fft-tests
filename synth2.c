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
bool inv;
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

comp *bitRev(comp *X, comp *Y){
  for (int i=0; i<n; i++){
    Y[intRev(i)]=X[i];
  }
  return Y;
}

void FFT2(comp *Y, int k, int m){
  if (m==1) return;
  FFT2(Y, k, m/2);
  FFT2(Y, k + m/2, m/2);
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

void addFreq(comp *F, double freq, double width){
  double max=1000.0;
  double slope = -1.0*max/width;
  //printf("freq:%f\n",freq);
  for (int i=freq; freq-i<width; i--){
    F[i] += CMPLX((freq-i)*slope + max, 0.0);
    //printf("%d %f\n",i,(freq-i)*slope + max);
    F[n-i]=F[i];
  }
  for (int i=(int)freq+1; i-freq<width; i++){
    F[i] += CMPLX((i-freq)*slope + max, 0.0);
    //printf("%d %f\n",i,(i-freq)*slope + max);
    F[n-i]=F[i];
  }
}

void *FFTs(void *args){
  int start=(long)args;
  for (int i=start; i<start+pllen && i<windows; i++){
    bitRev(spec[i], S+(i*n));
    FFT2(S+(i*n), 0, n);
  }
}

int main(){
  gettimeofday(&start, NULL);
  //srand((unsigned int)start.tv_usec);
  int len=1048576;
  n = 2048;
  windows = len/n;

  spec = malloc(sizeof(comp *) * windows);
  double baseFreq2=440.0*n/44100.0;
  double freq2;
  for (int i=0; i<windows; i++){
    freq2=baseFreq2;//+baseFreq2*i/windows;
    spec[i]=malloc(sizeof(comp) * n);
    for (int j=0; j<n; j++){
      spec[i][j]=CMPLX(0.0,0.0);
    }
    for (int k=1; k<=10; k++){
      addFreq(spec[i], k*freq2, 0.5);
    }
  }
  
  S=malloc(sizeof(comp) * len);
  int numThreads = 64;
  pllen = windows/numThreads;
  inv=true;
  pthread_t *threads = malloc(sizeof(pthread_t) * numThreads);
  for (int i=0; i<windows; i+=pllen){
    pthread_create(&threads[i/pllen], NULL, FFTs, (void *)(long)i);
  }
  for (int i=0; i<numThreads; i++){
    pthread_join(threads[i], NULL);
  }
  free(threads);

  for (int i=0; i<windows; i++){
    free(spec[i]);
  }
  free(spec);

  for (int i=0;i<len; i++){
    printf("%lf\n", S[i]);
  }
  free(S);

  gettimeofday(&end, NULL);
  //printf("%lus\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
  return 0;
}
