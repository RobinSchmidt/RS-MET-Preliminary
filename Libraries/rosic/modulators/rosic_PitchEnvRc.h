#ifndef rosic_PitchEnvRc_h
#define rosic_PitchEnvRc_h

// rosic-indcludes:
#include "../math/rosic_ElementaryFunctionsReal.h"

namespace rosic
{

 /**

 This is a class which generates an exponential envelope with adjustable 
 start-, attack-, peak-, hold-,  decay-, sustain-, release- and 
 end-values. It is based on feeding a stairstep-like input signal into
 a RC-filter unit. The filter input signal is switched to a new value
 according to the time and level values, at the same time the filter is
 switched to it's new time constant. This also implies, that the level-value 
 will not really be reached (in theory) but only approached asymptotically.
 So the time values are not really the time between the levels, but rather
 time constants tau of the RC unit. The time constant tau is defined as the
 time until the filter reaches 63.2% of the end value (for an incoming
 step-function). This time constant can be scaled to re-define the ramp time
 to other values than 63.2%.
 This class is particularly made for frequency envelopes - it accepts the
 level values in semitones and returns a value which can be used to be 
 multiplied with some frequency.

 */

 class PitchEnvRc
 {

 public:

  //---------------------------------------------------------------------------
  // construction/destruction:

  PitchEnvRc();   ///< Constructor.
  ~PitchEnvRc();  ///< Destructor.

  //---------------------------------------------------------------------------
  // parameter settings:

  void setSampleRate(double newSampleRate);  
  ///< Overrides the setSampleRate() method of the AudioModule base class.

  void setStart(double newStartValue);     
  ///< This sets the point where the envelope starts (in dB).

  void setAttack(double newAttackTime);    
  ///< Sets the length of attack phase (in seconds).

  void setPeak(double newPeakValue);      
  ///< Sets the highest point of the envelope (in dB).

  void setHold(double newHoldTime);      
  ///< Sets the hold time (in seconds).

  void setDecay(double newDecayTime);     
  ///< Sets the length of decay phase (in seconds).

  void setSustain(double newSustainValue);   
  ///< Sets the sustain level (in dB).

  void setRelease(double newReleaseTime);   
  ///< Sets the length of release phase (in seconds).

  void setEnd(double newEndValue);       
  ///< Sets the end point of the envelope (in dB).

  void setTimeScale(double newTimeScale); 
  /**< Scales the A,D,H and R times by adjusting the increment.
       It is 1 if not used - a timescale of 2 means the envelope is twice
       as fast, 0.5 means half as fast -> useful for implementing a 
       key/velocity-tracking feature for the overall length for 
       the envelope. */

  void setTauScale  (double newTauScale);  
  /**< Scales the time constants tau. 
       Can be used to reach other values than 63.2% in the 
       specified time values */

  void setPeakScale(double newPeakScale); 
  /**< Scales the peak-value of the envelope - useful for velocity response. */

  //triggering:
  void trigger();  
  ///< Causes the envelope to start with its attack-phase.

  void noteOff();  
  ///< Causes the envelope to start with its release-phase.

  void reset();    
  ///< Resets the time variable.

  //---------------------------------------------------------------------------
  //audio processing:

  INLINE double getSample();    
  ///< Calculates one output sample at a time.

  //---------------------------------------------------------------------------
  //inquiry:

  //bool endIsReached();  
  ///< True, if output is below 40 dB.

  // flags to be accessed from an outlying class:
  //bool outputIsZero;   
  ///< Flag to indicate, if amplitude envelope has reached its end.

 protected:

  // amplitude values for the stairstep input signal
  // (as amplitude values - not in dB):
  doubleA startFreq, peakFreq, sustainFreq, endFreq;   
  
  // time values in seconds:
  doubleA attackTime, holdTime, decayTime, releaseTime;

  // accumulated time values:
  doubleA attPlusHld,
          attPlusHldPlusDec,
          attPlusHldPlusDecPlusRel;

  // time reference variables:
  doubleA time;       // this variable represents the time since the last call
                      // to Trigger() (the unit is seconds)
  doubleA timeScale;  // scale the time constants in the filters according to
                      // increment
  doubleA increment;  // increment for the time variable per sample (usually
                      // 1/sampleRate but can be modified for changing the 
                      // overall duration of the envelope), also influences the
                      // filter coefficients
  doubleA tauScale;   // scale factor for the time constants of the filters
  doubleA peakScale;  // scale factor for the peak-value

  // filter coefficients - the inputCoeffs scale the current input sample
  // whereas the recursionCoeffs scale the previous output sample - which pair
  // of coefficients will be used is determined by the state of the envelope 
  // (which is given by the time variable and note on/off status):
  doubleA inputCoeffAttack,  recursionCoeffAttack,
          inputCoeffDecay,   recursionCoeffDecay,   
          inputCoeffRelease, recursionCoeffRelease;

  // buffering:
  doubleA previousOutput;  // holds the previous output sample

  doubleA sampleRate; 
  doubleA sampleRateRec;   // reciprocal of the sampleRate

  bool    noteIsOn;   // indicates if note is on, if not, the envelope starts 
                      // with its release phase
 };

 //----------------------------------------------------------------------------
 // from here: definitions of the functions to be inlined, i.e. all functions
 // which are supposed to be called at audio-rate (they can't be put into
 // the .cpp file):
 INLINE double PitchEnvRc::getSample()
 {
  static doubleA outSamp;

  // envelope is in attack or hold phase:
  if(time <= attPlusHld)   // noteIsOn has not to be checked, because, time is
                           // advanced to the beginning of the release phase
                           // in noteOff()
  {
   outSamp =   inputCoeffAttack*peakScale*peakFreq 
             + recursionCoeffAttack*previousOutput
             + TINY;
   time   += increment;
  }

  // envelope is in decay phase:
  else if(time <= (attPlusHldPlusDec)) // noteIsOn has not to be checked
  {
   outSamp =   inputCoeffDecay*sustainFreq 
             + recursionCoeffDecay*previousOutput
             + TINY;
   time   += increment;
  }

  // envelope is in sustain phase:
  else if(noteIsOn)
   outSamp =   inputCoeffDecay*sustainFreq 
             + recursionCoeffDecay*previousOutput
             + TINY;
   // time is not incremented in sustain

  // envelope is in release phase:
  else
  {
   outSamp =   inputCoeffRelease*endFreq 
             + recursionCoeffRelease*previousOutput
             + TINY;
   time   += increment;
  }

  // store output sample for next call:
  previousOutput = outSamp;

  return outSamp;
 }

} // end namespace rosic

#endif // rosic_PitchEnvRc_h
