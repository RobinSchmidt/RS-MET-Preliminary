#pragma once


/** A class to create a sinusoidal model for quasi-harmonic signals. The assumed harmonic 
relationship between the partials allows for a different analysis algorithm that is tailored 
specifically for harmonic signals and is supposed to give better results than a more general
sinusoidal analysis algorithm that doesn't make such an assumption (if the assumption indeed holds
true for the analyzed signal, of course). The algorithm works as follows:

-pre-process audio (pitch flattening):
 -obtain cycle marks
 -obtain a time warping map that makes the cycle-marks equidistant (distance is chosen to be a 
  power of two >= the longest cycle in the input signal)
 -time-warp the signal -> all cycles have the same, FFT-friendly length
-harmonic analysis:
 -analyze each cycle of pitch flattened signal by an FFT
 -the FFT size is equal to the cycle-length -> frequencies don't need to be estimated, they are 
  known in advance
 -only amplitude and phase have to be measured
-post-process model data (account for pitch flattening):
 -move time instants of datapoints according to the inverse time-warping map
 -modify frequencies according to reciprocal of the slope of time-warping map

The so obtained model models inharmonicity, transient and noise in the input signal as fast 
variations of instantaneous frequencies and amplitudes of the partials - when the envelopes are
lowpass-filtered before resynthesis, we can resynthesize only the quasi-harmonic part of the sound.
Time domain subtraction from the original should give a residual containing inharmonic, noisy and 
transient parts. */

template<class T>
class rsHarmonicAnalyzer
{

public:



  rsHarmonicAnalyzer();



  /** \name Setup */


  /** Sets up the sample-rate. This determines the values of the frequencies that will be written
  into the model. */
  void setSampleRate(T newRate) { sampleRate = newRate; }


  /** \name Processing */

  /** Analyzes the given input sound and returns the sinusoidal model object that models the given 
  sound. */
  RAPT::rsSinusoidalModel<T> analyze(T* sampleData, int numSamples);

protected:

  // todo: refactor to use these 3 functions in analyze:

  /** The first step in the analysis algo is to pre-process the audio by flattening the pitch, 
  which is done by this function. The flattened signal will be stored in out member array y and 
  this function will also fill the member arrays tIn, tOut that contain the time warping map that
  has been used for flattening (sampled at instants of cycle marks). These arrays will be later 
  used for post processing the model data to accoutn for the pitch-flattening. The boolean return
  value informs, if the process was successful (it will fail, if it can't find at least 2 cycle 
  marks). */
  bool flattenPitch(T* sampleData, int numSamples);

  /** The second step in the analysis algo is to perform an FFT on each cycle of pitch-flattened
  signal. Because the pitch is now flat, each cycle has the same length (which was chosen to be
  a power of two, greater or equal to the length of the longest cycle in the input signal). This
  fills in the model data with (preliminary) values. */
  void analyzeHarmonics(RAPT::rsSinusoidalModel<T>& mdl);

  /** The third step in the analysis algo is to modify the time and frequency data to account for 
  the pitch flattening that was done in the first step. */
  void deFlattenPitch(RAPT::rsSinusoidalModel<T>& mdl);

  void handleEdges(RAPT::rsSinusoidalModel<T>& mdl);
  // not yet implemented

  /** During most of our computational steps in the algo, we represent time in units of samples,
  but ultimately, rsSinusoidalModel wants to have time values in seconds, so this function is used
  as final step to convert all values. */
  void convertTimeUnit(RAPT::rsSinusoidalModel<T>& mdl);



  /** Returns length of time-warping map (sampled at cycle marks). */
  int getMapLength() const { return (int) tIn.size(); }
  // rename to getTimeWarpMapLength
   
  /** Returns the number of FFT analysis frames. */
  int getNumFrames() const { return getMapLength()-1; }

  /** Returns the number of FFT bins (including DC - maybe rename to getNumBins) */
  int getNumHarmonics() const { return blockSize / 2; }

  /** Returns the number of datapoints (per partial) in the sinusoidal model. */
  //int getNumDataPoints() const { return getNumFrames(); }
  int getNumDataPoints() const { return getNumFrames() + 2; }
  // later: getNumFrames() + 2 for fade in/out

  /** Computes and returns an array of cycle-marks. */
  std::vector<T> findCycleMarks(T* x, int N);

  /** Returns the time instant that should be associated with the frame of given index 
  (before post-processing). The time stamp will be baed on the content of tOut member array. */
  T getTimeStampForFrame(int frameIndex);
  // maybe rename to getWarpedTimeForFrame and have a corresponding getUnWarpedTimeforFrame
  // function (that uses tIn instead of tOut)...this will actually be used to *replace* the 
  // original time-stamp data later in the post-processing - so we may not actually need to 
  // compute the values before post-processing - but for conceptual clarity of the algorithm, 
  // we just do it for the time being (the effort is neglibilibe anyway)

  T getUnWarpedTimeStampForFrame(int frameIndex);
  // maybe rename to getUnWarpedSampleIndexForFrame




  /** Used internally to fill in the data in the model at the given frame-index based on the 
  current content of our "sig" buffer member variable. The time-stamp of the frame should be passed
  by the caller - frequency, magnitude and phase data are computed from the FFT of the sig 
  buffer. */
  void fillHarmonicData(RAPT::rsSinusoidalModel<T>& mdl, int frameIndex, T timeStamp);




  //RAPT::rsPitchFlattener<T, T> flattener;
  RAPT::rsFourierTransformerRadix2<T> trafo;


  int blockSize = 0;    // FFT block size

  T sampleRate = 1;
  T sincLength = 64.0;  // length of sinc-interpolator for time-warping

  std::vector<T> y;     // pre-processed (time-warped) signal

  // buffers:
  std::vector<T> tIn, tOut;      // time-warping map (sampled at cycle marks)
  // maybe rename to "to", "tw" for original and warped

  std::vector<T> sig, mag, phs;  // blocks of signal, magnitude and phase

};