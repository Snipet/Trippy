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
    points[actionBands + 1][2] = 1;
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

  void removeBand(int band) {
    gains[band] = 0;
    freqs[band] = 0;
    points[band][2] = 0;
    q[band] = 0;
    reshiftArrays();
  }


  void reshiftArrays() {

    bool repeatNeeded = true;

    while (repeatNeeded) {

      bool allOne = true;


      for (int i = 0; i < 20; i++) {
        //We are looking for the uid here because that tells us if there is anything on that band
        if (points[i][2] == 0 && allOne) {
          //This would be where the array stops
          allOne = false;
        }
        else if (points[i][2] == 1 && !allOne) {
          //There shouldn't be any more items in the array, but there's one here because something was deleted. So now we need to shift it.
          gains[i - 1] = gains[i];
          freqs[i - 1] = freqs[i];
          q[i - 1] = q[i];
          points[i - 1][0] = points[i][0];
          points[i - 1][1] = points[i][1];
          points[i - 1][2] = points[i][2];

          gains[i] = 0;
          freqs[i] = 0;
          points[i][0] = 0;
          points[i][1] = 0;
          points[i][2] = 0;
          q[i] = 0;

          repeatNeeded = true;
        }

      }
      int state = 0;
      for (int x = 0; x < 20; x++) {
        //Do a sweep to make sure everything fits
        if (points[x][2] == 1 && state == 0) {
          state = 1;
        }
        if (points[x][2] == 0 && state == 1) {
          state = 2;
          actionBands = x - 1;
        }
        if (points[x][2] == 1 && state == 2) {
          state = 3; //This is bad
        }
      }
      if (state != 1) {
        repeatNeeded = false;
      }
      else {
        repeatNeeded = true;
      }
    }
  }

private:

  void updateBand(int band) {
    getCoefs(coeffs[band], freqs[band], q[band], 4, 44100, gains[band]);
  }

  float points[20][3];
  float gains[20];
  float freqs[20];
  float q[20];
  float buf[20][4];
  double coeffs[20][5];
  float fs;
  int actionBands; //How many bands are actually in play -1
};