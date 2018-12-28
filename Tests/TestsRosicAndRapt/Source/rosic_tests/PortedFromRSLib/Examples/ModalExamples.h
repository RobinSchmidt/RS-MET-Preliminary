#ifndef RS_MODALEXAMPLES_H
#define RS_MODALEXAMPLES_H

#include <iostream>
#include "SampleMapGenerator.h"

void createModalFilterExamples();
void createModalFilterBankExamples();

// single sample creations:
void createPiano1();

// sample-map creations:
void createBass1();
void createGong1();
void createPluck1();



/** A class to generate the low-level parameters for each mode from a bunch of higher level macro
parameters. */

template<class T>
class rsModalParameterGenerator
{

public:


  rsModalParameterGenerator();

  rsModalBankParameters<T> getModalParameters();



  /** \name Frequency parameters */

  void setSampleRate(T newSampleRate) { sampleRate = newSampleRate; }

  void setReferenceFrequency(T newFrequency) { frequency = newFrequency; }

  void setInharmonicity(T newInharmonicity) { inharmonicity = newInharmonicity; }

  // void setFrequencyRatios
  // harmonic/12-tet/idealBar/etc...




  /** \name Amplitude parameters */

  void setAmplitude(T newAmplitude) { amplitude = newAmplitude; }

  void setLowpassSlope(T newSlope) { lowpassSlope = newSlope; }

  void setLowpassCutoff(T newCutoff) { lowpassCutoff = newCutoff; }

  void setEvenAmpScale(T newScaler) { evenAmpScale = newScale; }

  void setAmpCombHarmonic(T newHarmonic) { ampCombHarmonic = newHarmonic; }

  void setAmpCombAmount(T newAmount) { ampCombAmount = newAmount; }


  /** \name Phase parameters */




  /** \name Envelope parameters */

  void setReferenceAttack(T newAttack) { attackTime = newAttack; }

  void setReferenceDecay(T newDecay) { decayTime = newDecay; }

  void setEvenDecayScale(T newScale) { evenDecayScale = newScale; }

  void setDecayCombHarmonic(T newHarmonic) { decayCombHarmonic = newHarmonic; }

  void setDecayCombAmount(T newAmount) { decayCombAmount = newAmount; }



  /** \name Static function */


  /** Gives the relative mode decay time for mode with relative frequency f given a (relative)
  cutoff frequency fc and an ultimate slope of the decay-time function (with respect to f)
  determined by the exponent p
  d(f) = a / (b + (f/fc)^p) where a and b are adjusted such that d(f=1)=1 and d(f=fc)=1/2
  fc must be > 1 */
  static T modeDecayTime(T f, T fc, T p);
  // rename and use also for amplitude


protected:

  void getFrequencies(std::vector<T>& f); 
  // maybe rename to getRelativeFrequencies or getFrequencyRatios, also assigns numPartials so it 
  // should be always called before getPhases

  void getPhases(std::vector<T>& p, const std::vector<T>& f);

  void getAmplitudes(std::vector<T>& a, const std::vector<T>& f);


  void getDecayTimes(std::vector<T>& d, const std::vector<T>& f);

  void getAttackTimes(std::vector<T>& a, const std::vector<T>& f, const std::vector<T>& d);












  // data:

  // frequency related:
  T sampleRate    = 44100;
  T frequency     = 100;
  T inharmonicity = 0.0;
  //int partialRatios  = HARMONIC;

  // amplitude related:
  T amplitude       = 1.0;
  T lowpassSlope    = 0.0;    // in dB/oct..or maybe as direct exponent
  T lowpassCutoff   = 1;      // as harmonic number
  T evenAmpScale    = 1.0;    // amplitude scaler for even harmonics
  T ampCombHarmonic = 7.0;    // harmonic number of 1st notch
  T ampCombAmount   = 0.0;

  // phase related:
  int phaseRandomSeed  = 0;
  int phaseRandomShape = 0;  // shape of the distribution: 0: uniform, 1: triangular, 2: parabolic
  T phaseRandomness    = 0;

  // envelope related:
  T decayTime         = 1.0;
  T decayCutoff       = 1.0;
  T decaySlope        = 0.0;
  T evenDecayScale    = 1.0;    // decay-time scaler for even harmonics
  T decayCombHarmonic = 7.0;
  T decayCombAmount   = 0.0;

  T attackTime        = 0.1;
  T attackDecayRatioLimit = 0.99;

  int maxNumPartials = 1024;
  //int numPartials    = 0;



  std::vector<T> tmp; // for temporary values
  RAPT::rsNoiseGenerator2<T> prng;
};


// move somewhere else:
void createInsertionSortSound();


#endif
