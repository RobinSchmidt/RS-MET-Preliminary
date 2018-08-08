#ifndef jura_VariousModulators_h
#define jura_VariousModulators_h

class JUCE_API TriSawModulatorModule : public AudioModuleWithMidiIn, public ModulationSource
{

public:

  TriSawModulatorModule(CriticalSection *lockToUse,
    MetaParameterManager* metaManagerToUse = nullptr, ModulationManager* modManagerToUse = nullptr);

  // override setSampleRate

  virtual void noteOn(int noteNumber, int velocity) override
  {
    core.reset();
  }

  virtual void updateModulationValue() override
  {
    modValue = core.getSample();
  }
  // todo: change interface in order to return the modValue and let the framework take care of 
  // where to store it


protected:

  virtual void createParameters();


  rosic::rsTriSawModulator core; // maybe use a pointer to be able to wrap existing objects

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriSawModulatorModule)
};
// maybe make it modulatable? or is it already?

#endif