#include "Trippy.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "lowpass.hpp"
#include "cmath"

namespace q = cycfi::q;
using namespace q::literals;



Trippy::Trippy(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kAttack)->InitDouble("Attack", 10., 0., 10000., 0.01, "ms", 0, "", IParam::ShapePowCurve(3.));
  GetParam(kDecay)->InitDouble("Decay", 200., 0., 10000., 0.01, "ms", 0, "", IParam::ShapePowCurve(3.));
  GetParam(kSustain)->InitDouble("Sustain", 0.5, 0., 1, 0.01, "", 0, "", IParam::ShapePowCurve(3.));

  GetParam(kDistortMix)->InitDouble("Distortion Mix", 0., 0., 100., 0.01, "%");
  GetParam(kFilterMix)->InitDouble("Filter Mix", 0., 0., 100., 0.01, "%");
  GetParam(kVolumeMix)->InitDouble("Volume Mix", 0., 0., 100., 0.01, "%");

  GetParam(kDrive)->InitDouble("Drive", 0., 0., 1, 0.001, "");


#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {

    const IVStyle style{
      true, // Show label
      false, // Show value
      {
        DEFAULT_BGCOLOR, // Background
        DEFAULT_FGCOLOR, // Foreground
        DEFAULT_PRCOLOR, // Pressed
        COLOR_BLACK, // Frame
        DEFAULT_HLCOLOR, // Highlight
        DEFAULT_SHCOLOR, // Shadow
        COLOR_WHITE, // Extra 1
        DEFAULT_X2COLOR, // Extra 2
        DEFAULT_X3COLOR  // Extra 3
      }, // Colors
      IText(12.f, EAlign::Center, COLOR_WHITE),
    };


    auto button1action = [pGraphics, style, this](IControl* pCaller) mutable{
      pGraphics->HideControl(kVolumeFoo, false);
      pGraphics->HideControl(kDrive, true);
     page = 1;
    };

    auto button2action = [pGraphics, this](IControl* pCaller) mutable {
      pGraphics->HideControl(kVolumeFoo, true);
      pGraphics->HideControl(kDrive, false);
      page = 0;
    };




    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(IColor(255, 13, 13, 13));
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();

    // pGraphics->AttachControl(new ILEDControl(b.GetCentredInside(75).GetVShifted(-200), COLOR_RED), kLedTrigger);
    pGraphics->AttachControl(new IADSR(IRECT(0, 350, 480, 600), { kAttack, kDecay, kSustain }), kEnvPlot);
    //pGraphics->AttachControl(new ILFO(IRECT(0, 350, 480, 600)));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(80), kAttack, "", style));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(165), kDecay, "", style));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(250), kSustain, "", style));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(-250), kDistortMix, "", style));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(-140), kFilterMix, "", style));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(240).GetVShifted(-30), kVolumeMix, "", style));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(-240).GetVShifted(0), kDrive, "", style), kDriveKnob);

    pGraphics->AttachControl(new IDistortDisplay(IRECT(0, 50, 480, 250), { kDrive, kDistortMix }), kDistortPlot);
    pGraphics->AttachControl(new ICustomDisplay(IRECT(0, 50, 480, 250), kVolumeFoo), kVolumeDisplay);
    pGraphics->HideControl(kVolumeFoo, true);

    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(50).GetHShifted(-250).GetVShifted(-272), button1action, "", false, false));
    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(50).GetHShifted(-150).GetVShifted(-272), button2action, "", false, false));
  
  };
#endif
  env = 0;
  t = new TransientProcessor[NOutChansConnected()];
  adsr = new ADSRM[NOutChansConnected()];
  for (int i = 0; i < NOutChansConnected(); i++) {

  }
}

#if IPLUG_DSP
void Trippy::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();
  const double gainMix = GetParam(kVolumeMix)->Value() / 100.;
  const double distortionMix = GetParam(kDistortMix)->Value() / 8.;
  const double filterMix = GetParam(kFilterMix)->Value() / 100.;
  const double drive = GetParam(kDrive)->Value() * 10;
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      if (t[c].detect(inputs[c][s])) {
        adsr[c].retrigger();
      }
      env = adsr[c].tick();
      double process = inputs[c][s];
      if (gainMix != 0) {
        process = process * env * gainMix + process * (1 - gainMix);
      }

      //if (distortionMix > 0) {
      process = tanh(process * (((env) * distortionMix + drive) + 1));
      //}

      outputs[c][s] = process;
    }

  }
 // mLastOutputData.vals[0] = (float)env;
 // mEnvLastOutput.vals[0] = (float)adsr[0].getPoint();
  mDistortSender.PushData({ kDistortPlot, {float(env)} });
  mEnvSender.PushData({ kEnvPlot, {float(adsr[0].getPoint())} });
  mDisplaySender.PushData({ kVolumeDisplay, {float(env)} });
}

void Trippy::OnParamChange(int paramIdx) {
  switch (paramIdx) {

  case kAttack:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr[i].setAttack(GetParam(kAttack)->Value());
    }
    break;
  case kDecay:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr[i].setDecay(GetParam(kDecay)->Value()*GetSampleRate() / 1000);
    }
    break;
  case kSustain:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr[i].setSustain(GetParam(kSustain)->Value() * GetSampleRate() / 1000);
    }
    break; 
  }
 
}

void Trippy::OnIdle() {
  mDistortSender.TransmitData(*this);
  mEnvSender.TransmitData(*this);
  mDisplaySender.TransmitData(*this);

}

#endif
