#ifndef rosic_TurtleSource_h
#define rosic_TurtleSource_h

namespace rosic
{

/** A class for counting up to some upper limit and then reset to zero. Meant to be used in a real 
time turtle graphics driver to reset the turtle's state periodically. */

class ResetCounter
{

public:

  ResetCounter();

  /** Sets the reset interval for the counter. The interval does not have to be an integer. If it 
  has a fractional part, this class will internally manage to appropriately alternate between the 
  floor and ceiling of the desired interval so as to give an average reset interval equal to the 
  desired value. */
  void setInterval(double newInterval);

  /** Counts one unit up for the given counter and possibly resets it to zero, when the limit is 
  reached. The return value informs, whether or nor a rest occurred. */
  bool tick();

  void setParametersZero();

  void setStateZero();

protected:

  void updateJitteringLimit();

  // counter parameter variables:
  //double interval;   // time interval after which to periodically reset ...actually redundant -> get rid
  int    intPart;    // integer part of interval
  double fracPart;   // fractional part of interval

  // counter state variables:
  double errAccu;        // accumulated fractional error
  int    jitteringLimit; // alternates between floor and ceil of interval
  int    counter;        // the actual counter

};


//=================================================================================================

/** TurtleSource is a sound generator based on the turtle graphics concept. It interprets the 
sequence of x,y points generated by the turtle as outputs for the left and right stereo channel. */

class TurtleSource
{

public:

  /** Enumeration of different the ways that can be used to interpolate between the sequence of 
  points that the turtle generates. */
  enum interpolationModes
  {
    LEFT_NEIGHBOUR,
    RIGHT_NEIGHBOUR,
    NEAREST_NEIGHBOUR,
    LINEAR,
    CUBIC,               // cubic hermite - matches slopes at datapoints
    QUARTIC              // like cubic and normalizes integral to be the same as in linear
  };

  TurtleSource();

  //-----------------------------------------------------------------------------------------------
  // \name Setup

  /** Sets the string of turtle graphics commands to be used to produce ("draw") the output 
  signal. */
  void setTurtleCommands(const std::string& commands);

  /** Sets the sample-rate. */
  void setSampleRate(double newSampleRate);

  /** Sets the frequency (in Hz) of the signal to be generated. */
  void setFrequency(double newFrequency);

  /** Sets the amplitude (as raw factor). */
  void setAmplitude(double newAmplitude) { amplitude = newAmplitude; }

  /** Sets a rotation angle (in degrees) to be applied to the produced xy coordinate pair. */
  void setRotation(double newRotation);

  /** Sets the number of cycles, the turtle has to run trough its command-string before it 
  will be reset into its initial state. When 0 is passed, the turtle will not reset at all, i.e.
  it is in free running mode. A free running turtle can make a rotating picture, if the turtle 
  ends up in the (almost) the same position after one cycle but heads toward a sligtly different 
  direction tha when starting the cycle.
  
  todo: let the user set up a number of lines after which to reset - that number may or may not a 
  multiple of the number of lines in the drawing (if it is, it corresponds to resetting after a 
  number of cycles). */
  void setResetAfterCycles(int numCycles);

  /** Sets the number of lines that the turtle generates before it is reset into its initial state.
  This resetting works in addition to the one effected by setResetAfterCycles. The number of lines
  given here does not need to be an integer. If it's 100.5, the actual counter will alternately 
  count up to 100 and 101, if it's 100.25 it will count to 100 3 times, then once to 101 and so 
  on. */
  void setResetAfterLines(double numLines);


  void setResetRatio1( double newRatio)  { setResetRatio( 0, newRatio);  }
  void setResetOffset1(double newOffset) { setResetOffset(0, newOffset); }

  void setResetRatio2( double newRatio)  { setResetRatio( 1, newRatio);  }
  void setResetOffset2(double newOffset) { setResetOffset(1, newOffset); }

  // move to protected:
  void setResetRatio( int index, double newRatio);
  void setResetOffset(int index, double newValue);

  // under construction - to compute resetInterval = numLines * (param1 + param2/inc):
  void setResetRatio(double newRatio);               // param1
  void setResetRatioOffsetOverInc(double newValue);  // param2 - find better name
  // replace by an array of independent resetters, maybe factor resetting out into a class 
  // ResetManager





  /** Sets the method that is used to interpolate between the sequence of points that the turtle 
  generates. */
  void setInterpolationMode(int newMode) { interpolation = newMode; }

  /** Selects whether or not a precomputed (wave) table should be used or samples should be 
  computed from the turtle commands on the fly. The behavior is a bit different in both cases.
  Using a table, the 'f' turtle command does not behave properly and turn angle modulation is
  prohibitively expensive. On the other hand, the table-based implementation allows fo stereo
  detuning and should be generally more efficient (verify this). */
  void setUseTable(bool shouldUseTable) { useTable = shouldUseTable; }

  /** Experimental feature.. */
  void setAntiAlias(bool shouldAntiAlias) { antiAlias = shouldAntiAlias; }

  //void setStereoDetune(double newDetune);
  //void setStereoFrequencyOffset(double newOffset);
  // these are relevant only in table-mode - maybe use the regular wavetable oscillator

  /** Sets the turning angle for the turtle-graphics interpreter. */
  void setAngle(double newAngle);

  //-----------------------------------------------------------------------------------------------
  // \name Inquiry

  /** Returns the number of lines that the trutel would produce according to the current command 
  string. */
  virtual int getNumTurtleLines();

  /** Returns the number of lines produced since the last reset of lineCount. (mostly for figuring
  out resetting behavior) */
  int getLineCount() const { return lineCount; }

  //-----------------------------------------------------------------------------------------------
  // \name Processing

  /** Calculates one output-sample frame at a time. */
  INLINE void getSampleFrameStereo(double *outL, double *outR)
  {
    // some checks (optimize - have a single readyToPlay flag so we only need one check here):
    if(numLines < 1)                return;
    if(!tableUpToDate && useTable)  updateWaveTable();
    if(!incUpToDate)                updateIncrement();

    // integer and fractional part of position:
    int iPos = floorInt(pos);
    double fPos = pos - iPos;
    if(iPos != lineIndex)
      goToLineSegment(iPos);

    // read out buffered line segment (x[], y[] members) with interpolation:
    interpolate(outL, outR, fPos);
    *outL = normalizer * amplitude * (*outL - meanX);
    *outR = normalizer * amplitude * (*outR - meanY);
    rotator.apply(outL, outR);

    updatePosition();
  }

  /** Resets the state of the object, such that we start at 0,0 and head towards 1,0 (in 
  unnormalized coordinates). */
  void reset();


protected:

  //-----------------------------------------------------------------------------------------------
  // \name Misc

  /** Updates our readout position variable by incrementing it and taking care of the required 
  wraparound(s). */
  INLINE void updatePosition()
  {
    // increment and wraparound:
    pos += inc;
    while(pos >= numLines) pos -= numLines;
    //while(pos <  0)        pos += numLines; // uncomment later, when we allow negative inc
  }

  /** Reads out our line-segment buffer with interpolation and assigns the output for left and 
  right channel accordingly. */
  INLINE void interpolate(double *left, double *right, double frac)
  {
    switch(interpolation)
    {
    case LEFT_NEIGHBOUR: {
      *left  = x[0];
      *right = y[0]; } break;
    case RIGHT_NEIGHBOUR: {
      *left  = x[1];
      *right = y[1]; } break;
    case NEAREST_NEIGHBOUR: {
      if(frac >= 0.5) { *left = x[0]; *right = y[0]; }
      else            { *left = x[1]; *right = y[1]; } } break;
    case LINEAR: {
      *left  = (1-frac)*x[0] + frac*x[1];
      *right = (1-frac)*y[0] + frac*y[1]; } break;
    default: {
      *left  = 0;
      *right = 0;
      rsAssertFalse; } // unknown interpolation setting 
    }
    // or maybe call the 1st three floor, ceil, round, ..at least on the GUI
  }

  /** Resets only the turtle into its initial state, leaving the other state variables as is. Used
  internally in reset (which resets everything)) and for resetting after a number of lines has been
  rendered. */
  void resetTurtle();

  /** Makes the buffers x[0],x[1],y[0],y[1] etc. reflect the line endpoints of the given index for
  the target line.  */
  void goToLineSegment(int targetLineIndex);

  /** Goes form the current line segment (defined by member lineIndex) to the next, possibly 
  including a wraparound. */
  void goToNextLineSegment();

  void updateXY();

  /** Renders the wavetable and updates related variables. */
  void updateWaveTable();

  /** Updates internal resetter variables according to the resetter user parameters, numLines, 
  frequency (and sampleRate?) */
  void updateResetterVariables();

  /** Updates the wavetable increment according to desired frequency, sample rate and wavetable 
  length. */
  void updateIncrement();
   
  /** Updates our meanX, meanY, normalizer members according to the current turtleCommands and 
  angle settings. */
  void updateMeanAndNormalizer();

  //-----------------------------------------------------------------------------------------------
  // \name Data

  // state for on-the fly rendering:
  double pos = 0;                 // position in wavetable / string
  double inc = 0;                 // wavetable increment
  double meanX = 0, meanY = 0;    // mean values of x,y coordinates in one cycle
  double normalizer = 1;          // scales outputs such that -1 <= x,y <= +1 for all points
  int numLines     = 0;           // number of 'F's in turtleCommands
  int lineIndex    = 0;           // index of current line
  int commandIndex = 0;           // index in the list of turtle-commands
  int cycleCount   = 0;           // counter for generated runs through the curve
  int lineCount    = 0;           // counter for generated lines
  double x[2], y[2];              // x[0]: point we come from, x[1]: point we go to, maybe apply a DC blocking filter to these x,y states
  std::string turtleCommands;     // drawing commands for the turtle

  // parameters:
  double amplitude      = 1;
  double frequency      = 0;
  double sampleRate     = 1;
  double turnAngle      = 0;
  int    cyclicReset    = 1;
  int    interpolation  = LINEAR;
  bool   antiAlias      = false;
  bool   useTable       = false;



  // new resetter stuff:
  static const int numResetters = 2;
  double resetRatios[numResetters];
  double resetOffsets[numResetters];
  ResetCounter resetCounters[numResetters];


  // user parameters for reset:
  double resetRatio  = 1;
  double resetOffset = 0;  // this one gets frequency scaled

  // internal parameter for reset:
  double lineCountReset      = 0;  // number of lines aftre which to reset
  double lineCountResetFrac  = 0;  // fractional part
  double lineCountResetErr   = 0;  // accumulates fractional error
  int    lineCountResetFloor = 0;
  int    lineCountResetAlt   = 0;  // alternates between floor and ceil







  // rendering objects and related variables:
  TurtleGraphics turtle;
  RAPT::rsRotationXY<double> rotator; // for rotating final x,y coordinates


  // for optional table-based synthesis (maybe at some point we can drop that?)
  std::vector<double> tableX, tableY;  // rendered (wave)tables for x (left) and y (right)

  rsEngineersFilterStereo turtleLowpass; 
    // lowpass applied to turtle output for anti-aliasing ...check, if the filter coeffs have the
    // correct limit, i.e. go into bypass mode when cutoff == fs/2


  // flags to indicate whether or not various rendering state variables are up to date:

  std::atomic_bool tableUpToDate = false; // maybe rename to tableReady ...get rid
  std::atomic_bool incUpToDate = false;
  // this is a new way of dealing with updating internal variables - it avoids redundant 
  // recalculations when sereval parameters change at once and allows the re-calculation to be
  // done in a thread-safe way in the audio thread - the gui thread just atomically sets these
  // flags...maybe if we do it everywhere like this, we can even get rid of locking in 
  // jura::Parameter...that would be great!!

};

}

#endif