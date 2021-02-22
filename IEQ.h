#pragma once

#include "IControl.h"
#include "cmath"
#include <complex>

#define PI 3.14159265358979323846264

enum EMethods {
  kUpdate = 0,
  kAddPoint
};


class IEQ : public IControl {

public:

  IEQ(const IRECT& bounds, const std::initializer_list<int>& params)
    : IControl(bounds, params) {

    mPoints.resize(200);
    actionBand = -1;

  }
  void Draw(IGraphics& g) override {
    IRECT b = mRECT.GetPadded(-5);
    setPoints();
    g.FillRoundRect(IColor(255, 150, 150, 150), mRECT, 0, 10, 0, 10);
    g.FillRoundRect(IColor(255, 220, 220, 220), b, 0, 10, 0, 10);
    g.DrawData(IColor(255, 0, 30, 255), b, mPoints.data(), 100, nullptr, 0, 5);
    //double ypos = transformY(yToPercent(-1 * points[0][1])) * (mRECT.B - mRECT.T) + mRECT.T;
    for (int p = 0; p < actionBand + 1; p++) {
      g.FillCircle(IColor(255, 0, 30, 255), points[p][0], points[p][1], 10);
    }
  }
  double yToPercent(double y) {
    return 1 - (y - mRECT.T) / (mRECT.B - mRECT.T);
  }
  double xToFreq(double x) {
    return  (x / mRECT.R) * 22050;
  }

  void OnMouseDrag(float x, float y, float dx, float dy, const IMouseMod& mod) override {
    int p = atPoint(x, y, 20);
    if (p != -1) {
      double send[4] = { yToPercent(y) * 30 - 15, scaleFreq(xToFreq(x)), q[p], p};
      GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kUpdate, -1, sizeof(send), &send);
      gains[p] = yToPercent(y) * 30 - 15;
      freqs[p] = xToFreq(x);
      points[p][0] = x;
      points[p][1] = y;
    }
  }
  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override {
    int p = atPoint(x, y, 20);
    if (p == -1) {
      addPoint(x, y);
    }
    else {
      //TODO: Delete the point
      p = 0;
    }
  }

  void setPoints() {
    std::fill(mPoints.begin(), mPoints.end(), 0);

    for (int p = 0; p < actionBand + 1; p++) {

      double freq = 20;
      for (int i = 0; i < 200; i++) {
        std::complex<double> z = getz(freq);
        double x;
        if (gains[p] > 0) {
          x = transformY(zdomain(z, p));
        }
        else {
          x = -1 * (transformY(zdomain(z, p))) + 1;
        }
        if (actionBand < 2) {
          mPoints[i] += x - (actionBand) * 0.25;
        }
        else {
          //mPoints[i] += x - ((actionBand - 1.)/(actionBand + 1.));
          mPoints[i] += x - getPattern(actionBand);
        }

        freq += 110;
      }
    }
  }

  double zdomain(std::complex<double>& z, int band) {
    getCoefs(coeffs, freqs[band], q[band], 4, 44100, abs(gains[band]));

    std::complex<double> yeet = ((coeffs[0] + coeffs[1] / z + (coeffs[2] / z) * (coeffs[2] / z)) / (1. + coeffs[3] / z + (coeffs[4] / z) * (coeffs[4] / z)));
    return yeet.real();
  }
  std::complex<double> getz(double freq) {
    double w = 2 * PI * freq / 44100;
    std::complex<double> ImagUnit = std::complex<double>(0.0, 2.);
    return exp(ImagUnit * w);
  }

  double scaleFreq(double freq) {
    return (exp(4 * (freq / 22050) - 4) - 0.0183) * 22050;
  }


  int atPoint(float x, float y, float range) { //Returns -1 if there isn't a point at the mouse
    for (int i = 0; i < 20; i++) {
      if ((points[i][0] - range < x && points[i][0] + range > x) && (points[i][1] - range < y && points[i][1] + range > y)) {
        return i;
      }
    }
    return -1;
  }
  void addPoint(float x, float y) {
    actionBand++;
    gains[actionBand] = 0;
    freqs[actionBand] = scaleFreq(xToFreq(x));
    points[actionBand][0] = x;
    points[actionBand][1] = y;
    q[actionBand] = 2;
    double send[4] = { gains[actionBand], freqs[actionBand], q[actionBand] , actionBand};
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kAddPoint, -1, sizeof(send), &send);
  }
  double transformY(double y) {
    return (y / 8) + 0.375;
  }

  double getPattern(int in) {
    double n;
    double d;
    if (in % 2 == 0) {
      n = in / 2.;
      d = in + 1.;
    }
    else {
      n = in;
      d = (in + 1) * 2.;
    }
    return n / d;
  }

private:
  std::vector<float> mPoints;
  float gains[20];
  float freqs[20];
  float q[20];
  float points[20][3];  //x, y, uid
  double coeffs[5];
  int actionBand;
};