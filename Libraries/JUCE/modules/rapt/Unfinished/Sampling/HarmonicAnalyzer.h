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
 -only amplitude and phase have to be measured (i.e. simply read off from the FFT data)
-post-process model data (account for pitch flattening):
 -move time instants of datapoints according to the inverse time-warping map
 -modify frequencies according to the applied stretch factors

The so obtained model models any inharmonicity, transients and noise in the input signal as fast 
variations of instantaneous frequencies and amplitudes of the partials - when the envelopes
are lowpass-filtered before resynthesis, we can resynthesize only the quasi-harmonic part of the 
sound. Time domain subtraction from the original should give a residual containing inharmonic, 
noisy and transient parts. The model also includes a DC component which models - apparently - any
DC present in the signal, but if the amplitude of the DC varies, it may also model subharmonic
content. However, subharmonic content may also be partially modeled/encoded by amplitude- and 
frequency modulation of the harmonics.

\todo: give the user the option to prepend and append some zeros before the analysis to avoid 
edge-artifacts - this padding, if used, has to be accounted for in the model after analysis is 
finished. the padding should be at least one cycle long, maybe use two to be on the safe side

-maybe have an "oversampling" option: let the hop-size be a half or quarter of the block-size
 -samples amplitudes and phases more densely in time

-let the user choose to set up the fundamental or to estimate it (the value that is used to tune 
 the bandpass in the cycle-mark finder - maybe when a "fundamental" member is zero, it means, it 
 should be detected

-make a sample editor module for ToolChain:
 -let the user choose background: blank (some color according to colorscheme), spectrogram, 
  phasogram, etc.. and forground: blank, waveform, partial freqs, etc...
-the user can make edits based on a sinusoidal model of the sound, spectrogram and waveform (and 
 later maybe more representations - what about a wavelet trafo, for example? or a single big 
 spectrum for the whole sound)
-it should be possible to save and load sinusoidal models (devise a file-format)

*/

template<class T>
class rsHarmonicAnalyzer
{

public:

  rsHarmonicAnalyzer();


  /** \name Setup */

  /** Sets up the sample-rate. This determines the values of the frequencies that will be written
  into the model. */
  void setSampleRate(T newRate) { sampleRate = newRate; }

  //void setRemoveDC(bool shouldBeRemoved);

  /** Sets the length of the sinc interpolator that is used for the pitch flattening. In tests with
  a sum-of-sines input, it turned out that the amplitude of the residual seems to be roughly 
  inverserly proportional to that length. But that's only a rough tendency - for example, there's a 
  huge difference (factor ~3) between 16 and 17 ...more research needed */
  void setSincInterpolationLength(T newLength) { sincLength = newLength; }

  /** This option can be used to remove any harmonics that exceed the Nyquist limit, even if just 
  temporarily. The analysis may prodcue such frequencies due to the fact that the original audio is 
  stretched before analysis and the post-processing then shifts all frequencies up. However, if a 
  (short) cycle gets stretched out a lot, it will not actually contain any high-freq content above 
  the original Nyquist rate - so when a partial in the model goes above that limit, it will usually
  have zero amplitude. ...it turned out that this option may sometimes remove valid high-freq 
  content, so it's off by default ...more research needed....  */
  void removePotentiallyAliasingHarmonics(bool shouldRemove)
  {
    antiAlias = shouldRemove;
  }



  /** \name Inquiry */

  std::vector<T> getOriginalTimeStamps() { return tIn; }


  std::vector<T> getWarpedTimeStamps() { return tOut; }


  /** \name Processing */

  /** Analyzes the given input sound and returns the sinusoidal model object that models the given 
  sound. */
  RAPT::rsSinusoidalModel<T> analyze(T* sampleData, int numSamples);

protected:


  /** The first step in the analysis algo is to pre-process the audio by flattening the pitch, 
  which is done by this function. The flattened signal will be stored in our member array y and 
  this function will also fill the member arrays tIn, tOut that contain the time warping map that
  has been used for flattening (sampled at instants of cycle marks). These arrays will be later 
  used for post processing the model data to account for the pitch-flattening. The boolean return
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

  /** Removes those partials in the model that are above the original Nyquist freqeuncy. Such 
  partials may be generated by the analysis due to the fact that original signal is pitch-shifted
  down in the flattening step. They will typically have amplitude close to zero because, of 
  course, the original signal cannot contain these frequencies - except at the edges - there they
  may indeed have nonzero amplitudes if the original signal is harshly cut off there. Anyway - 
  zero amplitude or not - we don't want them in the model - their mere existence is an artifact
  of the downshifting step. */
  void removeAliasing(RAPT::rsSinusoidalModel<T>& mdl);

  /** Fills the initial datapoint at time zero and the final datapoint at time 
  (numSamples-1)/sampleRate - these are treated separately. */
  void handleEdges(RAPT::rsSinusoidalModel<T>& mdl);

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
  // todo: maybe decide in advance, which harmonics will or will not alias and analyze only those
  // which won't as an optimization (instead of analyzing them all and then discarding the aliasing 
  // ones) - in this case, return the actual number of harmonics here

  /** Returns the number of datapoints (per partial) in the sinusoidal model. */
  //int getNumDataPoints() const { return getNumFrames(); }
  int getNumDataPoints() const { return getNumFrames() + 2; }
  // later: getNumFrames() + 2 for fade in/out

  /** Computes and returns an array of cycle-marks. */
  std::vector<T> findCycleMarks(T* x, int N);

  /** Returns the time instant that should be associated with the frame of given index 
  (before post-processing). The time stamp will be based on the content of tOut member array. */
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
  rsFourierTransformerRadix2<T> trafo;
  //rsCycleMarkFinder<T> cycleMarkFinder;


  int blockSize = 0;    // FFT block size

  bool antiAlias = false;

  T sampleRate = 1;
  T sincLength = 512.0;  // length of sinc-interpolator for time-warping

  std::vector<T> y;     // pre-processed (time-warped) signal



  // buffers:
  std::vector<T> tIn, tOut;      // time-warping map (sampled at cycle marks)
  // maybe rename to "to", "tw" for original and warped

  std::vector<T> sig, mag, phs;  // blocks of signal, magnitude and phase

};