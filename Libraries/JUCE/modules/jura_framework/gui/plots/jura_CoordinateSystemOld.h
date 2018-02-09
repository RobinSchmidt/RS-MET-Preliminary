#ifndef jura_CoordinateSystemOld_h
#define jura_CoordinateSystemOld_h 

/** This class is a component, intended to be used as base-class for all components that need some
underlying coordinate-system, such as function-plots, XY-pads, etc. It takes care of the coordinate
axes, a coarse and a fine grid, conversion between component-coordinates and the coordinates in the
desired coordinate-system (which can be lin- or log-scaled).

\todo:

-rename to rsPlot, rsPlotBase or rsPlotComponent  */

class JUCE_API CoordinateSystemOld : virtual public DescribedComponent
{

  friend class CoordinateSystemZoomer;

public:

  CoordinateSystemOld(const juce::String& newDescription = juce::String("some 2D widget"));
  virtual ~CoordinateSystemOld();

  //-----------------------------------------------------------------------------------------------
  // \name Component overrides

  /** Lets a context menu pop up when the right button is clicked to allow export of the content
  as image or svg drawing. */
  virtual void mouseDown(const MouseEvent& e);

  /** Overrides mouseEnter for displaying the inspection Label. */
  virtual void mouseEnter(const MouseEvent& e);

  /** Overrides mouseMove for displaying the inspection Label. */
  virtual void mouseMove(const MouseEvent& e);

  virtual void resized();
  virtual void paint(Graphics &g);

  //-----------------------------------------------------------------------------------------------
  // range-management (split in range setup and range inquiry, inline inquiry functions):

  virtual void setMaximumRange(double newMinX, double newMaxX, double newMinY, double newMaxY);
  /**< Sets the maximum for the currently visible range. For logarithmic x- and/or y-axis-scaling,
  make sure that the respective minimum value is greater than zero! */

  virtual void setMaximumRange(rsPlotRange newMaximumRange);
  /**< Sets the maximum for the currently visible range. */

  virtual void setMaximumRangeX(double newMinX, double newMaxX);
  /**< Sets the maximum visible range for the y-axis. */

  virtual void setMaximumRangeY(double newMinY, double newMaxY);
  /**< Sets the maximum visible range for the y-axis. */

  virtual rsPlotRange getMaximumRange() { return plotSettings.maximumRange; }
  /**< Returns the maximum for the currently visible range. */

  virtual void setMaximumRangeMinX(double newMinX);
  /**< Sets the minimum value for the range of x. */

  virtual double getMaximumRangeMinX() { return plotSettings.maximumRange.getMinX(); }
  /**< Returns the minimum value for the range of x. */

  virtual void setMaximumRangeMaxX(double newMaxX);
  /**< Sets the maximum value for the range of x. */

  virtual double getMaximumRangeMaxX() { return plotSettings.maximumRange.getMaxX(); }
  /**< Returns the maximum value for the range of x. */

  virtual void setMaximumRangeMinY(double newMinY);
  /**< Sets the minimum value for the range of y. */

  virtual double getMaximumRangeMinY() { return plotSettings.maximumRange.getMinY(); }
  /**< Returns the minimum value for the range of y. */

  virtual void setMaximumRangeMaxY(double newMaxY);
  /**< Sets the maximum value for the range of y. */

  virtual double getMaximumRangeMaxY() { return plotSettings.maximumRange.getMaxY(); }
  /**< Returns the maximum value for the range of y. */

  virtual void setCurrentRange(double newMinX, double newMaxX, double newMinY, double newMaxY);
  /**< Sets the currently visible range. For logarithmic x- and/or y-axis-scaling, make sure that
  the respective minimum value is greater than zero! */

  virtual void setCurrentRange(rsPlotRange newRange);
  /**< Sets the currently visible range. */

  virtual void setCurrentRangeX(double newMinX, double newMaxX);
  /**< Sets the currently visible range for the y-axis. */

  virtual void setCurrentRangeY(double newMinY, double newMaxY);
  /**< Sets the currently visible range for the y-axis. */

  virtual rsPlotRange getCurrentRange() { return plotSettings.currentRange; }
  /**< Returns the currently visible range. */

  virtual void setCurrentRangeMinX(double newMinX);
  /**< Sets the minimum value of x. */

  virtual double getCurrentRangeMinX() { return plotSettings.currentRange.getMinX(); }
  /**< Returns the minimum value of x. */

  virtual void setCurrentRangeMaxX(double newMaxX);
  /**< Sets the maximum value of x. */

  virtual double getCurrentRangeMaxX() { return plotSettings.currentRange.getMaxX(); }
  /**< Returns the maximum value of x. */

  virtual void setCurrentRangeMinY(double newMinY);
  /**< Sets the minimum value of y. */

  virtual double getCurrentRangeMinY() { return plotSettings.currentRange.getMinY(); }
  /**< Returns the minimum value of y. */

  virtual void setCurrentRangeMaxY(double newMaxY);
  /**< Sets the maximum value of y. */

  virtual double getCurrentRangeMaxY() { return plotSettings.currentRange.getMaxY(); }
  /**< Returns the maximum value of y. */

  /** Returns a string that represents the info-line to be shown when mouse is over pixel (x,y). */
  virtual juce::String getInfoLineForPixelPosition(int x, int y);

  //-----------------------------------------------------------------------------------------------
  // \name Appearance Setup

  /** Sets up the colour-scheme. */
  virtual void setColourScheme(const PlotColourScheme& newColourScheme);

  /** Sets up the colour-scheme to be used for the right-click popup menu. */
  virtual void setPopUpColourScheme(const WidgetColourScheme& newColourScheme)
  {
    popUpColourScheme = newColourScheme;
  }

  /** Sets up the colour-scheme from an XmlElement. */
  virtual void setColourSchemeFromXml(const XmlElement& xml);

  /** Sets up a caption for the CoordinateSystemOld and the position where it should appear. */
  virtual void setCaption(const juce::String &newCaption, 
    int newPosition = rsPlotSettings::TOP_CENTER);

  /** Sets the position of the x-axis. For possible values see enum positions. */
  virtual void setAxisPositionX(int newAxisPositionX);

  /** Sets the position of the y-axis. For possible values see enum positions. */
  virtual void setAxisPositionY(int newAxisPositionY);

  /** Sets up several x-axis parameters at once */
  virtual void setupAxisX(double newMin, double newMax, bool shouldBeLogScaled, double newLogBase,
    int newAxisPosition, double newCoarseGridInterval, double newFineGridInterval);

  /** Sets up several y-axis parameters at once */
  virtual void setupAxisY(double newMin, double newMax, bool shouldBeLogScaled, double newLogBase,
    int newAxisPosition, double newCoarseGridInterval, double newFineGridInterval);

  /** Sets the visibility of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGridVisible(bool shouldBeVisible);

  /** Sets the interval of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the horizontal fine grid. */
  virtual void setHorizontalFineGridVisible(bool shouldBeVisible);

  /** Sets the interval of the horizontal fine grid. */
  virtual void setHorizontalFineGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the horizontal fine grid. */
  virtual void setHorizontalFineGrid(double newGridInterval, bool   shouldBeVisible);

  /** Sets the visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGridVisible(bool shouldBeVisible);

  /** Sets the interval of the vertical coarse grid. */
  virtual void setVerticalCoarseGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGrid(double newGridInterval,
    bool   shouldBeVisible);

  /** Sets the visibility of the vertical fine grid. */
  virtual void setVerticalFineGridVisible(bool shouldBeVisible);

  /** Sets the interval of the vertical fine grid. */
  virtual void setVerticalFineGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the vertical fine grid. */
  virtual void setVerticalFineGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the radial coarse grid. */
  virtual void setRadialCoarseGridVisible(bool shouldBeVisible);

  /** Sets the interval of the radial coarse grid. */
  virtual void setRadialCoarseGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the radial coarse grid. */
  virtual void setRadialCoarseGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the radial fine grid. */
  virtual void setRadialFineGridVisible(bool shouldBeVisible);

  /** Sets the interval of the radial fine grid. */
  virtual void setRadialFineGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the radial fine grid. */
  virtual void setRadialFineGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the angular coarse grid. */
  virtual void setAngularCoarseGridVisible(bool shouldBeVisible);

  /** Sets the interval of the angular coarse grid. */
  virtual void setAngularCoarseGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the angular coarse grid. */
  virtual void setAngularCoarseGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the angular fine grid. */
  virtual void setAngularFineGridVisible(bool shouldBeVisible);

  /** Sets the interval of the angular fine grid. */
  virtual void setAngularFineGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the angular fine grid. */
  virtual void setAngularFineGrid(double newGridInterval, bool shouldBeVisible);

  /** Decides if either the x-axis or the y-axis or both should be logarithmically scaled and sets 
  up the base for the logarithms. */
  virtual void useLogarithmicScale(bool shouldBeLogScaledX, bool   shouldBeLogScaledY,
    double newLogBaseX = 2.0, double newLogBaseY = 2.0);

  /** Decides, if the x-axis should be logarithmically scaled and sets up the base for the 
  logarithm. */
  virtual void useLogarithmicScaleX(bool shouldBeLogScaledX, double newLogBaseX = 2.0);

  /** Decides, if the y-axis should be logarithmically scaled and sets up the base for the 
  logarithm. */
  virtual void useLogarithmicScaleY(bool shouldBeLogScaledY, double newLogBaseY = 2.0);

  /** Sets the labels for the axes and their position. */
  virtual void setAxisLabels(const juce::String &newLabelX, const juce::String &newLabelY,
    int newLabelPositionX = rsPlotSettings::ABOVE_AXIS, 
    int newLabelPositionY = rsPlotSettings::RIGHT_TO_AXIS);

  /** Sets the label for the x-axis and its position. */
  virtual void setAxisLabelX(const juce::String &newLabelX, 
    int newLabelPositionX = rsPlotSettings::ABOVE_AXIS);

  /** Sets the label for the y-axis and its position. */
  virtual void setAxisLabelY(const juce::String &newLabelY, 
    int newLabelPositionY = rsPlotSettings::RIGHT_TO_AXIS);

  /** Switches x-value annotation between below or above the x-axis
  (or off). */
  virtual void setAxisValuesPositionX(int newValuesPositionX);

  /** Switches y-value annotation between left to or right to the y-axis
  (or off). */
  virtual void setAxisValuesPositionY(int newValuesPositionY);

  /** This function is used to pass a function-pointer with the address of a function which has a 
  double-parameter and a juce::String as return-value. The function will be used to convert the 
  values on the x-axis into corresponding strings for display on the axis. */
  virtual void setStringConversionForAxisX(
    juce::String (*newConversionFunction) (double valueToBeConverted));

  /** Like setStringConversionForAxisX but for y-axis. */
  virtual void setStringConversionForAxisY(
    juce::String (*newConversionFunction) (double valueToBeConverted));

  /** Like setStringConversionForAxisX but not for the axis but for the info-line. */
  virtual void setStringConversionForInfoLineX(
    juce::String (*newConversionFunction) (double valueToBeConverted));

  /** Like setStringConversionForAxisY but not for the axis but for the info-line. */
  virtual void setStringConversionForInfoLineY(
    juce::String (*newConversionFunction) (double valueToBeConverted));

  /** With this function, the automatic re-rendering of the underlying image can be turned on or
  off. If on (default), everytime you change a parameter which will change the appearance of
  the CoordinateSystemOld, it will be re-rendered. However, when you want to change many
  parameters at a time, this can be wasteful in terms of CPU-load. In these cases, it can be
  useful to switch the automatic re-rendering temporarily off. */
  virtual void setAutoReRendering(bool shouldAutomaticallyReRender);

  //-----------------------------------------------------------------------------------------------
  // \name Appearance Inquiry

  PlotColourScheme getPlotColourScheme() const { return plotColourScheme; }

  bool isHorizontalCoarseGridVisible() const { return plotSettings.horizontalCoarseGridIsVisible; }
  bool isHorizontalFineGridVisible()   const { return plotSettings.horizontalFineGridIsVisible; }
  bool isVerticalCoarseGridVisible()   const { return plotSettings.verticalCoarseGridIsVisible; }
  bool isVerticalFineGridVisible()     const { return plotSettings.verticalFineGridIsVisible; }
  bool isRadialCoarseGridVisible()     const { return plotSettings.radialCoarseGridIsVisible; }
  bool isRadialFineGridVisible()       const { return plotSettings.radialFineGridIsVisible; }
  bool isAngularCoarseGridVisible()    const { return plotSettings.angularCoarseGridIsVisible; }
  bool isAngularFineGridVisible()      const { return plotSettings.angularFineGridIsVisible; }

  double getHorizontalCoarseGridInterval() const { return plotSettings.horizontalCoarseGridInterval; }
  double getHorizontalFineGridInterval()   const { return plotSettings.horizontalFineGridInterval; }
  double getVerticalCoarseGridInterval()   const { return plotSettings.verticalCoarseGridInterval; }
  double getVerticalFineGridInterval()     const { return plotSettings.verticalFineGridInterval; }
  double getRadialCoarseGridInterval()     const { return plotSettings.radialCoarseGridInterval; }
  double getRadialFineGridInterval()       const { return plotSettings.radialFineGridInterval; }
  double getAngularCoarseGridInterval()    const { return plotSettings.angularCoarseGridInterval; }
  double getAngularFineGridInterval()      const { return plotSettings.angularFineGridInterval; }

  bool isLogScaledX() const { return plotSettings.logScaledX; }
  bool isLogScaledY() const { return plotSettings.logScaledY; }

  //-----------------------------------------------------------------------------------------------
  // \name State Management:

  /** Creates an XmlElement from the current state and returns it. */
  virtual XmlElement* getStateAsXml(
    const juce::String& stateName = juce::String("CoordinateSystemState")) const;

  /** Restores a state based on an XmlElement which should have been created
  with the getStateAsXml()-function. */
  virtual bool setStateFromXml(const XmlElement &xmlState);

  //-----------------------------------------------------------------------------------------------
  // \name Export:

  /** Returns the drawing as SVG compliant XmlElement. The caller must take care to delete the
  pointer to the XmlElement when it's not needed anymore. */
  virtual XmlElement* getPlotAsSVG(int width, int height);

  /** Renders the plot to an image object of given width and height. The caller must take care to
  delete the pointer to the image when it's not needed anymore. */
  virtual Image* getPlotAsImage(int width, int height);

  /** Opens a dialog window to export the content of the CoordinateSystemOld to a png-image file 
  or svg vector drawing file. */
  virtual void openExportDialog(int defaultWidth, int defaultHeight, 
    const juce::String &defaultFormat, const juce::File& defaultTargetFile);

  //-----------------------------------------------------------------------------------------------
  // \name Misc:

  /** We define a member for the mouse-cursor which is to be showed in order to let a
  CoordinateSystemZoomer access this. This is required, because the zoomer object must be above
  the actual CoordinateSystemOld and therefore prevent the CoordinateSystemOld to set it's own
  MouseCursor. Instead we just assign the member mouse-cursor and let the zoomer retrieve it. */
  MouseCursor currentMouseCursor;

protected:

  /** Opens the PopupMenu that appears on right clicks. */
  void openRightClickPopupMenu();

  /** Draws the coordinate system background. */
  virtual void drawCoordinateSystem(Graphics &g);




  //virtual void transformToComponentsCoordinates(double &x, double &y);
  /**< Function for converting the x- and y-coordinate values into the corresponding coordinates in
  the component (double precision version).*/

  //virtual void transformToComponentsCoordinates(float &x, float &y);
  /**< Function for converting the x- and y-coordinate values into the corresponding coordinates in
  the component (single precision version).*/

  //virtual void transformFromComponentsCoordinates(double &x, double &y);
  /**< Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (double precision version). */

  //virtual void transformFromComponentsCoordinates(float &x, float &y);
  /**< Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (single precision version). */

  // convenience functions, rename to toPixelCoords, fromPixelCoords

  void transformToComponentsCoordinates(double &x, double &y)
  { x = coordinateMapper.mapX(x); y = coordinateMapper.mapY(y); }

  void transformToComponentsCoordinates(float &x, float &y)
  { x = (float) coordinateMapper.mapX(x); y = (float) coordinateMapper.mapY(y); }

  void transformFromComponentsCoordinates(double &x, double &y)
  { x = coordinateMapper.unmapX(x); y = coordinateMapper.unmapY(y); }

  void transformFromComponentsCoordinates(float &x, float &y)
  { x = (float) coordinateMapper.unmapX(x); y = (float) coordinateMapper.unmapY(y); }



  /** Updates the image object (re-draws it). Will be called, when something about the
  CoordinateSystemOld's appearance-settings was changed. */
  virtual void updateBackgroundImage();

  /** Sets up the output range (i.e. the pixel width and height) in our coordinateMapper. If a 
  non-nullptr is passed for targetImage, the image size will be used, else if a non-nullptr for
  the targetSVG is passed, its size will be used (the xml should already have "width" and "height"
  attributes), else this Component's size will be used. */
  void updateMapperOutputRange(Image* targetImage = nullptr, XmlElement* targetSVG = nullptr);
    // get rid of the image and svg pointers - i think they serve no purpose anymore

  /** Updates the input range of our coordinate mapper. */
  void updateMapperInputRange();


  rsPlotSettings plotSettings;


  // new - to be used soon in the drawing code:
  RAPT::rsCoordinateMapper2D<double> coordinateMapper; 
    // get rid of that - it's handled in rsPlotDrawer now...o...but we need to map form 
    // pixel-coordinates to component coordinates for node-editing..but maybe that can be handled
    // in rsNodeEditor? i think, it has a mapper itself?


  bool showPositionAsDescription;
  bool showPopUpOnRightClick;


  Image*  backgroundImage;
  /**< This image will be used for the appearance of the coodinate system, it will be updated via
  the updateBackgroundImage()-function when something about the coordinate-system (axes, grid,
  etc.) changes - but only if autoReRender is true (which it is by default). */

  bool autoReRenderImage;
  // see above

  juce::String (*stringConversionForInfoLineX) (double valueToConvert);
  juce::String (*stringConversionForInfoLineY) (double valueToConvert);

  // color-scheme management:
  PlotColourScheme   plotColourScheme;
  WidgetColourScheme popUpColourScheme;

  juce_UseDebuggingNewOperator;
};

#endif