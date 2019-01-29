template<class T>
std::vector<T> SinusoidalSynthesizer<T>::synthesize(const rsSinusoidalModel<T>& model) const
{
  int N = model.getLengthInSamples(sampleRate);
  std::vector<T> x(N);
  RAPT::rsArray::fillWithZeros(&x[0], N);
  for(size_t i = 0; i < model.getNumPartials(); i++)
    synthesizePartial(model.getPartial(i), &x[0], N, -model.getStartTime()); 
  return x;
}

template<class T>
void SinusoidalSynthesizer<T>::synthesizePartial(
  const rsSinusoidalPartial<T>& partial, T* x, int xLength, T timeShift) const 
{
  // figure out number of samples to produce:
  int nStart = (int) floor(sampleRate * (partial.getStartTime() + timeShift));
  int nEnd   = (int) ceil( sampleRate * (partial.getEndTime()   + timeShift)) + 1;
  nStart = rsClip(nStart, 0, xLength);
  nEnd   = rsClip(nEnd,   0, xLength);
  int N = nEnd - nStart;  // number of samples to generate

  // create arrays for non-interpolated instantaneous parameter data:
  std::vector<T> td, fd, ad, wpd;
  partial.getDataArrays(td, fd, ad, wpd);
  td = td + timeShift; // todo: implement vector += scalar operator and use: td += timeShift;
  std::vector<T> upd = unwrapPhase(td, fd, wpd);

  // factor out:
  // interpolate the amplitude and unwrapped phase data to sample-rate:
  // create time axis:
  size_t M = td.size();
  std::vector<T> t(N), a(N), p(N); // interpolated instantaneous data
  for(size_t n = 0; n < N; n++)          // fill time-array
    t[n] = (nStart + n) / sampleRate;    // ...optimize

  // interpolate phase:
  if(cubicPhase) rsNaturalCubicSpline(&td[0], &upd[0], (int)M, &t[0], &p[0], (int)N);
  else           rsInterpolateLinear( &td[0], &upd[0], (int)M, &t[0], &p[0], (int)N);

  // interpolate amplitude:
  if(cubicAmplitude) rsNaturalCubicSpline(&td[0], &ad[0], (int)M, &t[0], &a[0], (int)N);
  else               rsInterpolateLinear( &td[0], &ad[0], (int)M, &t[0], &a[0], (int)N);

  // it seems cubic interpolation for the phase and linear for the amplitude is most suitable,
  // although, for the amplitude, we may also use cubic - but linear for the phase leads to audible
  // artifacts (sort of clicks) at the segement junctions
  // -maybe linear interpolation of frequency with subsequent integration would work (due to the
  //  smoothing effect of integration) - but that would make it difficult to incorporate the 
  //  target phases (i think, we would have to produce an interpolated phase-delta array and add 
  //  that to an interpolated preliminary unwrapped-phase array)
  // -maybe using cubic interpolation for frequency, than integrating and then adding an 
  //  interpolated phase-delta array could give and even smoother freq-trajectory? maybe try it
  // -or maybe use higher order numeric integration on the non-interpolated freq-data?
  // -but when smoothing comes into play later, linear interpolation of the phase might be not so
  //  bad as it is without smoothing
  // -however - when the data comes from an analysis, it will be typically much denser in time than
  //  in this test (like one datapoint per cycle), so the difference between the interpolation
  //  methods may be not that important anymore - but for sparse data, it is crucial
  // -maybe the synthesizer should have "presets" for most useful combinations of synthesis 
  //  parameters

  // maybe let the select to apply a bidirectional smoothing filter to the amplitude and/or phase
  // ...and/or maybe to the frequency data before integration (but then it would have to opreate on
  // non-uniformly sampled data). when one is to be applied to the phase, we should first subtract
  // out the linear trend, then apply it and add the trend back

  // maybe provide hook-functions that can manipulate the amplitude/phase data after interpolation
  // maybe have subclasses that also work with real and imaginare parts instead of magnitude/phase
  // (and apply filters to those) ...but that is already something for the transformations

  // synthesize the sinusoid and add it to what's already there:
  std::vector<T> s(N); // needed here only for plotting, remove for production code
  for(size_t n = 0; n < N; n++)
    s[n] = x[nStart+n] += a[n] * cos(p[n]);
  // we use the cosine (not the sine) because that's what's used in the literature - probably 
  // because it's consistent with representing real sinusoids as the real part of complex
  // sinusoids (using the sine, we would have to take the imaginary part instead)


  //GNUPlotter plt;
  ////plt.addDataArrays(M, &td[0], &fd[0]);
  ////plt.addDataArrays((int)M, &td[0], &upd[0]);
  //plt.addDataArrays((int)N, &t[0],  &p[0]); // interpolated phase
  //plt.addDataArrays((int)N, &t[0],  &a[0]);   // interpolated amplitude
  ////plt.addDataArrays((int)N, &t[0],  &s[0]);   // produced sinusoid
  //plt.plot();
}

template<class T>
std::vector<T> SinusoidalSynthesizer<T>::unwrapPhase(const std::vector<T>& t,
  const std::vector<T>& f, const std::vector<T>& wp) const
{
  size_t M = t.size();
  RAPT::rsAssert(f.size()  == M);
  RAPT::rsAssert(wp.size() == M);
  std::vector<T> up(M);  // unwrapped phase

  // obtain preliminary uwrapped phase data points by numerically integrating the frequency:
  rsNumericIntegral(&t[0], &f[0], &up[0], (int)M, wp[0]);
  up = 2*PI*up; // convert from "number of cycles passed" to radians

  // incorporate the target phase values into the unwrapped phase:
  for(size_t m = 0; m < M; m++) {
    T wp1 = fmod(up[m], 2*PI);  // 0..2*pi
    T wp2 = fmod(wp[m], 2*PI);  // 0..2*pi
    T d   = wp2-wp1;            // -2*pi..2*pi, delta between target phase and integrated frequency
    if(d < 0) d += 2*PI;        // 0..2*PI
    if(d > PI)                  // choose adjustment direction of smaller phase difference
      d -= 2*PI;                // -pi..pi
    up[m] += d;                 // re-adjust final unwrapped phase
    if(accumulatePhaseDeltas)
      for(size_t k = m+1; k < M; k++) // re-adjustment at m should also affect m+1, m+2, ...
        up[k] += d;
  }
  // maybe provide an alternative implementation that uses the measured (unwrapped) phases y and 
  // the measured instantaneous frequencies as y'' in a Hermite interpolation scheme (this is how
  // it's described in the literature). to unwrap the phase of datapoint m, take the phase of m-1
  // and add the integral over t[m-1]..t[m] of the average frequency (f[m-1]+f[m])/2 and choose
  // p[m] + 2*pi*k as unwrapped where k is chosen such that p[m] is closest to value obtained from
  // integration - use this function here as dispatcher between the different algorithms

  return up;
}

// template instantiation:
template class SinusoidalSynthesizer<double>;

//=================================================================================================

// move to library:
/*
template<class T>
rsMatrix<T> matrixMagnitudes(const rsMatrix<std::complex<T>>& A)
{
  int N = A.getNumRows();
  int M = A.getNumColumns();
  rsMatrix<T> mags(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      mags(i, j) = abs(A(i, j));
  return mags;
}
template<class T>
rsMatrix<T> matrixPhases(const rsMatrix<std::complex<T>>& A)
{
  int N = A.getNumRows();
  int M = A.getNumColumns();
  rsMatrix<T> phases(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      phases(i, j) = arg(A(i, j));
  return phases;
}
// maybe factor out common code...maybe something like applyMatrixFunction with different
// input and output types for the template parameter

// interpolate between two values that are supposed to wrapping around and always be in xMin..xMax,
// for example -1..1, 0..1, 0..2*pi, -pi..pi, etc. t is the interpolation parameter between 0..1 
// where such that x = (1-t)*x0 + t*x1 = x0 + t*(x1-x0)
template<class T> 
T rsInterpolateWrapped(T x0, T x1, T t, T xMin, T xMax)
{
  T r  = xMax - xMin;  // range
  T du = x1 + r - x0;  // upper difference
  T dm = x1     - x0;  // middle difference
  T dl = x1 - r - x0;  // lower difference
  T au = rsAbs(du);
  T am = rsAbs(dm);
  T al = rsAbs(dl);
  T x;

  // add an appropriate fraction of the delta that has the smallest absolute difference to x0:
  if(au < am && au < al)
    x = x0 + t*du;
  else if(am < al)
    x = x0 + t*dm;
  else
    x = x0 + t*dl;

  // re-wrap result into allowed interval:
  if(x > xMax)
    x -= r;
  else if(x < xMin)
    x += r;
  return x;
}
// move to library and check if it is correct (maybe by a unit test?)...and if it can be simplified
*/

//=================================================================================================

template<class T>
int SinusoidalAnalyzer<T>::getRequiredBlockSize(int type, T df, T fs, bool oddSize) const
{
  T B = rsWindowFunction::getMainLobeWidth(type, 0.0); // later maybe pass a window parameter
  int size = (int) ceil(B*fs/df);
  if(oddSize && rsIsEven(size))   // maybe remove, if linear interpolation is used for the phase
    size += 1;
  return size;
  // Because the phase estimation is much more accurate for odd block sizes (because the phase 
  // spectrum of window functions is flat over the mainlobe for odd block sizes, for even sizes it 
  // has a linear trend), this will always return an odd number so you may use the returned value 
  // as your blockSize without further ado. or: maybe, if linear interpolation is used for phase 
  // estimation, we can get equally good results with even block-sizes? -> try it!
}

template<class T>
T SinusoidalAnalyzer<T>::getRequiredThreshold(int type, T margin) const
{
  return rsWindowFunction::getSideLobeLevel(type, 0.0) + margin;
  // todo: maybe if we constrain the peak-conditions further, such that a peak must be higher than
  // 2,3,4,... of its neighbours to the left and right, we can use lower thresholds without picking 
  // up sidelobes and maybe this number should depend on the mainlobe width of the window 
  // ...hmm...but those peaks will be buried in sidelobes from other partials anyway
}

template<class T>
size_t SinusoidalAnalyzer<T>::findBestMatchingTrack(T freq, 
  std::vector<RAPT::rsSinusoidalPartial<double>>& tracks, 
  const std::vector<bool>& trackContinued) const
{
  T dfMin = RS_INF(T);  
  size_t bestIndex = 0;
  for(size_t i = 0; i < tracks.size(); i++) {
    T trackFreq = tracks[i].getEndFreq();
    T df = rsAbs(freq - trackFreq);
    if(df <= dfMin && trackContinued[i] == false) { // look only at tracks that are not already continued
      dfMin = df;
      bestIndex = i;
    }
  }
  if(dfMin < getMaxFreqDelta(freq))
    return bestIndex;
  else
    return tracks.size();
}

template<class T>
void SinusoidalAnalyzer<T>::continuePartialTracks1(
  std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeaks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& aliveTracks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& deadTracks) const
{
  // initializations:
  typedef std::pair<size_t, size_t> IndexPair;
  std::vector<IndexPair> continuations;               // 1st: activeTracks, 2nd: newPeaks
  std::vector<size_t> deaths;                         // index into activeTracks
  std::vector<size_t> births;                         // index into newPeaks
  size_t pkIdx, trkIdx;                               // peak index, track index
  size_t numActiveTracks = aliveTracks.size();
  std::vector<bool> trackContinued(numActiveTracks);  // true, if track has already been continued
  for(trkIdx = 0; trkIdx < numActiveTracks; trkIdx++)
    trackContinued[trkIdx] = false;

  // Figure out which tracks have to be continued (and if so, which new peak data should be 
  // appended), which tracks have to be discontinued ("death") and for which peaks a new track
  // has to be created ("birth"):

  // loop over the new peaks to figure out birthes and continuations:
  for(pkIdx = 0; pkIdx < newPeaks.size(); pkIdx++) {
    trkIdx = findBestMatchingTrack(newPeaks[pkIdx].freq, aliveTracks, trackContinued); // looks only in those that are not already continued
    if(trkIdx == aliveTracks.size())     // no match found, so a new track is born
      births.push_back(pkIdx);
    else {
      continuations.push_back(IndexPair(trkIdx, pkIdx));
      trackContinued[trkIdx] = true;
    }
  }
  // Actually, it would be better to run the loop over the active tracks and find a match for
  // each track among the new peaks. This would allow to search for a frequency that is not exactly 
  // the last value in the respective track but one that is obtained by extrapolation of the 
  // frequency trajectory so far (linear prediction or polynomial extrapolation). Also, the search 
  // could take advantage of the fact that the peak array is sorted by frequency (to use binary 
  // instead of linear search). This would imply that we would have to work with a boolean 
  // "peakUsed" array instead of "trackContinued". Maybe keep both variants in a prototype 
  // implementation. ...well, actually, it would be possible to use extrapolation with the current
  // loop over the peaks as well - we'd just have to replace
  // T trackFreq = tracks[i].getEndFreq();
  // by
  // T trackFreq = getTrackFreq(tracks[i])
  // where getTrackFreq would be a function that performs the extrapolation - but still the 
  // advantage of binary serach would remain - moreover, looping over the tracks is the way it's
  // described in the literature as well. ..maybe try to cook up a situation where this actually 
  // makes a difference - i think, it makes a difference, if there are two competing possible 
  // matchings - in this function, the one with the lower track-index is chosen, in the other, the 
  // one with the loew peak index is chosen - actually, in this case, the one with the smaller 
  // frequency difference should be chosen - how can we achieve this? maybe first collect all 
  // candidate matchings and if a peak or track index appears twice (or more often), select one?
  // that would let us get rid of the bool-arrays and the difference between the two versions of 
  // the algo disappears, so we may keep only one -> check, how Serra's SMS tools do it

  // loop over the "trackContinued" array to figure out deaths:
  for(trkIdx = 0; trkIdx < numActiveTracks; trkIdx++) {
    if(trackContinued[trkIdx] == false)
      deaths.push_back(trkIdx);
  }

  // we have figured out the deaths, births and continuations - apply them:
  applyContinuations(newPeaks, aliveTracks, deadTracks, births, deaths, continuations);
}

template<class T>
size_t SinusoidalAnalyzer<T>::findBestMatchingPeak(T freq, 
  std::vector<RAPT::rsInstantaneousSineParams<T>>& peaks,
  const std::vector<bool>& peakUsed) const
{
  T dfMin = RS_INF(T);  
  size_t bestIndex = 0;
  for(size_t i = 0; i < peaks.size(); i++) {
    T peakFreq = peaks[i].freq;
    T df = rsAbs(freq - peakFreq);
    if(df <= dfMin && peakUsed[i] == false) { // look only at peaks that are not already used up
      dfMin = df;
      bestIndex = i;
    }
  }
  if(dfMin < getMaxFreqDelta(freq))
    return bestIndex;
  else
    return peaks.size();
}

template<class T>
void SinusoidalAnalyzer<T>::continuePartialTracks0(
  std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeaks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& aliveTracks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& deadTracks) const
{
  // initializations:
  typedef std::pair<size_t, size_t> IndexPair;
  std::vector<IndexPair> continuations;               // 1st: aliveTracks, 2nd: newPeaks
  std::vector<size_t> deaths;                         // index into aliveTracks
  std::vector<size_t> births;                         // index into newPeaks
  size_t pkIdx, trkIdx;                               // peak index, track index
  size_t numNewPeaks = newPeaks.size();
  std::vector<bool> peakUsed(numNewPeaks);            // true, if peak is already used up
  for(pkIdx = 0; pkIdx < numNewPeaks; pkIdx++)
    peakUsed[pkIdx] = false;

  // loop over the alive tracks to figure out deaths and continuations:
  for(trkIdx = 0; trkIdx < aliveTracks.size(); trkIdx++) {
    pkIdx = findBestMatchingPeak(aliveTracks[trkIdx].getEndFreq(), newPeaks, peakUsed); // looks only at peaks that are not already used up
    if(pkIdx == newPeaks.size())        // no match found, so the track dies
      deaths.push_back(trkIdx);
    else {
      continuations.push_back(IndexPair(trkIdx, pkIdx));
      peakUsed[pkIdx] = true;
    }
  }

  // loop over the "peakUsed" array to figure out births:
  for(pkIdx = 0; pkIdx < numNewPeaks; pkIdx++) {
    if(peakUsed[pkIdx] == false)
      births.push_back(pkIdx);
  }

  // we have figured out the deaths, births and continuations - apply them:
  applyContinuations(newPeaks, aliveTracks, deadTracks, births, deaths, continuations);
}

template<class T>
void SinusoidalAnalyzer<T>::applyContinuations(
  std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeaks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& aliveTracks,
  std::vector<RAPT::rsSinusoidalPartial<T>>& deadTracks,
  std::vector<size_t>& births, std::vector<size_t>& deaths,
  std::vector<std::pair<size_t, size_t>>& continuations) const
{
  size_t pkIdx;   // peak index
  size_t trkIdx;  // track index
  int i;          // signed because we use a >= 0 comparison in the deaths loop - 
                  // why actually? - because in the loop, we remove them from the end of the 
                  // aliveTracks array to not mess up the indices inside the loop

  // continue matched tracks with new peaks:
  for(i = 0; i < continuations.size(); i++) {
    trkIdx = continuations[i].first;
    pkIdx  = continuations[i].second;
    aliveTracks[trkIdx].appendDataPoint(newPeaks[pkIdx]);
  }

  // maybe we should assert that deaths is sorted ascending - because when we remove tracks from
  // the aliveTracks array, we must do so from behind in order to not mess up the array indexes
  // inside the loop...

  rsSinusoidalPartial<T> track;
  rsInstantaneousSineParams<T> params;

  // kill discontinued tracks by appending an additonal "fade-out" datapoint at the end of the 
  // track and moving them from the aliveTracks to the deadTracks array:
  if(deaths.size() > 0) {  // can we get rid of this if? i think so
    for(i = (int) deaths.size()-1; i >= 0; i--) {
      trkIdx = deaths[i];
      track  = aliveTracks[trkIdx];
      if(fadeOutTime > T(0))
        track.applyFadeOut(fadeOutTime);
      rsAppend(deadTracks, track);
      rsRemove(aliveTracks, trkIdx); 
    }
  }

  // create new tracks by creating a fresh track from the peaks that should give birth and also 
  // prepend a "fade-in" datapoint:
  for(i = 0; i < births.size(); i++) {
    pkIdx  = births[i];
    params = newPeaks[pkIdx];
    RAPT::rsSinusoidalPartial<T> newTrack;
    newTrack.appendDataPoint(params);
    if(fadeInTime > T(0)) 
      newTrack.applyFadeIn(fadeInTime);
    aliveTracks.push_back(newTrack);
  }
}

template<class T>
rsMatrix<std::complex<T>> SinusoidalAnalyzer<T>::getComplexSpectrogram(
  T* sampleData, int numSamples)
{
  return sp.complexSpectrogram(sampleData, numSamples);
}



void plotDecibels(int N, double* x, double *mag) // for debug-plotting
{
  double* dB = new double[N];
  for(int i = 0; i < N; i++)
    dB[i] = rsAmpToDbWithCheck(mag[i], 0.00000001);
  plotData(N, x, dB);
  delete[] dB;
}

template<class T>
RAPT::rsSinusoidalModel<T> SinusoidalAnalyzer<T>::analyzeSpectrogram(
  const RAPT::rsMatrix<std::complex<T>>& stft, T sampleRate)
{
  // Initializations:
  typedef RAPT::rsInstantaneousSineParams<double> InstParams;
  typedef RAPT::rsSinusoidalPartial<double> Partial;  // rename to SineTrack
  int numFrames  = stft.getNumRows();
  int numBins    = stft.getNumColumns();
  int firstFrame = 0; 
  int lastFrame  = numFrames-1;
  int frameIndex  = firstFrame;
  T binDelta   = sampleRate / sp.getFftSize();
  T frameDelta = sp.getHopSize() / sampleRate;


  rsMatrix<T> mag = matrixMagnitudes(stft);
  rsMatrix<T> phs = matrixPhases(stft);

  // ...maybe plot the spectrogram here...
  //plotPhasogram(numFrames, numBins, phs.getDataPointer(), sampleRate, sp.getHopSize());

  std::vector<Partial> activeTracks, finishedTracks;
  std::vector<InstParams> instPeakParams;

  //bool plotShortTimeSpectra = true;

  // needed for plotting only:
  std::vector<T> freqs(numBins);
  for(int i = 0; i < numBins; i++)
  {
    //freqs[i] = i * sampleRate / sp.getFftSize(); // plot against actual frequency
    freqs[i] = i;  // plot against bin idex
  }

  // loop over the frames:
  while(frameIndex != lastFrame) {

    T time  = frameIndex * frameDelta;
    T* pMag = mag.getRowPointer(frameIndex);
    T* pPhs = phs.getRowPointer(frameIndex);
    std::complex<T>* pCmp = stft.getRowPointer(frameIndex);  // pointer to complex short-time spectrum


    //plotData(numBins, pMag);
    //plotDecibels(numBins, &freqs[0], pMag); // for development
    //plotData(numBins, &freqs[0], pMag); // for development
    //plotData(numBins, &freqs[0], pPhs);

    // find spectral peaks:
    std::vector<int> peaks = peakIndices(pMag, numBins, magThreshold);

    // determine exact peak frequencies, amplitudes and phases:
    instPeakParams.resize(peaks.size());
    for(size_t i = 0; i < peaks.size(); i++) {
      T peakBin, peakAmp;
      spectralMaximumPositionAndValue(pMag, peaks[i], &peakBin, &peakAmp);

      //T peakFreq  = peakBin*sampleRate/sp.getFftSize();

      // maybe later let the user select the phase-interpolation method between no-interpolation, 
      // linear and maybe others (maybe interpolate in the complex domain and then extract phase):
      //T peakPhase = pPhs[peaks[i]];      // maybe use interpolation later
      T peakPhase = RAPT::rsArray::interpolatedValueAt(pPhs, numBins, peakBin);

      instPeakParams[i].time  = time;
      instPeakParams[i].freq  = peakBin*sampleRate/sp.getFftSize(); // use getBinFrequency(peakBin);
      instPeakParams[i].gain  = peakAmp;
      instPeakParams[i].phase = peakPhase;
    }

    // peak continuation, birth or death - todo: dispatch between tracking algorithms:
    continuePartialTracks0(instPeakParams, activeTracks, finishedTracks);
    frameIndex += 1;
  }

  // create and return the model data structure:
  rsSinusoidalModel<T> model;
  model.addPartials(finishedTracks);       // add the finished tracks to the model
  if(fadeOutTime > T(0))                   // "finalize" active tracks by applying fade-out...
    for(size_t i = 0; i < activeTracks.size(); i++)
      activeTracks[i].applyFadeOut(fadeOutTime);
  model.addPartials(activeTracks);         // ...and add them, too
  cleanUpModel(model);                     // remove spurious tracks, merge, etc.
  return model;


  // algorithm:

  // -scan through this spectrogram from left to right
  // -for each frame m, do:
  //  -find the spectral peaks in the frame
  //  -for each peak k in the frame, do:
  //   -figure out its exact frequency, amplitude and phase (and maybe later time, if re-assignment 
  //    is used)
  //   -try to find a match in the set of active partials:
  //    -if a match is found, append the new datapoint to that matched partial
  //    -if no match is found, start a new partial 
  //   -if there are active partials that have not been used up by this matching procedure,
  //    they end now

  // -todo: post-processing: after this is all done, go over the data a second time to see if we 
  //  can merge partials and/or remove partials whose length is below a user-defined threshold

  // -find peak frequency by (parabolic) interpolation (or maybe cubic?)
  // -evaluate complex amplitude at peak-freq
  // -compute magnitude phase from interpolated complex amplitude and store in one of the partials

  // references:
  // http://www.cerlsoundgroup.org/Loris/
  // https://ccrma.stanford.edu/~juan/ATS_manual.html
  // http://clam-project.org/
}

template<class T>
std::vector<int> SinusoidalAnalyzer<T>::peakIndices(T* x, int N, T threshToMax)
{
  T max = RAPT::rsArray::maxValue(x, N);
  std::vector<int> peaks;
  for(int i = 1; i < N-1; i++)
  {
    T dbg = x[i];
    if(x[i] > x[i-1] && x[i] > x[i+1] && x[i] > threshToMax*max)
      peaks.push_back(i);
  }
  return peaks;
} 

template<class T> 
void SinusoidalAnalyzer<T>::spectralMaximumPositionAndValue(T *x, int k, T* pos, T* val)
{
  // find coeffs of parabolic interpolant (maybe factor out, so we can plot the parabola):
  T lowAmp = 0.0000001; // -140 dB - to prevent log-of-zero
  T a[3], y[3];
  y[0] = rsAmpToDbWithCheck(x[k-1], lowAmp);   // left
  y[1] = rsAmpToDbWithCheck(x[k],   lowAmp);   // mid
  y[2] = rsAmpToDbWithCheck(x[k+1], lowAmp);   // right
  rsPolynomial<T>::fitQuadratic_m1_0_1(a, y);

  // find maximum position and evaluate parabola there:
  T d  = rsPolynomial<T>::quadraticExtremumPosition(a);
  *pos = k + d;
  *val = rsDbToAmp(a[0] + (a[1] + a[2]*d)*d);

  // todo: we should safeguard against a[2] == 0 (a degenerate parabola that has no extremum) which
  // will lead to div-by-zero in quadraticExtremumPosition
}

template<class T>
rsSinusoidalModel<T> SinusoidalAnalyzer<T>::analyze(
  T* sampleData, int numSamples, T sampleRate)
{
  // -maybe pre-process the input signal by flattening the pitch and make the period coincide with
  //  a fraction of the analysis frame size 
  // -we should keep the time warping map around and when done with the analysis, use it to 
  //  re-map the time-instants in the model
  // -we should also keep the corresponding (instantaneous) frequency scaler map around and apply
  //  the inverse to the freq data in the model
  //  ...but later - first, let's see how far we get without such a pre-processing and try to make 
  //  the algorithm work well under this (suboptimal) condition, too

  rsMatrix<std::complex<T>> stft = getComplexSpectrogram(sampleData, numSamples);
  return analyzeSpectrogram(stft, sampleRate);
}

template<class T>
void SinusoidalAnalyzer<T>::cleanUpModel(rsSinusoidalModel<T>& model) const
{
  // todo: merge partials (before removing spurious ones)

  // remove spurious partials (maybe factor out):
  for(int i = (int)model.getNumPartials()-1; i >= 0; i--)  
    if(model.getPartial(i).getLength() <= minTrackLength) 
      model.removePartial(i);
  // We use <= instead of < to remove zero-length tracks, even when minTrackLength == 0, so we 
  // allow only for tracks that are *strictly* longer than 0 when minTrackLength == 0.
}

template class SinusoidalAnalyzer<double>;


/*

Ideas:

Peak-Tracking:
-maybe instead of looping over the tracks or peaks and greedily picking the closest match in the 
 respective other array, we should first collect all candidate matches and then decide which gets
 assigned to which based on the whole picture
-maybe a single peak can actually correspond to two tracks (if two tracks cross each other in the
 current frame) and maybe the peak's amplitude should then be split between the two tracks

Window:
-use a Dolph-Chebychev window and let the user only select the amplitude threshold for partials
 -the window will then be automatically tuned such the sidelobes are some reasonable margin below 
  the detection threshold (the margin has to figured out empirically - maybe 15 or 20 dB)
 -this, in turn, determines the mainlobe width of the window which in turn determines the required
  block-size
 ...soo in the high-level setup, the user just selects frequency resolution amplitude threshold 
 (and possibly the margin - but maybe not) and anything else can then be set up automatically
 ...nevertheless, provide the low-level setup anyway
 -maybe the allowed frequency deviation between two frames (which should be proportional to the 
  hopSize) can be set up in a way that is independent from the hopSize - maybe in Hz/s
  10 Hz/s means the frequency is allowed to change by 10 Hz in one second
 -but also have a look at the phase-response - make sure, the window doesn't mess up phase 
  measurement

-


// how do we best compute the instantaneous phase - linear interpolation?
int binInt  = peaks[i];
T   binFrac = peakBin-binInt;
if(binFrac < 0) {
binInt  -= 1;
binFrac  = 1-binFrac;
}
//T amp2 = (1-binFrac)*pMag[binInt] + binFrac*pMag[binInt+1]; // another way to compute amplitude - vary bad
//std::complex<T> binCmpVal = (1-binFrac)*pCmp[binInt] + binFrac*pCmp[binInt+1]; 
//T binMag = abs(binCmpVal);
//T binPhs = arg(binCmpVal);
// complex value at peak-bin
T phs0 = pPhs[binInt];   // just for debug
T phs1 = pPhs[binInt+1]; // dito
peakPhase = rsInterpolateWrapped(pPhs[binInt], pPhs[binInt+1], binFrac, -PI, PI);
// hmm...it doesn't seem to make sense to interpolate the phase between bins like that - it 
// does not seem to be a smooth function - it jumps erratically between bins - maybe we need 
// a completely different way to obtain the instantaneus phase...maybe by comparing values in
// different frames? or maybe by correlating the function with a complex sine that has 
// exactly the frequency? maybe the Goertzel algo can be used?:
// https://en.wikipedia.org/wiki/Goertzel_algorithm
// https://www.embedded.com/design/configurable-systems/4006427/A-DSP-algorithm-for-frequency-analysis
// https://www.st.com/content/ccc/resource/technical/document/design_tip/group0/20/06/95/0b/c3/8d/4a/7b/DM00446805/files/DM00446805.pdf/jcr:content/translations/en.DM00446805.pdf
// https://stackoverflow.com/questions/13499852/scipy-fourier-transform-of-a-few-selected-frequencies
// https://stackoverflow.com/questions/13499852/scipy-fourier-transform-of-a-few-selected-frequencies
// https://stackoverflow.com/questions/11579367/implementation-of-goertzel-algorithm-in-c
//...if we go down that route, we may also use the obtained amplitude
// for refining our amplitude estimate...ok - for now, just leave the instantaneous phase 
// measurement at zero

https://ccrma.stanford.edu/~jos/sasp/Phase_Interpolation_Peak.html


see here for window functions:
https://en.wikipedia.org/wiki/Window_function
implement blackman-harris and maybe blackman-nutall, dolph-chebychev - i think, low sidelobes are
important for sinusoidal parameter estimation...maybe gaussian for frequency estimation? or use 
re-assignment?
*/
