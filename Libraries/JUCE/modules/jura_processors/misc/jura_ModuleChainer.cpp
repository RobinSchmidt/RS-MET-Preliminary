
ModuleChainer::ModuleChainer(CriticalSection *lockToUse) : AudioModuleWithMidiIn(lockToUse)
{
  ScopedLock scopedLock(*plugInLock);
  moduleName = "ModuleChainer";
  setActiveDirectory(getApplicationDirectory() + "/ModuleChainerPresets");
}

AudioModule* ModuleChainer::createModule(const String& type)
{
  if(type == "None")         return new DummyModule( plugInLock);
  if(type == "PhaseScope")   return new PhaseScope(  plugInLock);
  if(type == "Enveloper")    return new Enveloper(   plugInLock);
  if(type == "Ladder")       return new Ladder(      plugInLock);
  if(type == "PhasorFilter") return new PhasorFilter(plugInLock);
  // add more module types here...

  jassertfalse;  // unknown module type requested
  return nullptr;
}

// overrides:

AudioModuleEditor* ModuleChainer::createEditor()
{
  return new ModuleChainerEditor(this);
}

void ModuleChainer::processBlock(double **inOutBuffer, int numChannels, int numSamples)
{
  jassert(numChannels == 2);
  for(int i = 0; i < modules.size(); i++)
    modules[i]->processBlock(inOutBuffer, numChannels, numSamples);
}

void ModuleChainer::setSampleRate(double newSampleRate)
{
  for(int i = 0; i < modules.size(); i++)
    modules[i]->setSampleRate(newSampleRate);
}

void ModuleChainer::noteOn(int noteNumber, int velocity)
{
  for(int i = 0; i < modules.size(); i++)
  {
    AudioModuleWithMidiIn *m = dynamic_cast<AudioModuleWithMidiIn*> (modules[i]);
    if(m != nullptr)
      m->noteOn(noteNumber, velocity);
  }
}

void ModuleChainer::noteOff(int noteNumber)
{
  for(int i = 0; i < modules.size(); i++)
  {
    AudioModuleWithMidiIn *m = dynamic_cast<AudioModuleWithMidiIn*> (modules[i]);
    if(m != nullptr)
      m->noteOff(noteNumber);
  }
}

void ModuleChainer::reset()
{
  for(int i = 0; i < modules.size(); i++)
    modules[i]->reset();
}

//=================================================================================================

ModuleChainerEditor::ModuleChainerEditor(jura::ModuleChainer *moduleChainerToEdit)
  : AudioModuleEditor(moduleChainerToEdit)
{
  ScopedLock scopedLock(*plugInLock);
  chainer = moduleChainerToEdit;

  //createWidgets();

  setSize(200, 100);
}
