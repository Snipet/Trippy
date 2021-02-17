#pragma once

#include "IControl.h"
#include "cmath"
using namespace iplug;
using namespace igraphics;

template <int MAXNC = 1>
class ICustomDisplay : public IControl {

public:

  ICustomDisplay(const IRECT& bounds, int paramIdx)
    : IControl(bounds, paramIdx) {
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
    temp.resize(100);
    temp[99] = add;
    for (int i = 0; i < 99; i++) {
      temp[i] = mPoints[i + 1];
    }
    mPoints = temp;
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
  std::vector<float> temp;
  double add;
};