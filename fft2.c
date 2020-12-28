#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

int n;
int logn;
typedef double complex comp;

int intRev(int x){
  int r=0;
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

comp *FFT(comp *a, bool inv){
  comp *A = bitRev(a);
  int m=1;
  for(int s=1; s<=logn; s++){
    m*=2;
    comp omega_n = cexp(2*M_PI*I / m);
    if (inv) omega_n = (1.0/omega_n);
    for (int k=0; k<n; k+=m){
      comp omega = 1.0;
      for (int j=0; j<m/2; j++){
        comp t = omega * A[k + j + m/2];
        comp u = A[k + j];
        A[k+j] = u+t;
        A[k+j+m/2] = u-t;
        omega *= omega_n;
      }
    }
  }
  return A;
}

int main(){
  scanf("%d", &n);
  logn = (int)log2((double)n);
  printf("logn: %d\n",logn);
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
  /*
  for (int i=0;i<n; i++){
    printf("%f%+fi\n", crealf(y2[i]) / n, cimagf(y2[i]) / n);
  }
  */
  return 0;
}
