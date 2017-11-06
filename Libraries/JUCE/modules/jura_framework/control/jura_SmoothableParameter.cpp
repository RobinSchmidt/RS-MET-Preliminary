
double rsSmoother::relativeTolerance = 1.e-6;
double rsSmoother::absoluteTolerance = 1.e-12;

rsSmoother::rsSmoother()
{
  smoothingFilter.setOrder(1);
}

//=================================================================================================

rsSmoothingManager::~rsSmoothingManager()
{
  int i;
  for(i = 0; i < size(usedSmoothers); i++)
    delete usedSmoothers[i];
  for(i = 0; i < size(smootherPool); i++)
    delete smootherPool[i];
}

void rsSmoothingManager::addSmootherFor(rsSmoothingTarget* target, double targetValue, 
  double oldValue)
{
  if(target->isSmoothing)
    target->smoother->setTargetValue(targetValue);
  else
  {
    rsSmoother* smoother;
    if(size(smootherPool) > 0)
      smoother = getAndRemoveLast(smootherPool);
    else
      smoother = new rsSmoother;

    smoother->setSmoothingOrder(3);
    smoother->setTimeConstantAndSampleRate(target->getSmoothingTime(), sampleRate);
      // maybe, we should call these conditionally to avoid computations when it doesn't acually 
      // change anything

    smoother->setSmoothingTarget(target);
    smoother->setTargetValue(targetValue);
    smoother->setCurrentValue(oldValue);
    append(usedSmoothers, smoother);
    target->smoothingWillStart();
  }
}

void rsSmoothingManager::removeSmoother(int index)
{
  rsSmoother* smoother = usedSmoothers[index];
  smoother->getSmoothingTarget()->smoothingHasEnded();
  remove(usedSmoothers, index);
  append(smootherPool, smoother);
}

//=================================================================================================

rsSmoothableParameter::rsSmoothableParameter(const juce::String& name, double min, double max, 
  double defaultValue, int scaling, double interval)
  : ModulatableParameter(name, min, max, defaultValue, scaling, interval)
{

}

void rsSmoothableParameter::setValue(double newValue, bool sendNotification, bool callCallbacks)
{
  if(smoothingTime == 0.0 || smoothingManager == nullptr)
    ModulatableParameter::setValue(newValue, sendNotification, callCallbacks);
  else
  {
    double oldValue = getValue();
    ModulatableParameter::setValue(newValue, sendNotification, false);
    smoothingManager->addSmootherFor(this, newValue, oldValue);
  }
}

void rsSmoothableParameter::setSmoothedValue(double newValue)
{
  modulatedValue = unmodulatedValue = value = newValue;
  callValueChangeCallbacks(); // maybe we should call a "NoLock" version of that?
}
