#pragma once

#include "IControl.h"
#include "cmath"
#include <complex>
#include <iostream>

#define PI 3.14159265358979323846264


template <int MAXNC = 1>
class IFilterDisplay : public IControl {

public:

  IFilterDisplay(const IRECT& bounds, int paramIdx)
    : IControl(bounds, paramIdx) {
    setPoints();
    b0 = 0;
    b1 = 0;
    b2 = 0;
    a1 = 0;
    a2 = 0;
  }
  void Draw(IGraphics& g) override {
    setPoints();
    IRECT b = mRECT.GetPadded(-5);
    g.FillRoundRect(IColor(255, 150, 150, 150), mRECT, 0, 10, 0, 10);
    g.FillRoundRect(IColor(255, 220, 220, 220), b, 0, 10, 0, 10);
    g.DrawData(IColor(255, 255, 38, 0), b, mPoints.data(), 100, nullptr, 0, 5);
    g.DrawData(IColor(255, 255, 136, 0), b, mPoints.data(), 100, nullptr, 0, 1);
  }

  void setPoints() {
    mPoints.resize(20);
    double freq = 0;
    for (int i = 0; i < 20; i++) {
      std::complex<double> z = getz(freq);
      mPoints[i] = zdomain(z);
      freq += 1000;
    }
  }
  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      b0 = d.vals[0];
      b1 = d.vals[1];
      b2 = d.vals[2];
      a1 = d.vals[3];
      a2 = d.vals[4];
      SetDirty(false);
    }
  }

  double zdomain(std::complex<double> &z) {
    return ((b0 + b1 / z + (b2 / z) * (b2 / z)) / (1. + a1 / z + (a2 / z) * (a2 / z))).real;
  }
  std::complex<double> getz(double freq) {
    double w = 2 * PI * freq / 44100;
    std::complex<double> ImagUnit = std::complex<double>(0.0, 1.0);
    return exp(ImagUnit * w);
  }

private:
  std::vector<float> mPoints;
  std::vector<float> temp;
  double b0;
  double b1;
  double b2;
  double a1;
  double a2;
};