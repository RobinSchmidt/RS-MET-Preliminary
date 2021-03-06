#ifndef jura_Plot_h
#define jura_Plot_h 

/** This class is a component, intended to be used as base-class for all components that need some
underlying coordinate-system, such as function-plots, XY-pads, etc. It takes care of the coordinate
axes, a coarse and a fine grid, conversion between component-coordinates and the coordinates in the
desired coordinate-system (which can be lin- or log-scaled).

\todo:

-rename to rsPlot, rsPlotBase or rsPlotComponent  */

class JUCE_API rsPlot : virtual public DescribedComponent, public rsPlotSettingsObserver
{

  friend class CoordinateSystemZoomer;

public:

  rsPlot(const juce::String& newDescription = juce::String("some 2D widget"));
  virtual ~rsPlot();

  //-----------------------------------------------------------------------------------------------
  // \name Component overrides

  /** Lets a context menu pop up when the right button is clicked to allow export of the content
  as image or svg drawing. */
  virtual void mouseDown(const MouseEvent& e) override;

  /** Overrides mouseEnter for displaying the inspection Label. */
  virtual void mouseEnter(const MouseEvent& e) override;

  /** Overrides mouseMove for displaying the inspection Label. */
  virtual void mouseMove(const MouseEvent& e) override;

  virtual void resized() override;
  virtual void paint(Graphics &g) override;

  //-----------------------------------------------------------------------------------------------
  // \name rsPlotSettingsObserver overrides

  /** Possibly triggers a re-rendering when the appearence stting has changed. */
  virtual void rsPlotAppearanceChanged(rsPlotSettings* plotSettings) override;

  /** Updates the input range of our coordinate mapper. */
  virtual void rsPlotVisibleRangeChanged(rsPlotSettings* plotSettings) override;

  /** Does nothing but needs to be overriden. */
  virtual void rsPlotMaximumRangeChanged(rsPlotSettings* plotSettings) override {};

  //-----------------------------------------------------------------------------------------------
  // range-management (split in range setup and range inquiry, inline inquiry functions):

  // i think, they should be non-virtual and just delegate to the corresponding plotSettings
  // method (any updating is then done in the received callbacks):
  virtual void setMaximumRange(double newMinX, double newMaxX, double newMinY, double newMaxY);
  virtual void setMaximumRange(rsPlotRange newMaximumRange);
  virtual void setMaximumRangeX(double newMinX, double newMaxX);
  virtual void setMaximumRangeY(double newMinY, double newMaxY);
  virtual void setMaximumRangeMinX(double newMinX);
  virtual void setMaximumRangeMaxX(double newMaxX);
  virtual void setMaximumRangeMinY(double newMinY);
  virtual void setMaximumRangeMaxY(double newMaxY);
  virtual void setCurrentRange(double newMinX, double newMaxX, double newMinY, double newMaxY);
  virtual void setCurrentRange(rsPlotRange newRange);
  virtual void setCurrentRangeX(double newMinX, double newMaxX);
  virtual void setCurrentRangeY(double newMinY, double newMaxY);
  virtual void setCurrentRangeMinX(double newMinX);
  virtual void setCurrentRangeMaxX(double newMaxX);
  virtual void setCurrentRangeMinY(double newMinY);
  virtual void setCurrentRangeMaxY(double newMaxY);

  rsPlotRange getMaximumRange() const { return plotSettings.getMaximumRange(); }
  rsPlotRange getCurrentRange() const { return plotSettings.getCurrentRange(); }
  double getMaximumRangeMinX() const { return plotSettings.getMaximumRangeMinX(); }
  double getMaximumRangeMaxX() const { return plotSettings.getMaximumRangeMaxX(); }
  double getMaximumRangeMinY() const { return plotSettings.getMaximumRangeMinY(); }
  double getMaximumRangeMaxY() const { return plotSettings.getMaximumRangeMaxY(); }
  double getCurrentRangeMinX() const { return plotSettings.getCurrentRangeMinX(); }
  double getCurrentRangeMaxX() const { return plotSettings.getCurrentRangeMaxX(); }
  double getCurrentRangeMinY() const { return plotSettings.getCurrentRangeMinY(); }
  double getCurrentRangeMaxY() const { return plotSettings.getCurrentRangeMaxY(); }

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

  /** Sets up a caption for the rsPlot and the position where it should appear. */
  virtual void setCaption(const juce::String &newCaption, 
    int newPosition = rsPlotSettings::TOP_CENTER);

  /** Sets the position of the x-axis. For possible values see enum positions. */
  virtual void setAxisPositionX(int newAxisPositionX);

  /** Sets the position of the y-axis. For possible values see enum positions. */
  virtual void setAxisPositionY(int newAxisPositionY);

  /** Sets up several x-axis parameters at once */
  virtual void setupAxisX(double newMin, double newMax, bool shouldBeLogScaled,
    int newAxisPosition, double newCoarseGridInterval, double newFineGridInterval);

  /** Sets up several y-axis parameters at once */
  virtual void setupAxisY(double newMin, double newMax, bool shouldBeLogScaled,
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
  virtual void setHorizontalFineGrid(double newGridInterval, bool shouldBeVisible);

  /** Sets the visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGridVisible(bool shouldBeVisible);

  /** Sets the interval of the vertical coarse grid. */
  virtual void setVerticalCoarseGridInterval(double newGridInterval);

  /** Sets the interval and visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGrid(double newGridInterval, bool shouldBeVisible);

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

  /** Decides if either the x-axis or the y-axis or both should be logarithmically scaled. */
  virtual void useLogarithmicScale(bool shouldBeLogScaledX, bool shouldBeLogScaledY);

  /** Decides, if the x-axis should be logarithmically scaled. */
  virtual void useLogarithmicScaleX(bool shouldBeLogScaledX);

  /** Decides, if the y-axis should be logarithmically scaled. */
  virtual void useLogarithmicScaleY(bool shouldBeLogScaledY);

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
  the rsPlot, it will be re-rendered. However, when you want to change many
  parameters at a time, this can be wasteful in terms of CPU-load. In these cases, it can be
  useful to switch the automatic re-rendering temporarily off. */
  virtual void setAutoReRendering(bool shouldAutomaticallyReRender);

  //-----------------------------------------------------------------------------------------------
  // \name Appearance Inquiry

  PlotColourScheme getPlotColourScheme() const { return plotColourScheme; }
  const RAPT::rsCoordinateMapper2D<double>* getCoordinateMapper() const { return &coordinateMapper; }
  const rsPlotSettings* getPlotSettings() const { return &plotSettings; }

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

  /** Opens a dialog window to export the content of the rsPlot to a png-image file 
  or svg vector drawing file. */
  virtual void openExportDialog(int defaultWidth, int defaultHeight, 
    const juce::String &defaultFormat, const juce::File& defaultTargetFile);

  //-----------------------------------------------------------------------------------------------
  // \name Misc:


  inline double toPixelX(double x)   const { return coordinateMapper.mapX(x); }
  inline double toPixelY(double y)   const { return coordinateMapper.mapY(y); }
  inline double fromPixelX(double x) const { return coordinateMapper.unmapX(x); }
  inline double fromPixelY(double y) const { return coordinateMapper.unmapY(y); }

  /** Function for converting the x- and y-coordinate values into the corresponding coordinates in
  the component (double precision version).*/
  void toPixelCoordinates(double &x, double &y)
  { x = coordinateMapper.mapX(x); y = coordinateMapper.mapY(y); }

  /** Function for converting the x- and y-coordinate values into the corresponding coordinates in
  the component (single precision version).*/
  void toPixelCoordinates(float &x, float &y)
  { x = (float) coordinateMapper.mapX(x); y = (float) coordinateMapper.mapY(y); }

  /** Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (double precision version). */
  void fromPixelCoordinates(double &x, double &y)
  { 
    x = coordinateMapper.unmapX(x); 
    y = coordinateMapper.unmapY(y); 
  }

  /** Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (single precision version). */
  void fromPixelCoordinates(float &x, float &y)
  { 
    x = (float) coordinateMapper.unmapX(x); 
    y = (float) coordinateMapper.unmapY(y); 
  }

  /** We define a member for the mouse-cursor which is to be showed in order to let a
  CoordinateSystemZoomer access this. This is required, because the zoomer object must be above
  the actual rsPlot and therefore prevent the rsPlot to set it's own
  MouseCursor. Instead we just assign the member mouse-cursor and let the zoomer retrieve it. */
  MouseCursor currentMouseCursor;


protected:

  /** Performs a sanity check on the given fird-spacing interval and potentially re-adjusts it. You 
  must tell, whether a log-spcing is used, because sane values are different in this case. */
  void sanityCheckGridSpacing(double& spacing, bool logScaled);

  /** Opens the PopupMenu that appears on right clicks. */
  void openRightClickPopupMenu();

  /** Draws the coordinate system background. */
  virtual void drawCoordinateSystem(Graphics &g);

  /** Updates the image object (re-draws it). Will be called, when something about the
  rsPlot's appearance-settings was changed. */
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