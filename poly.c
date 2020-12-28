#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int m;

double complex *FFT(double complex *P, int len, bool inv){
  if (len == 1) return P;
  double complex omega_n = cexp(2*M_PI*I/len);
  if (inv) omega_n = 1.0/omega_n;
  double complex omega = 1.0;
  double complex *P_even = malloc(sizeof(double complex) * len / 2);
  double complex *P_odd = malloc(sizeof(double complex) * len / 2);
  for (int k = 0; k < len / 2; k++){
    P_even[k] = P[2 * k];
    P_odd[k] = P[2 * k + 1];
  }
  free(P);
  double complex *y_even = FFT(P_even, len / 2, inv);
  double complex *y_odd = FFT(P_odd, len / 2, inv);
  double complex *y = malloc(sizeof(double complex) * len);
  for (int k = 0; k < len / 2; k++){
    y[k] = y_even[k] + omega * y_odd[k];
    y[k + len/2] = y_even[k] - omega * y_odd[k];
    omega *= omega_n;
  }
  free(y_even);
  free(y_odd);
  return y;
}

int main(){
  scanf("%d", &m);
  double complex *P1 = malloc(sizeof(double complex) * m * 2);
  for (int i=0;i<m; i++){
    double x;
    scanf("%lf", &x);
    P1[i] = CMPLX(x, 0.0);
    P1[i+m] = CMPLX(0.0, 0.0);
  }
  double complex *P2 = malloc(sizeof(double complex) * m * 2);
  for (int i=0;i<m; i++){
    double x;
    scanf("%lf", &x);
    P2[i] = CMPLX(x, 0.0);
    P2[i+m] = CMPLX(0.0, 0.0);
  }
  double complex *y1 = FFT(P1, m * 2, false);
  double complex *y2 = FFT(P2, m * 2, false);
  double complex *pts = malloc(sizeof(double complex) * m * 2);
  for (int i=0;i<m*2; i++){
    pts[i] = y1[i] * y2[i];
  }
  double complex *res = FFT(pts, m * 2, true);
  for (int i=0;i<m*2; i++){
    printf("%f\n", crealf(res[i]) / m / 2);
    // Imaginary: cimagf(res[i]) / m / 2
  }
  return 0;
}
