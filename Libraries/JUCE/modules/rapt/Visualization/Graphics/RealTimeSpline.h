#ifndef RAPT_REALTIMESPLINE_H_INCLUDED
#define RAPT_REALTIMESPLINE_H_INCLUDED

/** A class to compute dot-locations and brightness-values for realtime scope visualization of
incoming sample points. The class computes the dot location by interpolating the incoming 
data points in various ways. In the simplest case, it just produces dots that sit along a line that
connects two successive input sample points, but it can do more sophisticated things, like 
connecting the incoming data-points with spline segments and it can also ensure that the dots along
the spline segments are equally spaced. */

template<class TCor, class TWgt>
class rsRealTimeSpline
{

public:

  /** Standard constructor. Initializes to line drawing mode. */
  rsRealTimeSpline();



  //-----------------------------------------------------------------------------------------------
  /** \name Setup */


  //-----------------------------------------------------------------------------------------------
  /** \name Processing */

  /** This is the main processing function to be called from client code. You supply a new pair of 
  input coordinates newX, newY and the function gives you back the locations and brightness values
  for the dots to draw. ... The return value is the number of dots produced. */
  int getDotsForInputPoint(TCor newX, TCor newY, TCor* dotsX, TCor* dotsY, TWgt weights, 
    int xywLength);

  /** Resets the input point buffers to the given coodinate values. You will typically want to pass
  incoordinates that correspond to the origin of you coordinate system, either in pixel coordinates
  or in normalized device coordinates. */
  void reset(TCor x = TCor(0), TCor y = TCor(0));
  // todo: facilitate use with pixel-coordinates (as required by rsPhaseScopeBuffer) or normalized 
  // device coordinates (as required for use with OpenGL)

protected:

  TCor density;
  int maxNumDots = -1; // -1 is code for: no limit
  TCor x[4], y[4];
  TWgt cOld;           // old line end color

};
// maybe rename to RealTimeSplineGenerator

#endif