#pragma once
#include "BiQuadCoeffs.h"


class EQ {
public:
  EQ() {
    actionBands = -1;
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 4; j++) {
        buf[i][j] = 0;
      }
    }
  }
  void setGain(int band, float gain) {
    gains[band] = gain;
    updateBand(band);
  }
  void setFreq(int band, float freq) {
    freqs[band] = freq;
    updateBand(band);
  }
  void setReso(int band, float reso) {
    q[band] = reso;
    updateBand(band);
  }
  void set(int band, float gain, float freq, float reso) {
    gains[band] = gain;
    freqs[band] = freq;
    q[band] = reso;
    updateBand(band);
  }
  void setSampleRate(float sampleRate) {
    fs = sampleRate;
  }
  void setActionBands(int count) {
    actionBands = count;
  }
  void addBand(float gain, float cutoff, float reso) {
    gains[actionBands + 1] = gain;
    freqs[actionBands + 1] = cutoff;
    q[actionBands + 1] = reso;
    updateBand(actionBands + 1);
    actionBands++;
  }
  double tick(double input) {
    if (actionBands > -1) {
      double output = input;
      double processed;
      for (int i = 0; i < actionBands + 1; i++) {
        processed = coeffs[i][0] * input + coeffs[i][1] * buf[i][0] + coeffs[i][2] * buf[i][1] - coeffs[i][3] * buf[i][2] - coeffs[i][4] * buf[i][3];
        buf[i][1] = buf[i][0];
        buf[i][0] = input;
        buf[i][3] = buf[i][2];
        buf[i][2] = processed;
        output = output + processed;
        output = output - input;
      }
      return output;
    }
    else {
      return input;
    }
  }

private:

  void updateBand(int band) {
    getCoefs(coeffs[band], freqs[band], q[band], 4, 44100, gains[band]);
  }

  float gains[20];
  float freqs[20];
  float q[20];
  float buf[20][4];
  double coeffs[20][5];
  float fs;
  int actionBands; //How many bands are actually in play -1
};