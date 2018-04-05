rsMultiBandEffect::rsMultiBandEffect()
{
  initBands();
}

// setup:

void rsMultiBandEffect::setSampleRate(double newSampleRate)
{
  splitter.setSampleRate(newSampleRate);
}

void rsMultiBandEffect::insertBand(int i, double splitFreq)
{
  splitter.insertBand(i, splitFreq);
  tmp.resize(getNumberOfBands());
}

void rsMultiBandEffect::removeBand(int i, bool mergeRight)
{
  splitter.removeBand(i, mergeRight);
  tmp.resize(getNumberOfBands());
}

void rsMultiBandEffect::setSplitMode(int newMode)
{
  splitter.setSplitMode(newMode);
}

void rsMultiBandEffect::setSplitFrequency(int bandIndex, double newFrequency)
{
  splitter.setSplitFrequency(bandIndex, newFrequency);
}

void rsMultiBandEffect::initBands()
{
  splitter.setNumberOfActiveBands(1);
  tmp.resize(1);
}

double rsMultiBandEffect::getDecibelsAt(int index, double frequency)
{
  return 0; // not yet implemented
}

// processing:

void rsMultiBandEffect::reset()
{
  splitter.reset();
}

//=================================================================================================

rsMultiBandCompressor::rsMultiBandCompressor() 
{
  initBands();
}

rsMultiBandCompressor::~rsMultiBandCompressor()
{
  clearCompressors();
}

void rsMultiBandCompressor::setSampleRate(double newSampleRate)
{
  rsMultiBandEffect::setSampleRate(newSampleRate);
  for(size_t k = 0; k < compressors.size(); k++)
    compressors[k]->setSampleRate(newSampleRate);
}

void rsMultiBandCompressor::setThreshold(int bandIndex, double newThreshold)
{
  compressors[bandIndex]->setThreshold(newThreshold);
}

void rsMultiBandCompressor::setRatio(int bandIndex, double newRatio)
{
  compressors[bandIndex]->setRatio(newRatio);
}

void rsMultiBandCompressor::setAttackTime(int bandIndex, double newAttackTime)
{
  compressors[bandIndex]->setAttackTime(newAttackTime);
}

void rsMultiBandCompressor::setReleaseTime(int bandIndex, double newReleaseTime)
{
  compressors[bandIndex]->setReleaseTime(newReleaseTime);
}

void rsMultiBandCompressor::insertBand(int i, double splitFreq)
{
  rsMultiBandEffect::insertBand(i, splitFreq);
  RAPT::rsInsertValue(compressors, new rosic::Compressor, i);
  // todo: set up compressor setting to the same values as compressor at i (or i-1)
}

void rsMultiBandCompressor::removeBand(int i, bool mergeRight)
{
  rsMultiBandEffect::removeBand(i, mergeRight);
  delete compressors[i];
  RAPT::rsRemove(compressors, i);
  // maybe adjust splitFreq of splitter i or i-1?
}

void rsMultiBandCompressor::initBands()
{
  rsMultiBandEffect::initBands();
  clearCompressors();
  compressors.resize(getNumberOfBands());      // should be initially 1
  for(size_t k = 0; k < compressors.size(); k++)
    compressors[k] = new rosic::Compressor;
}

void rsMultiBandCompressor::reset()
{
  rsMultiBandEffect::reset();
  for(size_t k = 0; k < compressors.size(); k++)
    compressors[k]->reset();
}

void rsMultiBandCompressor::clearCompressors()
{
  for(size_t k = 0; k < compressors.size(); k++)
    delete compressors[k];
  compressors.resize(0);
}