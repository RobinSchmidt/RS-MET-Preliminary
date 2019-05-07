template<class T, class TOsc, class TBlep>
rsSuperBlepOsc<T, TOsc, TBlep>::rsSuperBlepOsc()
{
  setMaxNumOscillators(8);
}

// move to rsArray:
template<class T>
void affineTrafo(const T* x, T* y, int N, T a, T b)
{
  for(int i = 0; i < N; i++)
    y[i] = a * x[i] + b;
}
template<class T>
void transformRange(const T* x, T* y, int N, T targetMin, T targetMax)
{
  T currentMin = rsArray::minValue(x, N);
  T currentMax = rsArray::maxValue(x, N);
  T a = (targetMin - targetMax) / (currentMin - currentMax);
  T b = (currentMax*targetMin - currentMin*targetMax) / (currentMax - currentMin);
  affineTrafo(x, y, N, a, b);
}

template<class T, class TOsc, class TBlep>
void rsSuperBlepOsc<T, TOsc, TBlep>::updateIncrements()
{
  // this is the place, where we should experiment with different strategies to compute the 
  // ratios ...maybe have functions like getMetallicRatios, getIntegerSquareRoots, etc., call them
  // here and after that is done, 

  // compute ratios of the increments:
  //rsArray::fillWithValue(&incs[0], numOscs, T(1)); // preliminary

  typedef rsRatioGenerator<T> RG;

  if(numOscs == 1)
    incs[0] = 1;
  else
  {
    // factor out into fillIncrementRatios:

    // linear progression of increments - this produces really bad beating:
    T minRatio = T(1) - T(0.5) * detune;
    T maxRatio = T(1) + T(0.5) * detune;
    T ratioInc = (maxRatio-minRatio) / T(numOscs-1);
    for(int i = 0; i < numOscs; i++)
    {
      incs[i] = minRatio + i * ratioInc;

      // test:
      //incs[i] = rsMetallicRatio(T(i+1));
      incs[i] = RG::metallic(T(i));
      //incs[i] = sqrt( T(i+1) );

    }


  
    transformRange(&incs[0], &incs[0], numOscs, minRatio, maxRatio);
    // actually, if we know that the min is in incs[0] and the max is in incs[numOscs-1], we don't
    // need to search for min/max -> optimize
    // -> factor out a rangeConverversionCoeffs function from transformRange that computes a, b
    // from given x1,x2,y1,y2

    int dummy = 0;
  }




  // maybe generate "prototype" ratios and then apply a linear transformations that transforms the 
  // range from the prototype min/max to the target min/max

  //// translate from inc-ratios to freq-ratios (todo: make user-option):
  //for(int i = 0; i < numOscs; i++)
  //  incs[i] = T(1) / incs[i];
  //rsArray::reverse(&incs[0], numOscs); 




  // todo: other options: linear progression of frequencies (increment reciprocals), geometric
  // spacing...hmm - can we somehow "invert" the generalized mean formula to obtain a generalized
  // spreding function - we can do it for arithmetic mean, harmonic mean and geometric mean - but
  // what should be in between?
  // ...

  //




  // see rosic::SuperOscillator::setFreq for other spacing strategies - factor out the code from 
  // there to make it usable here too





  

  // todo: maybe compute the mean ratio and use it to center the mean frequency/increment - but 
  // what mean should we use? probably the arithmetic, but the geometric or harmonic seems 
  // plausible as well - maybe make it a user option (use a generalized mean and let the user set 
  // the exponent). actually i think, the harmonic mean of the increments (corresponding to the
  // arithmetic mean of the frequencies) makes more sense - experimentation needed....

  // multiply in the reference increment:
  for(int i = 0; i < numOscs; i++)
    incs[i] *= refInc;
  // ...hmm - or maybe it would be better to do thi in getSample and therefore avoid updating the
  // ratios on freq-changes?
}




template<class T, class TBlep>
void rsSyncOsc<T, TBlep>::reset()
{
  master.reset();
  slave.reset();
  blep.reset();
}



template<class T, class TBlep>
void rsDualBlepOsc<T, TBlep>::reset()
{
  osc1.reset();
  osc2.reset();
  blep1.reset();
  blep2.reset();
}