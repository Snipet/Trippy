#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "TransientProcessor.h"
#include "ADSREnvelope.h"
#include "ADSRf.h"
#include "IADSR.h"
#include "iLFOG.h"
#include "IDistortDisplay.h"
#include "IControls.h"
#include "IPlugPaths.h"
#include "ICustomDisplay.h"
#include "BiquadFilterf.h"
#include "IFilterDisplay.h"
const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kDrive,
  kAttack,
  kDecay,
  kSustain,
  kDistortMix,
  kFilterMix,
  kVolumeMix,
  kVolumeFoo,
  kCutoff,
  kReso,
  kFilterFoo,
  kFilterType,
  kVolumeGain,
  kNumParams
};

enum EControlTags
{
  kLedTrigger = 0,
  kEnvPlot,
  kDistortPlot,
  kDriveKnob,
  kVolumeDisplay,
  kFilterDisplay,
  kCtrlTags
};

using namespace iplug;
using namespace igraphics;

class Trippy final : public Plugin
{
public:
  Trippy(const InstanceInfo& info);
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;



private:
  ISender<1> mFilterSender;
  ISender<1> mDistortSender;
  ISender<1> mEnvSender;
  ISender<2> mDisplaySender;
  //ISenderData<1> mLastOutputData = { kDistortPlot, 1, 0 };
  ISenderData<1> mEnvLastOutput = { kEnvPlot, 1, 0 };
  ISenderData<1> mFilterLastOutput = { kFilterDisplay, 1, 0 };
  TransientProcessor t;
  Filter* f;
  ADSRM adsr;
  int page;

  double* lastCutoff;
  double* lastReso;
};



/*
TODO / Good Ideas:



*/