#ifndef rosic_MultiBandCompressor_h
#define rosic_MultiBandCompressor_h

namespace rosic
{

/** Baseclass for multiband effects such as multiband-compressors, -distortion. etc. */

class rsMultiBandEffect
{

public:

  /** Constructor. */
  rsMultiBandEffect();

  //---------------------------------------------------------------------------------------------
  /** \name Setup */

  void setSampleRate(double newSampleRate);

  void setNumberOfBands(int newNumber);

  /** Adds a new band after the band at the given index. If the index is that of the last band, 
  the new band will become the last band, otherwise, it will be inserted between index and index+1.
  In the latter case, it will probably make sense, if the new frequency is somewhere between those 
  of the bands at index and index+1 - however, this class actually doesn't care about that. */
  void insertBand(int index, double splitFrequency); 
  // not yet implemented

  /** Removes the band with the given index. The frequency range that was occupied by this band 
  will then either be covered by the former left or right neighbour band, depending on the second
  boolean parameter. When the first ot last band is removed, the second or second-to-last will 
  become the new first or last band. When there is only a single band, this function will have no 
  effect (you can't remove all bands - there's always at least one). */
  void removeBand(int index, bool mergeWithRightNeighbour = false);
  // not yet implemented


  void setSplitMode(int newMode);

  void setSplitFrequency(int bandIndex, double newFrequency);

  //---------------------------------------------------------------------------------------------
  /** \name Inquiry */

  /** Retruns the number of bands that are currently active. */
  int getNumberOfBands() const { return numBands; }

  /** Returns the maximum number of bands that is supported. */
  int getMaxNumberOfBands() const { return maxNumBands; }

  /** Returns the upper cutoff frequency for the band with given index. */
  double getSplitFrequency(int index) { return splitterL.getSplitFrequency(index); }

  /** Returns the magnitude response in decibels for the band with given index at the given 
  frequency. */
  double getDecibelsAt(int index, double frequency);

  //---------------------------------------------------------------------------------------------
  /** \name Processing */

  /** Splits an incoming stereo signal into "numBands" bands and fills our member arrays 
  tmpL, tmpR with the individual band outputs. Subclasses are supposed to call this before 
  processing each band. */
  INLINE void split(double *inOutL, double *inOutR)
  {
    splitterL.processSampleFrame(*inOutL, &tmpL[0]);
    splitterR.processSampleFrame(*inOutR, &tmpR[0]);
  }

  /** Recombines the bands from our member arrays tmpL, tmpR into the outputs. Subclasses are 
  supposed to call this after processing each indiviudal band to produce the final recombined
  output. */
  INLINE void recombine(double *inOutL, double *inOutR)
  {
    *inOutL = *inOutR = 0;
    for(int k = 0; k < numBands; k++)
    {
      *inOutL += tmpL[k];
      *inOutR += tmpR[k];
    }
  }

  /** Resets the states of the band-splitting filters. */
  void reset();

protected:

  RAPT::rsMultiBandSplitter<double, double> splitterL, splitterR;
  std::vector<double> tmpL, tmpR; // temporary buffers
  int numBands = 1;
  int maxNumBands = 16; // preliminary - make indefinite in the future

};

//=================================================================================================

/** A multiband compressor with an arbitrary number of bands and perfect reconstruction, i.e. with 
neutral settings, it will be totally transparent (up to roundoff error) and not color the signal in
any way. */

class rsMultiBandCompressor : public rsMultiBandEffect
{

public:

  //---------------------------------------------------------------------------------------------
  /** \name Construction/Destruction */

  /** Constructor. */
  rsMultiBandCompressor();

  /** Destructor */
  ~rsMultiBandCompressor();

  //---------------------------------------------------------------------------------------------
  /** \name Setup */

  void setSampleRate(double newSampleRate);

  void setThreshold(int bandIndex, double newThreshold);

  void setRatio(int bandIndex, double newRatio);

  void setAttackTime(int bandIndex, double newAttackTime);

  void setReleaseTime(int bandIndex, double newReleaseTime);

  //-----------------------------------------------------------------------------------------------
  /** \name Processing */

  /** Calculates one output stereo sample-frame at a time. */
  INLINE void getSampleFrameStereo(double *inOutL, double *inOutR);

  void reset();

  //===============================================================================================

protected:

  std::vector<Compressor*> compressors;

};

//-----------------------------------------------------------------------------------------------
// inlined functions:

INLINE void rsMultiBandCompressor::getSampleFrameStereo(double *inOutL, double *inOutR)
{
  split(inOutL, inOutR);
  for(int k = 0; k < numBands; k++)  // compress individual bands
    compressors[k]->getSampleFrameStereo(&tmpL[k], &tmpR[k]);
  recombine(inOutL, inOutR);
}

}

#endif 
