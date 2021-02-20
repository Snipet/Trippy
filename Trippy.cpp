#include "Trippy.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "cmath"


Trippy::Trippy(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kAttack)->InitDouble("Attack", 10., 0.01, 10000., 0.01, "ms", 0, "", IParam::ShapePowCurve(3.));
  GetParam(kDecay)->InitDouble("Decay", 200., 0.01, 10000., 0.01, "ms", 0, "", IParam::ShapePowCurve(3.));
  GetParam(kSustain)->InitDouble("Sustain", 0.0, 0., 1, 0.01);

  GetParam(kDistortMix)->InitDouble("Distortion Mix", 0., 0., 100., 0.01, "%");
  GetParam(kFilterMix)->InitDouble("Filter Mix", 0., -100., 100., 0.01, "%");
  GetParam(kVolumeMix)->InitDouble("Volume Mix", 0., 0., 100., 0.01, "%");

  GetParam(kDrive)->InitDouble("Drive", 0., 0., 1, 0.001, "");
  GetParam(kCutoff)->InitDouble("Cutoff", 22000, 5., 22000, 0.1, "", 0, "", IParam::ShapePowCurve(5.));
  GetParam(kReso)->InitDouble("Reso", 0.707, 0.1, 15, 0.1, "", 0, "", IParam::ShapePowCurve(2.));

  GetParam(kVolumeGain)->InitDouble("Gain", 100., 0., 200., 0.01, "", 0, "", IParam::ShapePowCurve(1.5));

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
      pGraphics->HideControl(kCutoff, true);
      pGraphics->HideControl(kReso, true);
      pGraphics->HideControl(kVolumeGain, false);
     page = 1;
    };

    auto button2action = [pGraphics, this](IControl* pCaller) mutable {
      pGraphics->HideControl(kVolumeFoo, true);
      pGraphics->HideControl(kDrive, false);
      pGraphics->HideControl(kCutoff, true);
      pGraphics->HideControl(kReso, true);
      pGraphics->HideControl(kVolumeGain, true);
      page = 0;
    };

    auto button3action = [pGraphics, this](IControl* pCaller) mutable {
      pGraphics->HideControl(kVolumeFoo, true);
      pGraphics->HideControl(kDrive, true);
      pGraphics->HideControl(kCutoff, false);
      pGraphics->HideControl(kReso, false);
      pGraphics->HideControl(kVolumeGain, true);
      page = 2;
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
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(-240).GetVShifted(0), kCutoff, "", style));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(-110).GetVShifted(0), kReso, "", style));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(75).GetHShifted(-240).GetVShifted(0), kVolumeGain, "", style));

    pGraphics->AttachControl(new IDistortDisplay(IRECT(0, 50, 480, 250), { kDrive, kDistortMix }), kDistortPlot);
    pGraphics->AttachControl(new ICustomDisplay(IRECT(0, 50, 480, 250), kVolumeFoo), kVolumeDisplay);
    pGraphics->AttachControl(new IFilterDisplay(IRECT(0, 50, 480, 250), {kCutoff, kReso, kFilterMix}), kFilterDisplay);
    pGraphics->HideControl(kCutoff, true);
    pGraphics->HideControl(kReso, true);
    pGraphics->HideControl(kDrive, true);
    pGraphics->HideControl(kFilterFoo, true);


    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(50).GetHShifted(-250).GetVShifted(-272), button1action, "", false, false));
    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(50).GetHShifted(-150).GetVShifted(-272), button2action, "", false, false));
    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(50).GetHShifted(-50).GetVShifted(-272), button3action, "", false, false));
  
  };
#endif
  f = new Filter[NOutChansConnected()];
  lastCutoff = new double[NOutChansConnected()];
  lastReso = new double[NOutChansConnected()];
  for (int i = 0; i < NOutChansConnected(); i++) {
    f[i].set(15000, 2, GetSampleRate(), 0);
  }

}

#if IPLUG_DSP
void Trippy::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  double env = 0;
  const int nChans = NOutChansConnected();
  const double gainMix = GetParam(kVolumeMix)->Value() / 100.;
  const double distortionMix = GetParam(kDistortMix)->Value() / 8.;
  const double filterMix = GetParam(kFilterMix)->Value() / 100.;
  const double drive = GetParam(kDrive)->Value() * 10;
  const double cutoff = GetParam(kCutoff)->Value();
  const double reso = GetParam(kReso)->Value();
  const double gain = GetParam(kVolumeGain)->Value();


  for (int s = 0; s < nFrames; s++) {
    if (t.detect((inputs[0][s] + inputs[1][s]) / 2)) {
      adsr.retrigger();
    }
    env = adsr.tick();
    for (int c = 0; c < nChans; c++) {
      double process = inputs[c][s];
      if (gainMix != 0) {
        process = process * env * gainMix + process * (1 - gainMix) * gain;
      }

      process = tanh(process * (((env) * distortionMix + drive) + 1));
  


      double a = (exp((4 * env * filterMix - 4)) - 0.013);
      double nCutoff = cutoff + a * 17000;
      if (lastCutoff[c] != nCutoff || lastReso[c] != reso) {
        f[c].set(nCutoff, reso, GetSampleRate(), 0);
      }
      lastCutoff[c] = nCutoff;
      lastReso[c] = reso;

      process = f[c].tick(process);

      outputs[c][s] = process;
    }

  }
 // mLastOutputData.vals[0] = (float)env;
 // mEnvLastOutput.vals[0] = (float)adsr[0].getPoint();
  mDistortSender.PushData({ kDistortPlot, {float(env)} });
  mEnvSender.PushData({ kEnvPlot, {float(adsr.getPoint()) } });
  mFilterSender.PushData({ kFilterDisplay, {float(env)} });
  mDisplaySender.PushData({ kVolumeDisplay, {float(env)} });
  //sample** s[5] =  { 0.,0,0,0,0 };
}

void Trippy::OnParamChange(int paramIdx) {
  switch (paramIdx) {

  case kAttack:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr.setAttack(GetParam(kAttack)->Value() * GetSampleRate() / 1000);
    }
    break;
  case kDecay:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr.setDecay(GetParam(kDecay)->Value()*GetSampleRate() / 1000);
    }
    break;
  case kSustain:
    for (int i = 0; i < NOutChansConnected(); i++) {
      adsr.setSustain(GetParam(kSustain)->Value());
    }
    break;
  case kCutoff:
    for (int i = 0; i < NOutChansConnected(); i++) {
      //f[i].set(GetParam(kCutoff)->Value(), GetParam(kReso)->Value(), GetSampleRate());
    }
    break;

  case kReso:
    for (int i = 0; i < NOutChansConnected(); i++) {
      //f[i].set(GetParam(kCutoff)->Value(), GetParam(kReso)->Value(), GetSampleRate());
    }
    break;
  }
 
}
 
void Trippy::OnIdle() {
  mDistortSender.TransmitData(*this);
  mEnvSender.TransmitData(*this);
  mDisplaySender.TransmitData(*this);
  mFilterSender.TransmitData(*this);
}

#endif
