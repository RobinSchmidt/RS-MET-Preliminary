#ifndef jura_PhaseScope_h
#define jura_PhaseScope_h
  

/** Implements the buffering for a phasescope analyzer. 

\todo
-maybe move this class to RAPT
-maybe rename to PixelAccumulator (it may be reusable in more general circumstances)
-refactor to include a general CoordinateTransformer object (as pointer member) which is 
 responsible for the tranformation from input- to pixel coordinates
-optimize:
 -maybe use unsigned char for the storage matrix 
  ->cuts memory use by factor 4
  ->maybe it allows to use saturating arithmetic, avoiding the max() operations
  see:
  https://felix.abecassis.me/2011/10/sse-saturation-arithmetic/
  http://codereview.stackexchange.com/questions/6502/fastest-way-to-clamp-an-integer-to-the-range-0-255
  https://locklessinc.com/articles/sat_arithmetic/
 -or maybe get rid of max function by bit-masking the exponent of the floating point number (make an
  inlined function accumulateAndSaturate(float &accu, float &value)

*/

class JUCE_API PhaseScopeBuffer
{

public:

  /** Constructor. */
  PhaseScopeBuffer();

  /** Destructor. */
  virtual ~PhaseScopeBuffer();

  /** Sets the sample rate. */
  void setSampleRate(double newSampleRate);

  /** Sets the frame rate. */
  void setFrameRate(double newFrameRate);

  /** Sets the time it takes for "color" to decay away. */
  void setDecayTime(double newDecayTime);

  /** Sets the size of the matrix into which we buffer the incoming samples. This should correspond 
  to the pixel size of the display. */
  void setSize(int newWidth, int newHeight);

  /** Switches anti-aliasing on/off. */
  void setAntiAlias(bool shouldAntiAlias);

  /** Sets up the density of the lines the connect our actual datapoints. It actually determines the 
  number of artificial datapoints that are inserted (by linear interpolation) between our actual 
  incoming datapoints. If set to zero, it will just draw the datapoints as dots. */
  void setLineDensity(float newDensity);

  /** Sets up a weight by which ech pixel is not only accumulated into its actual place but also 
  into the neighbouring pixels to add more weight or thickness. It should be a value between 0 
  and 1. */
  void setPixelSpread(float newSpread);

  /** Converts the raw left- and right signal amplitude values to the matrix indices, where the 
  data should be written. This is the xy-pixel coordinates (kept still as real numbers), where the 
  display is to be illuminated in response to the given amplitude values. */
  void convertAmplitudesToMatrixIndices(double &x, double &y);

  /** Accepts one input sample frame for buffering. */
  void bufferSampleFrame(double left, double right);

  /** Applies our pixel decay-factor to the matrix of buffered values. This assumed to be called at 
  the frame rate. */
  void applyPixelDecay();

  /** Resets the internal buffer to all zeros. */
  void reset();

  /** Returns the buffered value at the give xy pixel position. No bounds checking is done - you must
  make sure that the indices are valid. */
  inline float getValueAt(int x, int y) { return buffer[x][y]; }

  /** Returns a pointer to our internally stored data matrix. */
  float** getDataMatrix() { return buffer; }

protected:

  /** Returns the distance between the pixels with coordinates (x1,y1) and (x1,y2). This distance 
  is used in our line drawing function to determine the number of additional dots that are to be 
  inserted between our actual datapoints. */
  float pixelDistance(float x1, float y1, float x2, float y2);

  /** Adds a line to the given x,y coordinates (in pixel coordinates). The starting point of the 
  line are the old pixel coordinates xOld, yOld (member variables). It takes into account our line 
  density - when it's set to zero, it will just draw a dot at the new given position. */
  void addLineTo(float x, float y);

  /** Adds a dot into our data matrix at the given position (given in matrix-index (i.e. pixel-) 
  coordinates using bilinear deinterpolation for anti-aliasing. */
  void addDot(float x, float y);

  /** Like addDot but without anti-aliasing (we just round the coordinates to the nearest 
  integer). */
  void addDotFast(float x, float y);

  /** Allocates the memory for our data matrix buffer. */
  void allocateBuffer();

  /** Frees the memory for our data matrix buffer. */
  void freeBuffer();

  /** Updates the pixel decay factor according to the settings of frame rate and desired decay 
  time. */
  void updateDecayFactor();

  /** Accumulates the given value into the accumulator accu. This accumulation amounts to adding
  the value and the saturating at 1. \todo maybe this can be optimized and/or a different accumulation
  function can be used to get different contrast and saturation behavior.  */
  inline void accumulate(float &accu, float value)
  {
    accu = min(1.f, accu + value);
  }

  double sampleRate;
  double frameRate;
  double decayTime;      // pixel illumination time
  float  decayFactor;    // factor by which pixels decay (applied at frameRate)
  float  insertFactor;   // factor by which are pixels "inserted" (applied at sampleRate)
  float  lineDensity;    // density of the artificial points between actual datapoints
  float  thickness;      // line (or dot) thickness from 0 to 1. 0: one pixel, 1: 3 pixels
                         // maybe rename to spread or weight or something
  float  xOld, yOld;     // pixel coordinates of old datapoint (one sample ago)
  int    width, height;  // pixel width and height
  bool   antiAlias;      // flag to switch anti-aliasing on/off

  // the actual matrix-shaped buffer (maybe use a kind of MatrixView class that wraps a std::vector 
  // later):
  float *bufferFlat;
  float **buffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaseScopeBuffer)
};

//=================================================================================================

/** Implements a phasescope analyzer */

class JUCE_API PhaseScope : public jura::AudioModule
{

public:

  PhaseScope(CriticalSection *lockToUse);

  /** Creates the parameters to control the drawing. 
  \todo: maybe make this function a virtual member function of the AudioModule baseclass to be 
  overriden - the call it from the constructor there. */
  virtual void createParameters();

  /** Sets the desired pixel size. */
  void setPixelSize(int width, int height);

  // parameter setup functions (to be used for the callbacks from the parameters):
  void setAfterGlow(double newGlow);
  void setLineDensity(double newDensity);
  void setPixelSpread(double newSpread);
  //void setFrameRate(double newRate);
  //void setAntiAlias(bool shouldAntiAlias);
  //void setDrawingMode(int newMode);

  inline void triggerPixelDecay() { needsPixelDecay = true; }




  // overriden from AudioModule baseclass:
  AudioModuleEditor *createEditor() override;
  virtual void processBlock(double **inOutBuffer, int numChannels, int numSamples) override;
  virtual void setSampleRate(double newSampleRate) override; 
  virtual void reset() override;

  inline Colour getColourAt(int x, int y) 
  { 
    uint8 c = 255;
    const Colour baseColor(c, c, c, c);  // make user selectable member later
    return baseColor.withAlpha(phaseScopeBuffer.getValueAt(x, y));
  }

protected:

  PhaseScopeBuffer phaseScopeBuffer;

  bool needsPixelDecay;   // flag to indicate that a pixel decay should be triggered

  friend class PhaseScopeDisplay;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaseScope)
};

//=================================================================================================

/** Implements the GUI display for the phase scope. 

\todo Maybe this class should be derived from the CoordinateSystem baseclass and use the angular 
and radial grids from there. 

\todo BUG: there's a flickering of the lines...could this be related to threading issues?
..it's more obvious with faster decay times - maybe it's because the decay is applied in the GUI 
thread whereas accumulation is done in the audio-thread? instead of calling applyPixelDecay in the 
GUI thread we could set a flag in the phaseScope audio module and the apply the decay there

\todo let the internal pixel-buffer size possibly be different from the display size (use image
rescaling) such that the line thickness my scale up when increasing the display size

*/

class JUCE_API PhaseScopeDisplay : public Component, public Timer
{

public:

  PhaseScopeDisplay(jura::PhaseScope *newPhaseScopeToEdit);

  virtual void resized() override;
  virtual void paint(Graphics &g)	override;
  virtual void timerCallback() override;

protected:

  PhaseScope *phaseScope;

  Image image;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaseScopeDisplay)
};

//=================================================================================================

/** Implements a GUI editor for the XY phase scope. */

class JUCE_API PhaseScopeEditor : public AudioModuleEditor
{

public:

  PhaseScopeEditor(jura::PhaseScope *newPhaseScopeToEdit);

  virtual void createWidgets();

  //void setDisplayPixelSize(int newWidth, int newHeight);

  virtual void resized() override;

protected:

  PhaseScope *scope;
  PhaseScopeDisplay display;
  int widgetMargin;

  // Widgets:
  RSlider *afterglowSlider, *pixelSpreadSlider, *lineDensitySlider;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaseScopeEditor)
};




#endif 