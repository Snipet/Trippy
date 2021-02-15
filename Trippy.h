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
  kNumParams
};

enum EControlTags
{
  kLedTrigger = 0,
  kEnvPlot,
  kDistortPlot,
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
  ISender<1> mDistortSender;
  ISender<1> mEnvSender;
  //ISenderData<1> mLastOutputData = { kDistortPlot, 1, 0 };
  ISenderData<1> mEnvLastOutput = { kEnvPlot, 1, 0 };
  TransientProcessor* t;
  ADSRM* adsr;
  double env;
};



/*
TODO / Good Ideas:

-For the routing table matrix thing, just do a few rows of effects and have a moduation amount to the env for how much it is modulated.




*/