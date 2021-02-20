#pragma once
#include "cmath"
#include "sin_table.hpp"
#include "phase.hpp"
#include "BiQuadCoeffs.h"

#define PI 3.14159265358979323846264
namespace q = cycfi::q;
class Filter {
public:
  Filter() {
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;
  }

  void set(double cutoff, double q, double fs, int type) {
    //cutoff should be 0 - 17000
    //q should be 0.1 - 10
    getCoefs(coeffs, cutoff, q, type, fs);
  }

  double tick(double input) {

    double processed = coeffs[0] * input + coeffs[1] * x1 + coeffs[2] * x2 - coeffs[3] * y1 - coeffs[4] * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = processed;
    return processed;
  }
private:
  double x1;
  double x2;
  double y1;
  double y2;

  double b0;
  double b1;
  double b2;
  double a1;
  double a2;

  double cutoff;
  double coeffs[5];

};