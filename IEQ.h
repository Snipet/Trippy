#pragma once

#include "IControl.h"
#include "cmath"
#include <complex>

#define PI 3.14159265358979323846264

enum EMethods {
  kUpdate = 0,
  kAddPoint,
  kDeletePoint
};


class IEQ : public IControl {

public:

  IEQ(const IRECT& bounds, const std::initializer_list<int>& params)
    : IControl(bounds, params) {

    mPoints.resize(200);
    actionBand = -1;
    dialogBand = -1;
    dialogBoxRect = IRECT(mRECT.L + 300, mRECT.T + 30, mRECT.R - 20, mRECT.B - 30);
    deltaY = 0;
    draggingControl = false;
    lastY = 0;
    tempCutoff = 0;
  }
  void Draw(IGraphics& g) override {
    IRECT b = mRECT.GetPadded(-5);
    setPoints();
    g.FillRoundRect(IColor(255, 150, 150, 150), mRECT, 0, 10, 0, 10);
    g.FillRoundRect(IColor(255, 220, 220, 220), b, 0, 10, 0, 10);
    g.DrawData(IColor(255, 0, 30, 255), b, mPoints.data(), 100, nullptr, 0, 5);
    //double ypos = transformY(yToPercent(-1 * points[0][1])) * (mRECT.B - mRECT.T) + mRECT.T;
    if (dialogBand != -1) {
      drawBandSettings(g);
    }
    for (int i = 0; i < actionBand + 1; i++) {
      //TODO: Add the dots to show where the points actually are
    }
  }
  double yToPercent(double y) {
    return 1 - (y - mRECT.T) / (mRECT.B - mRECT.T);
  }
  double xToFreq(double x) {
    return  (x / mRECT.R) * 22050;
  }

  void OnMouseDrag(float x, float y, float dx, float dy, const IMouseMod& mod) override {
    if (!draggingControl) {
      int p = atPoint(x, y, 20);
      if (p != -1) {
        //The user is dragging point
        //double mGain = yToPercent(y) * 60 - 30;
        //double send[4] = { mGain, scaleFreq(xToFreq(x)), q[p], p };
        //GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kUpdate, -1, sizeof(send), &send);
        //gains[p] = mGain;
       // freqs[p] = scaleFreq(xToFreq(x));
        //points[p][0] = x;
        //points[p][1] = y;
        //if (clickedRect(dialogBoxRect, x, y)) {
         // dialogBand = -1;
        //}
        //else {
         // dialogBand = p;
        //}
      }
      else if (clickedRect(dialogBoxRect, x, y)) {
        draggingControl = true;
        deltaY = 0;
        lastY = y;
        tempCutoff = freqs[dialogBand];
        //The user is probably dragging one of the controls
      }
    }
    else {
      int control = ((x - dialogBoxRect.L) / (dialogBoxRect.R - dialogBoxRect.L)) * 3.;
      switch (control) {
      case 0:
        //The user is changing the cutoff
        deltaY =+ y - lastY;

        lastY = y;
        tempCutoff = tempCutoff + deltaY * -20;
        if (tempCutoff > 22050) {
          tempCutoff = 22050;
        }
        else if (tempCutoff < 10) {
          tempCutoff = 10;
        }
        freqs[dialogBand] = tempCutoff;
        break;

      case 1:
        //The user is changing the reso
        deltaY = y - lastY;

        lastY = y;
        q[dialogBand] = q[dialogBand] + deltaY * -0.07;

        break;

      case 2:
        //The user is changing the gain
        deltaY = y - lastY;

        lastY = y;
        gains[dialogBand] = gains[dialogBand] + deltaY * -0.2;
        break;

      }
      double send[4] = { gains[dialogBand], freqs[dialogBand], q[dialogBand], dialogBand };
      GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kUpdate, -1, sizeof(send), &send);
    }
  }

  void OnMouseUp(float x, float y, const IMouseMod& mod) override {
    draggingControl = false;
  }

  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override {
    int p = atPoint(x, y, 20);
    if (p == -1) {
      addPoint(x, y);
    }
    else {
      removeBand(p);
      p = 0;
    }
  }

  void setPoints() {
    std::fill(mPoints.begin(), mPoints.end(), 0);

    for (int p = 0; p < actionBand + 1; p++) {

      double freq = 0;
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

        freq += 100;
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
  double scaleFreq2(double freq) {
    return (exp(3 * (freq / 22050) - 2.94) - 0.04978) * 22050;
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
    points[actionBand][2] = 1;
    q[actionBand] = 2;
    double send[4] = { gains[actionBand], freqs[actionBand], q[actionBand] , actionBand};
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kAddPoint, -1, sizeof(send), &send);
  }
  double transformY(double y) {
    //Position and scale down the EQ so more can be seen on the screen
    return (y / 8) + 0.375; //TODO: Add in options for custom scaling (i.e. don't just have it set at 8)
  }

  double getPattern(int in) {
    //There's a weird scaling thingy going on with adding in layers of biquad filters visually, so this just makes life easier
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

  void removeBand(int band) {
    gains[band] = 0;
    freqs[band] = 0;
    points[band][0] = 0;
    points[band][1] = 0;
    points[band][2] = 0;
    q[band] = 0;
    reshiftArrays();
    double send[1] = { band };
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kDeletePoint, -1, sizeof(send), &send);
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
          actionBand = x-1;
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
      if (state == 0) {
        actionBand = -1;
      }
    }
  }

  bool clickedRect(IRECT b, float x, float y) {
    return ( (x > b.L && x < b.R) && (y > b.T && y < b.B) );
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override {
    int p = atPoint(x, y, 10);
    if (p != -1) {
      //The user clicked on a point
      dialogBand = p;
    } else if (clickedRect(dialogBoxRect, x, y)) {
      //dialogBand = p;
    }
  }

  void drawBandSettings(IGraphics& g) {

    char reso[40];
    sprintf(reso, "%2.2f", q[dialogBand]);

    char cutoff[40];
    sprintf(cutoff, "%5.0f", freqs[dialogBand]);

    char gain[40];
    sprintf(gain, "%2.2f", gains[dialogBand]);

    g.FillRoundRect(IColor(255, 59, 59, 59), dialogBoxRect, 4.);
    g.FillRoundRect(IColor(255, 30, 30, 30), IRECT(dialogBoxRect.L + 20, dialogBoxRect.T, dialogBoxRect.R - 20, dialogBoxRect.B - 110), 0, 0, 5, 5);
    g.DrawText(IText(22., IColor(255,250,250,250)), "Band Settings", dialogBoxRect.GetCentredInside(0).GetVShifted(-55));
    g.DrawLine(IColor(255, 115, 115, 115), (dialogBoxRect.R - dialogBoxRect.L) / 3 + dialogBoxRect.L, dialogBoxRect.T + 40, (dialogBoxRect.R - dialogBoxRect.L) / 3 + dialogBoxRect.L, dialogBoxRect.B - 20);
    g.DrawLine(IColor(255, 115, 115, 115), (dialogBoxRect.R - dialogBoxRect.L) * 2 / 3 + dialogBoxRect.L, dialogBoxRect.T + 40, (dialogBoxRect.R - dialogBoxRect.L) * 2 / 3 + dialogBoxRect.L, dialogBoxRect.B - 20);

    //Text Controls
    g.DrawText(IText(22., IColor(255, 160, 160, 160)), "Reso", dialogBoxRect.GetCentredInside(0).GetVShifted(40));
    g.DrawText(IText(22., IColor(255, 250, 250, 250)), reso, dialogBoxRect.GetCentredInside(0).GetVShifted(-10));

    g.DrawText(IText(22., IColor(255, 160, 160, 160)), "Gain", dialogBoxRect.GetCentredInside(0).GetVShifted(40).GetHShifted(53));
    g.DrawText(IText(22., IColor(255, 250, 250, 250)), gain, dialogBoxRect.GetCentredInside(0).GetVShifted(-10).GetHShifted(53));

    g.DrawText(IText(20., IColor(255, 160, 160, 160)), "Cutoff", dialogBoxRect.GetCentredInside(0).GetVShifted(40).GetHShifted(-53));
    g.DrawText(IText(17., IColor(255, 250, 250, 250)), cutoff, dialogBoxRect.GetCentredInside(0).GetVShifted(-10).GetHShifted(-53));
  }

  char* doubleToCharArray(double input) {
    sprintf(temp, "%2.2f", input);
    return temp;
  }

private:
  IRECT dialogBoxRect;
  char temp[40];
  std::vector<float> mPoints;
  float gains[20];
  float freqs[20];
  float q[20];
  float points[20][3];  //x, y, uid
  double coeffs[5];
  int actionBand;
  int dialogBand;

  bool draggingControl;
  float deltaY;
  float lastY;
  float tempCutoff;
};