#pragma once


/** A class to generate sounds based on the heat equation. We consider a one-dimensional medium 
(like a thin rod) that has some initial heat distribution which evolves over time according to the
heat equation. The sound generation is just based on reading out this distribution and interpreting 
it as a cycle of a waveform. At each time-step of the PDE solver, we will get another (simpler) 
shape of the cycle than we had the previous cycle - so time-steps of the PDE solver occur after 
each cycle. The time evolution of the heat equation is such that the distribution becomes simpler 
and simpler over time and eventually settles to a uniform distribution at the mean value of the
intial distribution (which we may conveniently choose to be zero). */

template<class T>
class rsHeatEquation1D
{

public:

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Sets the maximum length of the cycle in samples. The cycle length is equal to the number of 
  rod-elements in our heat-equation simulation. */
  void setMaxCycleLength(int newLength);

  /** Sets up an (initial) heat distribution for the rod. */
  void setHeatDistribution(T* newDistribution, int rodLength);

  void setRandomHeatDistribution(int seed, int rodLength);

  /** Normalizes the current heat distribution in the rod to have the given mean and variance. 
  Note that the default mean-value of zero implies that we may have unphysical negative heat 
  values - but that's okay because we want to generate audio signals and don't care about the 
  physical interpretation. You may may want to call this right after calling setHeatDistribution
  in order to remove any DC in your initial distribution and/or in order to normalize the signal
  loudness. */
  void normalizeHeatDistribution(T targetMean = T(0), T targetVariance = T(1));
  // not yet complete

  /** Sets the coefficient by which the heat diffusses. The higher the value, the faster the 
  initial heat distributions evens out over the length of the rod - hence, the faster the 
  complexity and amplitude of the waveform decays. */
  void setDiffusionCoefficient(T newCoeff) { diffusionCoeff = newCoeff; }

  //-----------------------------------------------------------------------------------------------
  /** \name Processing */

  /** Produces one sample at a time and also updates the rod-element corresponding to the 
  currently produced sample. By spreading the rod-update over time in this way, we avoid spikes in 
  CPU load. */
  T getSample()
  {
    //return T(0);  // preliminary

    T out = rodIn[sampleCount];

    // update of a single rod-element:
    T neighborhood;
    if(sampleCount == 0)
      neighborhood = rodIn[sampleCount+1];
    else if(sampleCount == rodLength-1)      // maybe have rodLength-1 as member
      neighborhood = rodIn[sampleCount-1];
    else
      neighborhood = T(0.5) * ( rodIn[sampleCount-1] + rodIn[sampleCount+1] );
    T delta = neighborhood - out;
    rodOut[sampleCount] = rodIn[sampleCount] + diffusionCoeff * delta;
    // Maybe factor out and/or have other ways of handling the ends (for example cyclically, 
    // clamped, etc.). Currently, we handle the ends by just leaving out the mean-computation and 
    // just adjusting the end-element in the direction to its single neighbor element


    // update counter and return output:
    sampleCount++;
    if(sampleCount == rodLength) {
      sampleCount = 0;
      rsSwap(rodIn, rodOut);
    }
    return out;
  }

  void reset() {
    sampleCount = 0;
    rodIn  = &rodArray1[0];
    rodOut = &rodArray2[0];
  }

protected:

  std::vector<T> rodArray1, rodArray2;
  // array of heat-values in the rod - we need two arrays to alternate between in the evolution
  // of the heat equation

  int rodLength;       // number of (currently used) rod elements

  int sampleCount = 0;

  T diffusionCoeff = 0;


  T *rodIn, *rodOut;
  // Pointers that alternately point to the beginning of rodArray1[0] and rodArray2[0] - used for
  // computing the new heat-distribution from the old


};