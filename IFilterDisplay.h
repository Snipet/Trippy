#pragma once

#include "IControl.h"
#include "cmath"
#include <complex>
#include <iostream>
#include "BiQuadCoeffs.h"

#define PI 3.14159265358979323846264


template <int MAXNC = 1>
class IFilterDisplay : public IControl {

public:

  IFilterDisplay(const IRECT& bounds, const std::initializer_list<int>& params)
    : IControl(bounds, params) {
    setPoints();
    reso = 2;
    div = 1;
  }
  void Draw(IGraphics& g) override {
    setPoints();
    IRECT b = mRECT.GetPadded(-5);
    g.FillRoundRect(IColor(255, 150, 150, 150), mRECT, 0, 10, 0, 10);
    g.FillRoundRect(IColor(255, 220, 220, 220), b, 0, 10, 0, 10);
    g.DrawData(IColor(255, 0, 30, 255), b, mPoints.data(), 100, nullptr, 0, 5);
    g.DrawData(IColor(255,0,120,255), b, mPoints.data(), 100, nullptr, 0, 2);
  }

  void setPoints() {
    mPoints.resize(200);
    double freq = 200;
    for (int i = 0; i < 200; i++) {
      std::complex<double> z = getz(freq);
      double x = zdomain(z) / div;

      mPoints[i] = x;
      freq += 100;
    }
  }
  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      switch (int(round(GetValue(3) * 4))) {
      case 0:
        cutoff = (GetValue(2) - 0.5) * d.vals[0] * 20000 + (GetValue(0) * 12000 + 6000);
        reso = GetValue(1) * 5 + 0.1;
        div = 1.3;
        break;
      case 1:
        cutoff = (GetValue(2) - 0.5) * d.vals[0] * 20000 + (GetValue(0) * 15000 + 3000);
        reso = GetValue(1) * 5 + 0.1;
        div = 1.3;
        break;
      case 2:
        cutoff = (GetValue(2) - 0.5) * d.vals[0] * 20000 + (GetValue(0) * 12000 + 6000);
        reso = GetValue(1) * 5 + 0.1;
        div = 0.2;
      default:
        cutoff = (GetValue(2) - 0.5) * d.vals[0] * 20000 + (GetValue(0) * 19000 + 1000);
        div = 1.5;
        reso = GetValue(1) * 5 + 0.1;
        break;
      }

      SetDirty(false);
    }
  }

  double zdomain(std::complex<double> &z) {
    getCoefs(coeffs, cutoff, reso, round(GetValue(3) * 4), 44100, GetValue(4) * 40 - 20); 

    std::complex<double> yeet = ((coeffs[0] + coeffs[1] / z + (coeffs[2] / z) * (coeffs[2] / z)) / (1. + coeffs[3] / z + (coeffs[4] / z) * (coeffs[4] / z)));
    return yeet.real();
  }
  std::complex<double> getz(double freq) {
    double w = 2 * PI * freq / 44100;
    std::complex<double> ImagUnit = std::complex<double>(0.0, 2.);
    return exp(ImagUnit * w);
  }

private:
  std::vector<float> mPoints;
  std::vector<float> temp;
  double coeffs[5];
  double cutoff;
  double reso;
  double div;
};