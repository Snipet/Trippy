#pragma once
#include "cmath"

class TransientProcessor {
public:
  TransientProcessor() {
    average = 0;
  }

  bool detect(double input) {
    average = (abs(input) + average * 200) / 201;
    if (abs(input) > average * 5) {
      return true;
    }
    else {
      return false;
    }
  }

private: 
  double average;
};