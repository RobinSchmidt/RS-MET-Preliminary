#ifndef rosic_Snowflake_h
#define rosic_Snowflake_h

namespace rosic
{

/** Snowflake is a wavetable oscillator that uses stereo waveforms generated by Lindenmayer systems. 
When left and right channel are interpreted as x,y coordinates and plotted, the resulting curves 
show a self-similar character. The Koch snowflake is one simple example of such a curve. */

class Snowflake
{

public:

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

  /** Selects whether or not a precomputed (wave) table should be used or samples should be 
  computed from the turtle commands on the fly. The behavior is a bit different in both cases.
  Using a table, the 'f' turtle command does not behave properly and turn angle modulation is
  prohibitively expensive. On the other hand, the table-based implementation allows fo stereo
  detuning and should be generally more efficient (verify this). */
  void setUseTable(bool shouldUseTable) { useTable = shouldUseTable; }

  //void addStereoDetune(double newDetune);

  //void addStereoFrequencyOffset(double newOffset);

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

  //-----------------------------------------------------------------------------------------------
  // \name Processing

  /** Calculates one output-sample frame at a time. */
  INLINE void getSampleFrameStereo(double *outL, double *outR)
  {
    if(useTable)
      getFrameViaTable(outL, outR);
    else
      getFrameOnTheFly(outL, outR);
  }

  INLINE void getFrameViaTable(double *outL, double *outR)
  {
    if(!tableUpToDate) updateWaveTable();
    if(!incUpToDate) updateIncrement();
    if(numLines < 2) return;

    // integer and fractional part of position:
    int iPos = floorInt(pos);
    double fPos = pos - iPos;

    // linear interpolation, gain and rotation:
    *outL = normalizer * amplitude * ((1-fPos)*tableX[iPos] + fPos*tableX[iPos+1] - meanX);
    *outR = normalizer * amplitude * ((1-fPos)*tableY[iPos] + fPos*tableY[iPos+1] - meanY);
    rotator.apply(outL, outR);

    // increment and wraparound:
    pos += inc;
    while(pos >= numLines)
      pos -= numLines;
  }

  INLINE void getFrameOnTheFly(double *outL, double *outR)
  {
    if(!commandsReady) updateTurtleCommands();
    if(!incUpToDate) updateIncrement();

    int iPos = floorInt(pos);
    double fPos = pos - iPos;
    if(iPos != lineIndex)
      goToLineSegment(iPos); // may later also update cubic interpolation coeffs, so they 
      // don't need to be recomputed as long as we are traversing the
      // the same line/curve segment, maybe rename to goToLineSegment

    // later, switch other interpolation method here:
    *outL = normalizer * amplitude * ((1-fPos)*x[0] + fPos*x[1] - meanX);
    *outR = normalizer * amplitude * ((1-fPos)*y[0] + fPos*y[1] - meanY);
    rotator.apply(outL, outR);

    // increment and wraparound:
    pos += inc;
    while(pos >= numLines)
      pos -= numLines;
  }
  // get rid of the duplications with getFrameViaTable

  /** Resets the state of the object, such that we start at 0,0 and head towards 1,0 (in 
  unnormalized coordinates). */
  void reset();


protected:

  //-----------------------------------------------------------------------------------------------
  // \name Misc

  /** Updates the buffers that store past and future points between which we interpolate during
  realtime traversal of the curve. */
  void goToLineSegment(int targetLineIndex);
  //void updateRealtimePoints(int targetCommandIndex);

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
  double x[2], y[2];              // x[0]: point we come from, x[1]: point we go to, maybe apply a DC blocking filter to these x,y states
  std::string lindenmayerResult;  // output string of Lindenmayer system
  std::string turtleCommands;     // only the turtle commands from lindenmayerResult 


  // parameters:
  double amplitude  = 1;
  double frequency  = 0;
  double sampleRate = 1;
  double angle      = 0;

  // rendering objects and related variables:
  LindenmayerSystem lindSys;
  TurtleGraphics turtle;
  RAPT::rsRotationXY<double> rotator; // for rotating final x,y coordinates
  int numIterations = 0; // replace by iteratorString or applicatorString (a string like AAABBAC)
  std::string axiom;

  // for optional table-based synthesis (maybe at some point we can drop that?)
  std::vector<double> tableX, tableY;  // rendered (wave)tables for x (left) and y (right)
  bool useTable = true;

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