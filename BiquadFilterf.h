#pragma once
#include "cmath"

#define PI 3.14159265358979323846264

class Filter {
public:
  Filter() {
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;

    b0 = 0;
    b1 = 0;
    b2 = 0;
    a0 = 0;
    a1 = 0;
    a2 = 0;
  }

  void set(double cutoff, double q, double fs) {
    //cutoff should be 0 - 17000
    //q should be 0.1 - 10
    double c = cutoff;
    if (cutoff > 17000) {
      c = 17000;
    }
    else if (cutoff < 20) {
      c = 20;
      }
    double omega = 2 * PI * c / fs;
    double sn = sin(omega);
    double cs = cos(omega);
    double alpha = sn / (2. * q);

    b0 = (1 - cs) / 2;
    b1 = 1 - cs;
    b2 = (1 - cs) / 2;
    a0 = 1 + alpha;
    a1 = -2 * cs;
    a2 = 1 - alpha;

    b0 /= a0;
    b2 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
  }

  double tick(double input) {

    double processed = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = processed;
    return processed;
  }
  double b0;
  double b1;
  double b2;
  double a0;
  double a1;
  double a2;
private:
  double x1;
  double x2;
  double y1;
  double y2;
  double cutoff;
  double q;


};