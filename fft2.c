//FFT in-place
#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

int n;
bool inv;
typedef double complex comp;
comp* A;

int intRev(int x){
  int r=0;
  int logn = (int)log2((double)n);
  for (int i = 0; i < logn; i++) {
    r = (r << 1) | (x & 0x01);
    x >>= 1;
  }
  return r;
}

comp *bitRev(comp *a){
  comp *A = malloc(sizeof(comp)*n);
  for (int k=0; k<n; k++){
    A[intRev(k)]=a[k];
  }
  return A;
}

void *toPtr(int k, int m){
  return (void *)((((long)k) << 32) | m);
}

void *FFT2(void *args){
  int m = (long)args;
  if (m==1) return NULL;
  int k = (((long)args) >> 32);
  FFT2(toPtr(k, m/2));
  FFT2(toPtr(k + m/2, m/2));

  comp omega_n = cexp(2*M_PI*I / m);
  if (inv) omega_n = (1.0/omega_n);
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
  A = bitRev(a);
  inv=inv2;
  FFT2(toPtr(0, n));
  return A;
}

int main(){
  scanf("%d", &n);
  comp *P = malloc(sizeof(comp) * n);
  for (int i=0;i<n; i++){
    double x;
    scanf("%lf", &x);
    P[i] = CMPLX(x, 0.0);
  }
  comp *y = FFT(P, false);
  /*
  for (int i=0;i<n; i++){
    printf("%f%+fi, ", crealf(y[i]), cimagf(y[i]));
  }
  printf("\n");
  */
  comp *y2 = FFT(y, true);
  for (int i=0;i<n; i++){
    printf("%f%+fi\n", crealf(y2[i]) / n, cimagf(y2[i]) / n);
  }
  return 0;
}
