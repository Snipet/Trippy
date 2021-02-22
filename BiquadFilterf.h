#pragma once
#include "cmath"
#include "BiQuadCoeffs.h"

#define PI 3.14159265358979323846264

class Filter {
public:
  Filter() {

    for (int i = 0; i < 8; i++) {
      buf[i][0] = 0;
      buf[i][1] = 0;
      buf[i][2] = 0;
      buf[i][3] = 0;
    }
  }

  void set(double cutoff, double q, double fs, int type, double gain) {
    getCoefs(coeffs, cutoff, q, type, fs, gain);
  }

  double tick(double input, int order) {
    double processed = input;

    for (int i = 0; i < order; i++) {
      double prev = processed;
      processed = coeffs[0] * processed + coeffs[1] * buf[i][0] + coeffs[2] * buf[i][1] - coeffs[3] * buf[i][2] - coeffs[4] * buf[i][3];
      buf[i][1] = buf[i][0];
      buf[i][0] = prev;
      buf[i][3] = buf[i][2];
      buf[i][2] = processed;
    }
    return processed;
  }
private:

  double buf[8][4];

  double cutoff;
  double coeffs[5];

};