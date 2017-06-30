
RayBouncerAudioModule::RayBouncerAudioModule(CriticalSection *lockToUse) : AudioModule(lockToUse)
{
  ScopedLock scopedLock(*lock);
  moduleName = "RayBouncer";
  setActiveDirectory(getApplicationDirectory() + "Presets/RayBouncer");

  createParameters();
}

void RayBouncerAudioModule::createParameters()
{
  ScopedLock scopedLock(*lock);

  std::vector<double> defaultValues;
  typedef MetaControlledParameter Param;
  Param* p;

  p = new Param("Frequency", 20.0, 20000.0, 1000.0, Parameter::EXPONENTIAL);
  defaultValues.clear();
  defaultValues.push_back(125.0);
  defaultValues.push_back(250.0);
  defaultValues.push_back(500.0);
  defaultValues.push_back(1000.0);
  defaultValues.push_back(2000.0);
  defaultValues.push_back(4000.0);
  defaultValues.push_back(8000.0);
  defaultValues.push_back(16000.0);
  p->setDefaultValues(defaultValues);
  addObservedParameter(p);
  p->setValueChangeCallback<RayBouncerAudioModule>(this, &RayBouncerAudioModule::setFrequency);


  p = new Param("Size", 0.25, 1.0, 1.0, Parameter::EXPONENTIAL);
  addObservedParameter(p);
  p->setValueChangeCallback<rsRayBouncerDriverD>(&rayBouncer, 
    &rsRayBouncerDriverD::setEllipseSize);

  p = new Param("AspectRatio", 0.5, 2.0, 1.0, Parameter::EXPONENTIAL);
  addObservedParameter(p);
  p->setValueChangeCallback<rsRayBouncerDriverD>(&rayBouncer, 
    &rsRayBouncerDriverD::setEllipseAspectRatio);

  p = new Param("Angle", 0.0, 360.0, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<rsRayBouncerDriverD>(&rayBouncer, 
    &rsRayBouncerDriverD::setEllipseAngleDegrees);

  p = new Param("CenterX", -0.2, 0.2, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<rsRayBouncerDriverD>(&rayBouncer, 
    &rsRayBouncerDriverD::setEllipseCenterX);

  p = new Param("CenterY", -0.2, 0.2, 0.0, Parameter::LINEAR);
  addObservedParameter(p);
  p->setValueChangeCallback<rsRayBouncerDriverD>(&rayBouncer, 
    &rsRayBouncerDriverD::setEllipseCenterY);

  // maybe we need to call the valueChangeCallback in setValueChangeCallback to make everything
  // initially consistent...

  // add initial condition parameters - the output is very sensitive to the conditions
  // swicthing presets back and forth always sounds different because of different initial 
  // conditions
}

// Editor creation:

//AudioModuleEditor* RayBouncerAudioModule::createEditor()
//{
//  return new jura::RayBouncerAudioModuleEditor(this);
//}

// audio processing:

void RayBouncerAudioModule::processBlock(double **inOutBuffer, int numChannels, int numSamples)
{
  jassert(numChannels == 2);
  double* x = inOutBuffer[0];
  double* y = inOutBuffer[1];
  for(int n = 0; n < numSamples; n++)
    rayBouncer.getSampleFrame(x[n], y[n]);
}

// parameter setters (callback targets for the Parameter objects):

void RayBouncerAudioModule::setSampleRate(double newSampleRate)
{
  sampleRate = newSampleRate;
  rayBouncer.setFrequencyAndSampleRate(frequency, sampleRate);
}

void RayBouncerAudioModule::reset()
{
  rayBouncer.reset();
}

void RayBouncerAudioModule::setFrequency(double newFrequency)
{
  frequency = newFrequency;
  rayBouncer.setFrequencyAndSampleRate(frequency, sampleRate);
}

//=================================================================================================
// the GUI editor class for the RayBouncer:

//RayBouncerAudioModuleEditor::RayBouncerAudioModuleEditor(
//  jura::RayBouncerAudioModule *newRayBouncerToEdit) 
//  : AudioModuleEditor(newRayBouncerToEdit)
//{
//  ScopedLock scopedLock(*lock);
//
//  setHeadlineText("RayBouncer");
//
//  // assign the pointer to the edited object:
//  jassert(newRayBouncerToEdit != nullptr ); // you must pass a valid module here
//  rayBouncerToEdit = newRayBouncerToEdit;
//
//  // create the widgets and assign the automatable parameters to them:
//  addWidget( cutoffSlider = new AutomatableSlider() );
//  cutoffSlider->assignParameter( ladderToEdit->getParameterByName("Cutoff") );
//  cutoffSlider->setSliderName("Cutoff");
//  cutoffSlider->setDescription("Cutoff frequency in Hz");
//  cutoffSlider->setDescriptionField(infoField);
//  cutoffSlider->setStringConversionFunction(&hertzToStringWithUnitTotal5);
//}

//void LadderEditor::resized()
//{
//  ScopedLock scopedLock(*lock);
//  AudioModuleEditor::resized();
//
//  int x  = 0;
//  int y  = 0;
//  int w  = getWidth();
//  int h  = getHeight();
//  y = getPresetSectionBottom();
//
//  cutoffSlider->setBounds(      4, y+4, w2-4, 16);
//}
