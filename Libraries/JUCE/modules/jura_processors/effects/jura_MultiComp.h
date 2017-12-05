#ifndef jura_MultiComp_h
#define jura_MultiComp_h


class JUCE_API MultiCompAudioModule : public jura::ModulatableAudioModule
{

public:

  MultiCompAudioModule(CriticalSection *lockToUse,
    MetaParameterManager* metaManagerToUse = nullptr, ModulationManager* modManagerToUse = nullptr);

  /** Creates the static parameters for this module (i.e. parameters that are not created
  dynamically and are thus always there). */
  virtual void createParameters();

  // overriden from AudioModule baseclass:
  virtual void processBlock(double **inOutBuffer, int numChannels, int numSamples) override;
  virtual void setSampleRate(double newSampleRate) override;
  virtual void reset() override;

protected:

  rosic::rsMultiBandCompressor multiCompCore;

  int maxNumBands  = 0;  // assigned in constructor
  int selectedBand = 0;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiCompAudioModule)
};

#endif