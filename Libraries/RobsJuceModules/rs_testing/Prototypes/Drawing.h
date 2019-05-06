#pragma once

// rename to Rasterization

// maybe get rid:
typedef RAPT::rsImage<float> rsImageF;
typedef RAPT::rsAlphaMask<float> rsAlphaMaskF;
typedef RAPT::rsImagePainter<float, float, float> rsImagePainterFFF;
typedef RAPT::rsImageDrawer<float, float, float> rsImageDrawerFFF;
typedef RAPT::rsLineDrawer<float, float, float> rsLineDrawerFFF;
typedef RAPT::rsPhaseScopeBuffer<float, float, double> rsPhaseScopeBufferFFD;

//-------------------------------------------------------------------------------------------------
// Utilities

/** Given pixel coordinates x,y and the 3 vertices a,b,c of a triangle (in counterclockwise order), 
this function computes the area in which the triangle and pixel-square overlap. It's a value 
between 0 and 1, where 0 means the pixel and triangle do not intersect at all, 1 means the pixel is
fully covered by the triangle. */
float pixelCoverage(int x, int y, 
  const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c);

//-------------------------------------------------------------------------------------------------
// Lines

void drawLineWuPrototype(rsImageF& img, float x0, float y0, float x1, float y1, float color);
void drawLineBresenham(rsImageF& img, int x0, int y0, int x1, int y1, float color);

void drawThickLine(rsImageF& img, float x0, float y0, float x1, float y1, float color,
  float thickness, bool roundCaps = false);
void drawThickLine2(rsImageF& img, float x0, float y0, float x1, float y1, float color,
  float thickness, int endCaps = 0);
void drawThickLineViaWu(rsImageF& img, float x0, float y0, float x1, float y1, float color, 
  float thickness);

void plotLineWidth(rsImageF& img, int x0, int y0, int x1, int y1, float wd); //?

// line profile functions
float lineIntensity1(float d, float t2);
float lineIntensity2(float d, float t2);
float lineIntensity3(float d, float t2);
float lineIntensity4(float d, float t2);


//-------------------------------------------------------------------------------------------------
// Triangles

/** Fills all pixels whose centers are inside the given triangle with the given color. If a pixel
center is on an edge, it will be considered inside, if it's a top or a left edge 
("top-left rule"). It corresponds to a convention where a pixel with coordinate i (i being the 
integer x or y coordinate) being defined as covering the range [i, i+1), i.e. an interval that 
is closed to the left and open to ther right, i.e. including i but excluding i+1.
The order of the vertices doesn't matter.
A top edge, is an edge that is exactly horizontal and is above the other edges.
A left edge, is an edge that is not exactly horizontal and is on the left side of the triangle. 
A triangle can have one or two left edges.
see:
https://docs.microsoft.com/en-us/windows/desktop/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules#triangle-rasterization-rules-without-multisampling
If this algorithms is used to render several triangles with shared edges (as in a triangle mesh),
each pixel will be visited only once. */
void drawTriangle(rsImageDrawerFFF& drw, 
  const rsVector2DF& v0, const rsVector2DF& v1, const rsVector2DF& v2, float color);

void drawTriangleAntiAliasedProto(rsImageDrawerFFF& drw, 
  const rsVector2DF& v0, const rsVector2DF& v1, const rsVector2DF& v2, float color);
// todo: make sure correct winding order - either make an assertion or re-order vertices here
// uses unweighted area sampling -> disadvantage: objects that are smaller than one pixel don't 
// seem to move at all when they move within one pixel such that no part ever sticks out
// for that, weighted area sampling (maybe with a conic filter) can be used - but exact weighted
// coverage computations are complicated ...maybe use a 3x3 pixel filter with piecewise constant
// weighting, like
// |1 2 1|
// |2 4 2| * 1/12,  ..maybe let the user set the filter kernel coeffs
// |1 2 1|
// default:
// |0 0 0|
// |0 1 0|
// |0 0 0|
// or maybe a 2x2 matrix:
// |1 1| * 1/4
// |1 1| 
// or maybe find the center of gravity of the polygon (triangle-clipped-to-square) and use the
// "paint-dot" operation (with bilinear deinterpolation)


// fast version:
// dispatcher between box-based and span-based:

/** An optimized version - it dispatches between drawTriangleAntiAliasedBoxBased and 
drawTriangleAntiAliasedSpanBased based on the size of the triangle (for small triangles, it may
not be worth it do to the span-based optimizations - the overhead pays off only for larger
triangles). */
void drawTriangleAntiAliased(rsImageDrawerFFF& drw, 
  const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c, float color);

/** As a simple and obvious optimization, this function loops only over those pixels that are 
contained in the bounding box of the triangle. */
void drawTriangleAntiAliasedBoxBased(rsImageDrawerFFF& drw, 
  const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c, float color);

/** As a further optimization, this functions computes 3 spans for each scanline where the first 
and last spans compute coverages and the middle section rund over the pixles that are fully 
covered, so no coverage needs to be computed (it's just 1 everywhere there). */
void drawTriangleAntiAliasedSpanBased(rsImageDrawerFFF& drw, 
  const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c, float color);
// this does not work yet!!!


/** Class for representing an axis aligned rectangle. Mainly for simplifying computations for 
clipping aginst windows or pixels. */

class RectangleF  // just Rectangle doesn't compile (name conflict?)
{

public:

  float xMin = 0.f, xMax = 1.f, yMin = 0.f, yMax = 1.f;

  enum regions
  {
    INSIDE = 0,
    TOP_LEFT,
    CENTER_LEFT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
    CENTER_RIGHT,
    TOP_RIGHT,
    TOP_CENTER
  };

  /** Returns in which of the region the given point falls. */
  int getRegion(const rsVector2DF& p);

  /** Returns whether or not the given point is inside the unit square. Points on the boundary are
  considered inside. */
  bool isInside(const rsVector2DF& p) { return getRegion(p) == INSIDE; }

  /** Given a line L defined by the parametric equation L(t) = a + t*(b-a), this function returns 
  the value of the parameter t for which the line L intersects the top edge of the unit square. If 
  the line is parallel, it will be plus or minus infinity. */
  float getIntersectionParameterTop(const rsVector2DF& a, const rsVector2DF& b)
  {
    return (yMax-a.y) / (b.y-a.y);
  }

  float getIntersectionParameterLeft(const rsVector2DF& a, const rsVector2DF& b)
  {
    return (xMin-a.x) / (b.x-a.x);
  }

  float getIntersectionParameterBottom(const rsVector2DF& a, const rsVector2DF& b)
  {
    return (yMin-a.y) / (b.y-a.y);
  }

  float getIntersectionParameterRight(const rsVector2DF& a, const rsVector2DF& b)
  {
    return (xMax-a.x) / (b.x-a.x);
  }

};

/** Given a triangle defined by the vertices a,b,c, this function returns the number of vertices of
the polygon that results from clipping the triangle to the unit square. It writes the polygon 
vertices into the array p which should be of length 7 (this is the maximum number of vertices that 
the resulting polygon can have). If 0 is returned, the triangle doesn't overlap the unit-square at 
all. */
int clipTriangleToUnitSquare(const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c, 
  rsVector2DF* p);
// under construction


// todo: make class rsRenderer
// setMethod(int);   // rasterize or raytrace
// setAntiAlias(int); // off, unweighted area sampling, weighted area sampling, bilinear deinterpolation, supersampling
// setAreaSamplingFilter(rsImage* filterKernel); // or maybe pass a function-pointer f(float x, float y) definign the continuous kernel function?
// renderScene(rsScene*, rsImage*);
// renderTriangle(rsVertex* a, rsVertex* b, rsVertex* c);
// setVertexShader(rsVertexShader*);
// if i get serious about than, maybe i should call my company Media engineering tools - acronym 
// stays the same..and maybe add a paintbrush that paints a triangle to the logo :-)

// recommended book
// Physically Based Rendering: From Theory To Implementation
// https://www.amazon.de/Physically-Based-Rendering-Matt-Pharr/dp/0128006455/ref=dp_ob_title_bk
// has a website with source code (BSD licensed)
// http://www.pbrt.org/
// https://github.com/mmp/pbrt-v3