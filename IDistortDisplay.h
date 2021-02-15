#pragma once

#include "IControl.h"
#include "cmath"
using namespace iplug;
using namespace igraphics;

template <int MAXNC = 1>
class IDistortDisplay : public IControl {

public:

  IDistortDisplay(const IRECT& bounds, const std::initializer_list<int>& params)
    : IControl(bounds, params) {
    setPoints();
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
    mPoints.resize(100);
    double x = -0.5;
    for (int i = 0; i < 100; i++) {
      x += 0.01;
      mPoints[i] = tanh(30 * (GetValue(0)+0.1) * x * ((add * GetValue(1)) + 0.1))/2 + 0.5;
    }
  }
  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      add = d.vals[0];
      SetDirty(false); 
    }

  }
private:
  std::vector<float> mPoints;
  double add;
};