#pragma once
#include "cmath"
#define PI 3.14159265358979323846264





void getCoefs(double (&coeffs)[5], double cutoff, double q, int type, double fs, double fgain) {
  double c = cutoff;
  if (cutoff > 22000) {
    c = 22000;
  }
  else if (cutoff < 20) {
    c = 20;
  }
  double omega = 2 * PI * c / fs;
  double sn = sin(omega);
  //double sn = q::detail::sin_gen(q::phase(omega*360));
  double cs = cos(omega);
  double alpha = sn / (2. * q);
  double gain_abs = pow(10, fgain / 40);
  double a0;
  switch (type) {

  case 0:
    //Lowpass
    coeffs[0] = (1 - cs) / 2;
    coeffs[1] = 1 - cs;
    coeffs[2] = (1 - cs) / 2;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 1:
    //Highpass
    coeffs[0] = (1 + cs) / 2;
    coeffs[1] = -1 * (1 + cs);
    coeffs[2] = (1 + cs) / 2;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;
  case 2:
    //Bandpass
    coeffs[0] = alpha;
    coeffs[1] = 0;
    coeffs[2] = -alpha;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 3:
    //Notch
    coeffs[0] = 1;
    coeffs[1] = -2 * cs;
    coeffs[2] = 1;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 4:
    //Peak
    coeffs[0] = 1 + (alpha * gain_abs);
    coeffs[1] = -2 * cs;
    coeffs[2] = 1 - (alpha * gain_abs);
    a0 = 1 + (alpha / gain_abs);
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - (alpha / gain_abs);
    break;
  }



  coeffs[0] = coeffs[0] / a0;
  coeffs[1] = coeffs[1] / a0;
  coeffs[2] = coeffs[2] / a0;
  coeffs[3] = coeffs[3] / a0;
  coeffs[4] = coeffs[4] / a0;

}