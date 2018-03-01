#ifndef rosic_Snowflake_h
#define rosic_Snowflake_h

namespace rosic
{

/** Snowflake is a wavetable oscillator that uses stereo waveforms generated by Lindenmayer systems. 
When left and right channel are interpreted as x,y coordinates and plotted, the resulting curves 
show a self-similar character. The Koch snowflake is one simple example of such a curve. 

todo factor out a TurtleSynthesizer class - most of this class is generally about turtle-synthesis

References:
PGCA: Pattern Generation for Computational Art (Stefan and Richard Hollos)
LSFP: Lindenmayer Systems, Fractals and Plants (Prusinkiewicz, Hanan)
ABoP: The Algorithmic Beauty of Plants (Prusinkiewicz, Lindenmayer)  */

class Snowflake
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

  Snowflake();

  //-----------------------------------------------------------------------------------------------
  // \name Setup

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
  it is in free running mode. A free running turtle makes the picture rotate. A value of 1, i.e.
  resetting after every cycle, is equivalent to wavetable mode. */
  void setResetAfterCycles(int numCycles);

  /** Sets the method that is used to interpolate between the sequence of points that the turtle 
  generates. */
  void setInterpolationMode(int newMode) { interpolation = newMode; }

  /** Selects whether or not a precomputed (wave) table should be used or samples should be 
  computed from the turtle commands on the fly. The behavior is a bit different in both cases.
  Using a table, the 'f' turtle command does not behave properly and turn angle modulation is
  prohibitively expensive. On the other hand, the table-based implementation allows fo stereo
  detuning and should be generally more efficient (verify this). */
  void setUseTable(bool shouldUseTable) { useTable = shouldUseTable; }

  //void setStereoDetune(double newDetune);
  //void setStereoFrequencyOffset(double newOffset);
  // these are relevant only in table-mode - maybe use the regular wavetable oscillator

  /** Sets the number of iterations for the L-system. */
  void setNumIterations(int newNumIterations);

  /** Sets the turning angle for the turtle-graphics interpreter. */
  void setAngle(double newAngle);

  /** Clears the set of L-system rules. */
  void clearRules();

  /** Adds an L-system rule. */
  void addRule(char input, const std::string& output);

  /** Sets the seed (aka "axiom") for the L-system. */
  void setAxiom(const std::string& newAxiom);

  /** Updates all internal variables, so they reflect the user settings. Client code normally 
  doesn't have to care, because it does this automatically, when necessary. But sometimes it's
  useful for debugging to call this manually. */
  void updateAllInternals();

  //-----------------------------------------------------------------------------------------------
  // \name Processing

  /** Calculates one output-sample frame at a time. */
  INLINE void getSampleFrameStereo(double *outL, double *outR)
  {
    // some checks (optimize - have a single readyToPlay flag so we only need one check here):
    if(!commandsReady)              updateTurtleCommands();
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

    // increment and wraparound:
    pos += inc;
    while(pos >= numLines)
      pos -= numLines;
  }

  /** Resets the state of the object, such that we start at 0,0 and head towards 1,0 (in 
  unnormalized coordinates). */
  void reset();


protected:

  //-----------------------------------------------------------------------------------------------
  // \name Misc

  void interpolate(double *left, double *right, double frac)
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


  void resetTurtle();

  /** Makes the buffers x[0],x[1],y[0],y[1] etc. reflect the line endpoints of the given index for
  the target line. */
  void goToLineSegment(int targetLineIndex);

  /** Goes form the current line segment (defined by member lineIndex) to the next, possibly 
  including a wraparound. */
  void goToNextLineSegment();

  void updateXY();

  /** Renders the wavetable and updates related variables. */
  void updateWaveTable();

  /** Updates our string of turtle-graphics drawing commands. */
  void updateTurtleCommands();

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
  int numLines = 0;               // number of 'F's in turtleCommands
  int lineIndex = 0;              // index of current line
  int commandIndex = 0;           // index in the list of turtle-commands
  int cycleCount = 0;
  double x[2], y[2];              // x[0]: point we come from, x[1]: point we go to, maybe apply a DC blocking filter to these x,y states
  std::string lindenmayerResult;  // output string of Lindenmayer system
  std::string turtleCommands;     // only the turtle commands from lindenmayerResult 

  // parameters:
  double amplitude     = 1;
  double frequency     = 0;
  double sampleRate    = 1;
  double turnAngle     = 0;
  int    cyclicReset   = 1;
  int    interpolation = LINEAR;

  // rendering objects and related variables:
  LindenmayerSystem lindSys;
  TurtleGraphics turtle;
  RAPT::rsRotationXY<double> rotator; // for rotating final x,y coordinates
  int numIterations = 0; // replace by iteratorString or applicatorString (a string like AAABBAC)
  std::string axiom;

  // for optional table-based synthesis (maybe at some point we can drop that?)
  std::vector<double> tableX, tableY;  // rendered (wave)tables for x (left) and y (right)
  bool useTable = false;

  // flags to indicate whether or not various rendering state variables are up to date:
  std::atomic_bool commandsReady = false; // flag to indicate that "turtleCommands" is up to date
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