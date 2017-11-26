RotationOscillatorAudioModule::RotationOscillatorAudioModule(CriticalSection *lockToUse, 
  MetaParameterManager* metaManagerToUse, ModulationManager* modManagerToUse)
  : AudioModuleWithMidiIn(lockToUse, metaManagerToUse, modManagerToUse)
{
  ScopedLock scopedLock(*lock);
  moduleName = "EllipsoidOsc";
  setActiveDirectory(getApplicationDirectory() + "/Presets/EllipsoidOsc");

  createParameters();
}

void RotationOscillatorAudioModule::createParameters()
{
  ScopedLock scopedLock(*lock);

  typedef RAPT::rsRotationOscillator<double> RO;
  RO* ro = &oscCore;

  typedef ModulatableParameter Param;
  Param* p;

  p = new Param("FreqScaleX", 0.0, 10.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setFrequencyScalerX);

  p = new Param("FreqScaleY", 0.0, 10.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setFrequencyScalerY);

  p = new Param("FreqScaleZ", 0.0, 10.0, 1.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setFrequencyScalerZ);


  p = new Param("OutRotX", -180, +180.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setOutputRotationX);

  p = new Param("OutRotY", -180, +180.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setOutputRotationY);

  p = new Param("OutRotZ", -180, +180.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<RO>(ro, &RO::setOutputRotationZ);
}

void RotationOscillatorAudioModule::processBlock(double **inOutBuffer, int numChannels, int numSamples)
{
  //double x, y, z;
  for(int n = 0; n < numSamples; n++)
  {
    oscCore.getSampleFrameStereo(&inOutBuffer[0][n], &inOutBuffer[1][n]);
    //inOutBuffer[0][n] = inOutBuffer[1][n] = oscCore.getSample(); // produce stereo output later
  }
}

void RotationOscillatorAudioModule::setSampleRate(double newSampleRate)
{
  oscCore.setSampleRate(newSampleRate);
}

void RotationOscillatorAudioModule::reset()
{
  oscCore.reset();
}

void RotationOscillatorAudioModule::noteOn(int noteNumber, int velocity)
{
  oscCore.setFrequency(pitchToFreq(noteNumber));
  oscCore.reset();
}