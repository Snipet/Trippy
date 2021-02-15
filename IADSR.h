#pragma once


#include "IControl.h"
#include "cmath"

using namespace iplug;
using namespace igraphics;

template <int MAXNC = 1>
class IADSR : public IControl {
public:
  IADSR(const IRECT& bounds, const std::initializer_list<int>& params)
    : IControl(bounds, params) {

  }


  void Draw(IGraphics& g) override{
    IRECT b = mRECT.GetPadded(-10, -10, -10, -10);
    g.FillRoundRect(IColor(255, 150, 150, 150), mRECT, 0, 10, 0, 0);
    g.FillRoundRect(IColor(255,220,220, 220), b.GetPadded(5), 0, 10, 0, 0);
    const float inc = (b.R - b.L) / 12 / (GetValue(0)+GetValue(1));
    float x = b.L;
    int frameSize = (b.R - b.L) / inc + 2;
    for (int i = 0; i < frameSize; i++) {
      g.DrawLine(IColor(255, 180, 180, 180), x, mRECT.T, x, mRECT.B);
      x += inc;
    }
  


    const float attackx = b.L + (GetValue(0) / (GetValue(0) + GetValue(1))) * (b.R - b.L);
    const float sustainy = GetValue(2) * (b.T- b.B) + b.B;

    IColor c = IColor(255, 0, 25, 255);
    IColor c2 = IColor(255, 0, 150, 255);
    g.FillTriangle(IColor(150, 90, 150, 255), b.L, b.B, attackx, b.B, attackx, b.T);
    g.FillTriangle(IColor(150, 90, 150, 255), b.R, sustainy, attackx, b.B, attackx, b.T);
    g.FillTriangle(IColor(150, 90, 150, 255), b.R, b.B, attackx, b.B, b.R, sustainy);

    g.DrawLine(c, b.L, b.B, attackx, b.T, 0, 5.f);
    g.DrawLine(c2, b.L, b.B, attackx, b.T, 0, 2.f);

    g.DrawLine(c, b.R, sustainy, attackx, b.T, 0, 5.f);
    g.DrawLine(c2, b.R, sustainy, attackx, b.T, 0, 2.f);

    g.FillCircle(c, attackx, b.T, 5.);
    g.FillCircle(c, b.R, sustainy, 5.);
    g.FillCircle(c, b.L, b.B, 5.);
    if (point < 1) {
      const float envPointx = (b.R - b.L) * point + b.L;
      g.DrawLine(IColor(200, 0, 50, 255), envPointx, b.B, envPointx, b.T, 0, 3.f);
    }


  }
  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      point = d.vals[0];
      SetDirty(false);
    }

  }
 
private:

  float distance(float x1, float y1, float x2, float y2)
  {
    // Calculating distance 
    return sqrt(pow(x2 - x1, 2) +
      pow(y2 - y1, 2) * 1.0);
  }
  double point;
};