using namespace RAPT;

void lineDrawing()
{
  // Compares different line drawing algorithms. We draw lines of different directions.
  // todo:
  // Move the prototype implementations to another shared file, so they can also be used in unit
  // tests
  // drag the prototypes to ImagePainter, write unit tests, do optimzations, write performance
  // comparison between Bresenham, Wu and Dotted line

  // user parameters:
  int imageWidth   = 400;
  int imageHeight  = 400;
  int numLines     = 10;    // number of lines per direction (horizontal'ish and vertical'ish)
  float margin     = 10.f;
  float brightness = 0.5f;

  // create objects:
  rsImageF image(imageWidth, imageHeight);
  rsImagePainterFFF painter(&image, nullptr);
  float x0 = margin;
  float y0 = margin;
  int i;

  // create arrays for line endpoints:
  vector<float> x1, y1;
  for(i = 0; i < numLines; i++){    // flat, horizontal'ish
    x1.push_back(imageWidth - margin);
    y1.push_back(margin + i * (imageHeight - margin) / numLines); }
  x1.push_back(imageWidth -margin); // 45� diagonal
  y1.push_back(imageHeight-margin);
  for(i = 0; i < numLines; i++){    // steep, vertical'ish
    x1.push_back(margin + i * (imageWidth - margin) / numLines);
    y1.push_back(imageHeight - margin); }

  //// dotted algorithm:
  //for(i = 0; i < x1.size(); i++)
  //  painter.drawLineDotted(x0, y0, x1[i], y1[i], brightness, brightness, numDots); // we need to pass a number of dots
  //writeImageToFilePPM(image, "LinesDotted.ppm");

  // Wu algorithm:
  image.clear();
  for(i = 0; i < x1.size(); i++)
    painter.drawLineWu(x0, y0, x1[i], y1[i], brightness);
    //drawLineWuPrototype(image, x0, y0, x1[i], y1[i], brightness);
  writeImageToFilePPM(image, "LinesWu.ppm");

  // Bresenham algorithm:
  image.clear();
  for(i = 0; i < x1.size(); i++)
    drawLineBresenham(image, rsRoundToInt(x0), rsRoundToInt(y0), 
      rsRoundToInt(x1[i]), rsRoundToInt(y1[i]), brightness);
  writeImageToFilePPM(image, "LinesBresenham.ppm");
}

void lineDrawingThick()
{
  // user parameters:
  int imageWidth   = 800;
  int imageHeight  = 800;
  int numAngles     = 7;
  float brightness = 0.75f;
  float thickness  = 50.f;

  // create objects:
  rsImageF image(imageWidth, imageHeight);
  rsLineDrawerFFF drawer(&image);
  drawer.setColor(brightness);
  drawer.setBlendMode(rsImageDrawerFFF::BLEND_ADD_SATURATE);
  drawer.setLineWidth(thickness);
  drawer.setLineProfile(rsLineDrawerFFF::PROFILE_LINEAR);
  drawer.setRoundCaps(false);

  // create endpoint arrays:
  float margin = 2*thickness;
  int numLines = 2*numAngles - 2;
  vector<float> x0(numLines), y0(numLines), x1(numLines), y1(numLines);
  int i, j;
  for(i = 0; i < numAngles; i++){
    x0[i] = margin + i * (imageWidth - 2*margin) / (numAngles-1);
    y0[i] = margin;
    x1[i] = imageWidth - x0[i];
    y1[i] = imageHeight - margin; }
  for(i = 0; i < numAngles-2; i++){
    j = numAngles + i;
    y0[j] = margin + (i+1) * (imageHeight - 2*margin) / (numAngles-1);
    x0[j] = margin;
    y1[j] = imageHeight - y0[j];
    x1[j] = imageWidth - margin; }

  // draw the lines and save file:
  for(i = 0; i < numLines; i++)
  {
    drawer.drawLine(x0[i], y0[i], x1[i], y1[i]);
    //drawThickLine(image, x0[i], y0[i], x1[i], y1[i], brightness, thickness, true);
  }
  writeImageToFilePPM(image, "LinesThick.ppm");
}
void lineDrawingThick2()
{
  // user parameters:
  int imageWidth   = 100;
  int imageHeight  = 100;
  float brightness = 0.5f;
  float thickness  = 10.f;
  float x0 = 10.3f, y0 = 10.6f, x1 = 90.2f, y1 = 40.4f;
  //float x0 = 10, y0 = 10, x1 = 90, y1 = 40;

  rsImageF image(imageWidth, imageHeight);
  //drawThickLine(image, 10, 10, 70, 30, 1.f, 15.f); // dx > dy, x0 < x1, base case
  //drawThickLine(image, 20, 20, 80, 80, 1.f, 15.f); // dx = dy, x0 < x1, 45� diagonal
  //drawThickLine(image, 10, 10, 30, 70, 1.f, 15.f); // dx < dy, x0 < x1, steep case
  //drawThickLine(image, 70, 10, 10, 30, 1.f, 15.f); // dx > dy, x0 > x1, x-swap case
  //drawThickLine(image, 10, 30, 70, 10, 1.f, 15.f);
  //drawThickLine(image, 30, 10, 10, 70, 1.f, 15.f);
  //drawThickLine(image, 10, 10, 25, 15, 1.f,  8.f, true);
  //drawThickLine(image, 20, 20, 50, 30, 1.f, 16.f, false);
  drawThickLine(image, 20, 20, 50, 50, 1.f, 20.f, true);
  drawThickLine(image, 20, 50, 50, 80, 1.f, 20.f, false);
  //drawThickLine(image, 20, 70, 50, 70, 1.f, 5.f, false);
                                            
  //drawThickLine(image, 10, 10, 50, 90, 1.f, 15.f);
  //drawThickLine(image, x0, y0, x1, y1, brightness, thickness);
  //plotLineWidth(image, (int)x0, (int)y0, (int)x1, (int)y1, thickness);

  writeImageToFilePPM(image, "ThickLineScanlineTest.ppm");
}

void lineJoints()
{
  // user parameters:
  int imageWidth   = 800;
  int imageHeight  = 800;
  int numAngles    = 10;
  float brightness = 0.5f;
  float thickness  = 20.f;

  // create objects:
  rsImageF image(imageWidth, imageHeight);
  rsLineDrawerFFF drawer(&image);
  drawer.setBlendMode(rsImageDrawerFFF::BLEND_ADD_SATURATE);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_LINEAR);
  drawer.setLineProfile(rsLineDrawerFFF::PROFILE_FLAT);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_CUBIC);
  drawer.setLineWidth(thickness);
  drawer.setColor(brightness);

  // create endpoint arrays:
  // draw lines:
  float margin = 2*thickness;
  //vector<float> x0, y0, x1, y1;
  float x0, y0, x1, y1;
  float w2 = 0.5f*imageWidth;
  float h2 = 0.5f*imageHeight;
  float dy = (float)margin;
  float offset;
  for(int i = 0; i < numAngles; i++)
  {
    offset = i*margin;
    x0 = margin;
    y0 = margin + offset;
    x1 = w2;
    y1 = margin + i*dy + offset;
    drawer.drawLine(x0, y0, x1, y1);
    //drawThickLine(image, x0, y0, x1, y1, brightness, thickness, true);
    x0 = x1;
    y0 = y1;
    x1 = imageWidth - margin;
    y1 = margin + offset;
    drawer.drawLine(x0, y0, x1, y1);
    //drawThickLine(image, x0, y0, x1, y1, brightness, thickness, true);
  }

  writeImageToFilePPM(image, "LineJoints.ppm");
}

void lineTo()
{
  // Test the lineTo function by drawing 4 lines using lineTo for the 4 combinations of back/steep

  // user parameters:
  int size = 800;         // image width and height
  float brightness = 0.5f;
  float thickness  = 20.f;

  // create objects:
  rsImageF image(size, size);
  rsLineDrawerFFF drawer(&image);
  drawer.setBlendMode(rsImageDrawerFFF::BLEND_ADD_SATURATE);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_LINEAR);
  drawer.setLineProfile(rsLineDrawerFFF::PROFILE_FLAT);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_CUBIC);
  drawer.setLineWidth(thickness);
  drawer.setColor(brightness);

  float margin = 2*thickness;

  // flat, forward:
  drawer.initPolyLine(     margin, 0.4f*size);
  drawer.lineTo(  size-margin, 0.6f*size);

  // flat, backward:
  drawer.initPolyLine(size-margin, 0.4f*size);
  drawer.lineTo(       margin, 0.6f*size);

  // steep, forward:
  drawer.initPolyLine(0.4f*size,      margin);
  drawer.lineTo(  0.6f*size, size-margin);

  // steep, backward:
  drawer.initPolyLine(0.4f*size, size-margin);
  drawer.lineTo(  0.6f*size,      margin);

  writeImageToFilePPM(image, "LineTo.ppm");
}

void polyLineRandom()
{
  // user parameters:
  int imageWidth      = 800;
  int imageHeight     = 800;
  int numLines        = 50;
  float minBrightness = 0.125f;
  float maxBrightness = 1.0f;
  float thickness     = 20.f;

  // create objects:
  rsImageF image(imageWidth, imageHeight);
  rsLineDrawerFFF drawer(&image);
  //drawer.setBlendMode(rsImageDrawerFFF::BLEND_ADD_SATURATE);
  drawer.setBlendMode(rsImageDrawerFFF::BLEND_ADD_CLIP);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_LINEAR);
  drawer.setLineProfile(rsLineDrawerFFF::PROFILE_FLAT);
  //drawer.setLineProfile(rsLineDrawerFFF::PROFILE_CUBIC);
  drawer.setLineWidth(thickness);
  //drawer.setColor(brightness);
  drawer.setRoundCaps(true);

  float margin = 2*thickness;
  float xMin, yMin, xMax, yMax;
  float br;
  xMin = yMin = margin;
  xMax = imageWidth  - margin;
  yMax = imageHeight - margin;
  //drawer.drawLine(xMin, yMin, xMax, yMax);
  rsRandomUniform(0.0, 1.0, 1);
  float x0, y0, x1, y1;
  x0 = (float) rsRandomUniform(xMin, xMax);  
  y0 = (float) rsRandomUniform(yMin, yMax);
  drawer.initPolyLine(x0, y0);
  for(int i = 2; i <= numLines; i++)
  {
    x1 = (float) rsRandomUniform(xMin, xMax);
    y1 = (float) rsRandomUniform(yMin, yMax);
    br = (float) rsRandomUniform(minBrightness, maxBrightness);
    //drawer.drawLine(x0, y0, x1, y1);
    drawer.setColor(br);
    //drawer.lineTo(x1, y1);
    drawer.lineTo(x1, y1, true); // true: line joining code for uniform color polylines
    x0 = x1;
    y0 = y1;
  }

  writeImageToFilePPM(image, "PolyLineRandom.ppm");
}

void phaseScopeLissajous()
{
  // We create a PhaseScope image of a Lissajous figure to test the drawing code.
  // x(t) = sin(2*pi*a*t), y(t) = sin(2*pi*b*t)

  // input signal parameters:
  static const int N = 11;  // number of data points per cycle
  //static const int N = 80;
  //static const int N = 35;
  //static const int N = 23;
  float a = 2.f;
  float b = 3.f;
  float scale = 0.9f;
  //static const int numCycles = 1;

  typedef RAPT::rsRealTimeSpline<double, float> SG; // for spline-generator


  // create and set up rsPhaseScopeBuffer object:
  rsPhaseScopeBufferFFD psb;
  psb.setSampleRate(N);
  psb.setAntiAlias(true);
  //psb.setBrightness(300.f);  // wtf? - why did this work formerly?
  psb.setBrightness(0.1f); 
  psb.setLineDensity(1.f);
  psb.setPixelSpread(0.3f);
  //psb.setDrawMode(SG::LINEAR);
  //psb.setDrawMode(SG::CUBIC_HERMITE);
  psb.setDrawMode(SG::QUADRATIC);
  psb.setUseColorGradient(true);
  psb.setDensityNormalization(true);
  psb.setSize(400, 400);

  //// settings for testing color discontinuities in spline drawing (remove when problems are fixed):
  //psb.setAntiAlias(false);
  //psb.setUseColorGradient(false);
  //psb.setLineDensity(0.3f);
  //psb.setPixelSpread(0.0f);
  //psb.setSize(800, 800);
  //psb.setOneDimensionalMode(true); // for debug

  // create image:
  psb.reset();
  float x[N], y[N];
  float s = float(2*PI) / N;
  for(int n = 0; n < N; n++) {
    x[n] = scale*sin(s*a*n); y[n] = scale*sin(s*b*n); psb.processSampleFrame(x[n], y[n]);
  }
  // first run through the figure was only for warm-up (to avoid initial artifacts):
  psb.clearImage();
  for(int n = 0; n < N; n++) {
    x[n] = scale*sin(s*a*n); y[n] = scale*sin(s*b*n); psb.processSampleFrame(x[n], y[n]);
  } // factor out this loop into a function drawLissajous(...)

  // retrieve and save image:
  psb.getImage();
  writeImageToFilePPM(*psb.getImage(), "PhaseScopeLissajous.ppm");

  //// plot (for reference):
  //GNUPlotter plt;
  //plt.addDataArrays(N, x, y);
  //plt.plot();
}

void splineArc()
{
  // Tests the spline arc drawing - especially the numeric integration of the arc-length

  //int numDots = 100;

  float density = 0.125;
  int N = 100;        // number of spline evaluation samples
  int width   = 400;
  int height  = 400;

  //float x1 = 0, y1 = 0, x1s = 0, y1s =  1; // start at (0,0), pointing upward
  //float x2 = 1, y2 = 0, x2s = 0, y2s = -1; // end   at (1,0), pointing downward
  float x1 =  10, y1 = 10, x1s = 0, y1s =  1500; // start center left, pointing upward
  float x2 = 390, y2 = 10, x2s = 0, y2s = -1500; // end center right, pointing downward
  float distance = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)); // distance between the two points

  //int numDots = density*distance

  // maybe to test the density, have all points on one lines

  // compute polynomial coefficients:
  float a[4], b[4];
  cubicSplineArcCoeffs2D(x1, x1s, y1, y1s, x2, x2s, y2, y2s, a, b);

  typedef rsPolynomial<float> PL;

  // create points via simple algorithm (without density compensation):
  std::vector<float> t(N), x(N), y(N), s(N);
  float dx, dy, scaler = (1.f / (N-1));
  float length = 0.f;  // accumulated arc-length estimate
  t[0] = 0;
  x[0] = PL::evaluate(0, a, 3); // use optimized evaluateCubic (maybe inlined)
  y[0] = PL::evaluate(0, b, 3);
  for(int n = 1; n < N; n++) {
    t[n] = scaler * n;  // == n / N
    x[n] = PL::evaluate(t[n], a, 3);
    y[n] = PL::evaluate(t[n], b, 3);
    dx   = x[n]-x[n-1];
    dy   = y[n]-y[n-1];
    length += sqrt(dx*dx+dy*dy);
  }

  cubicSplineArcLength2D(&a[0], &b[0], &t[0], &s[0], N);
  // actually, we could try to use a different (less dense) t-array here, the s-array must then 
  // also be shorter - this number should be a second parameter

  GNUPlotter plt;
  //plt.addDataArrays(N, &x[0], &y[0]); // the actual spline curve
  plt.addDataArrays(N, &t[0], &s[0]);   // arc-length s as function of parameter t
  //plt.plot();


  float splineLength = s[N-1]; // last value in s is total length: s(t=1)
  int numSplineDots = std::max(1, (int)round(splineLength * density));


  rsImageF image(width, height);
  rsImagePainter<float, float, float> painter(&image);
  //painter.setAntiAlias(false);
  //painter.drawDottedSpline1(a, b, 1.0, 1.0, numSplineDots); // no density compensation
  //painter.drawDottedSpline2(a, b, 1.0, 1.0); // removed
   

  writeImageToFilePPM(*painter.getImage(), "CubicSpline.ppm");
  int dummy = 0;
}

void triangles()
{
  // create and set up objects and parameters:
  typedef rsVector2DF Vec2;    // for convenience
  typedef rsVector2DF V;       // even shorter - for inlined constructor calls
  float c = 1.0f;              // color (gray value)
  rsImageF img(35, 20);        // image to draw on
  rsImageDrawerFFF drw(&img);  // drawer object
  drw.setBlendMode(rsImageDrawerFFF::BLEND_ADD_CLIP);

  void (*pDrawTriangle)(rsImageDrawerFFF&, const Vec2&, const Vec2&, const Vec2&, float);
  pDrawTriangle = &drawTriangle;
  //pDrawTriangle = &drawTriangleAntiAliasedProto;

  //void drawTriangle(rsImageDrawerFFF& drw, 
  //  const rsVector2DF& v0, const rsVector2DF& v1, const rsVector2DF& v2, float color);

  // parallelogram made from a flat-top and flat-bottom triangle:
  Vec2 
    p1(  2.f,  1.f),  // V(2,1) 
    p2(  7.f,  1.f),  // V(7,1)
    p3(  4.f,  5.f),  // V(4,5)
    p4(  9.f,  5.f);  // V(9,5)
  pDrawTriangle(drw, p1, p2, p3, c);  // flat-top
  pDrawTriangle(drw, p2, p4, p3, c);  // flat-bottom
  //drawTriangleFlatTop(   drw, p1, p2, p3, c);  // p1,p2 above p3, clockwise
  //drawTriangleFlatBottom(drw, p2, p3, p4, c);  // p2 above p3,p4, counterclockwise

  // a general triangle:
  pDrawTriangle(drw, V(25,1), V(31,10), V(17,5), c);

  // polygon from 3 general triangles:
  pDrawTriangle(drw, V( 6, 7), V(15, 8), V( 9,10), c);
  pDrawTriangle(drw, V(15, 8), V(14,13), V( 9,10), c);
  pDrawTriangle(drw, V( 6, 7), V( 9,10), V( 3,11), c);

  // polygon from 7 triangles:
  pDrawTriangle(drw, V(17,12), V(15,17), V(17,16), c);
  pDrawTriangle(drw, V(17,12), V(17,16), V(20,17), c);
  pDrawTriangle(drw, V(19,11), V(17,12), V(20,17), c);
  pDrawTriangle(drw, V(19,11), V(20,17), V(23,12), c);
  pDrawTriangle(drw, V(23,12), V(20,17), V(23,15), c);
  pDrawTriangle(drw, V(23,12), V(23,15), V(26,14), c);
  pDrawTriangle(drw, V(26,14), V(23,15), V(24,17), c);

  // polygon from 5 triangles:
  pDrawTriangle(drw, V( 1,13), V(3,17), V( 7,13), c);
  pDrawTriangle(drw, V( 7,13), V(3,17), V(11,13), c);
  pDrawTriangle(drw, V(11,13), V(3,17), V( 9,16), c);
  pDrawTriangle(drw, V(11,13), V(9,16), V(11,16), c);
  pDrawTriangle(drw, V( 3,17), V(7,19), V( 9,16), c);

  // half-pixel mini triangles:
  pDrawTriangle(drw, V(27,12), V(27,13), V(28,12), c); // top left
  pDrawTriangle(drw, V(29,12), V(29,13), V(30,13), c); // bottom left
  pDrawTriangle(drw, V(31,12), V(32,13), V(32,12), c); // top right
  pDrawTriangle(drw, V(34,12), V(33,13), V(34,13), c); // bottom right

  // small "arrow-head":
  pDrawTriangle(drw, V(29,15), V(30,17), V(30,16), c);
  pDrawTriangle(drw, V(29,15), V(30,16), V(31,16), c);

  // for copy/paste:
  //pDrawTriangle(drw, V(,), V(,), V(,), c);

  // when assuming a downward y-axis, alway start at the top vertex (in case of a flat-top, at
  // the top-left) and traverse vertices in counterclockwise order

  // save to file:
  //writeImageToFilePPM(img, "PolygonsViaTriangles.ppm");
  writeScaledImageToFilePPM(img, "PolygonsViaTriangles.ppm", 16);
    // todo: write a function that includes a magnification factor (or maybe two, for x and y
    // separately)
}

void pixelCoverage()
{
  typedef rsVector2DF Vec2;    // for convenience
  typedef rsVector2DF V;       // even shorter - for inlined constructor calls

  Vec2 a(1.5f, 1.5f), b(-1.0f, 0.25f), c(2.0, -0.75);

  float x = 0, y = 0;

  float cov = pixelCoverage2(x, y, a, b, c);

  cov = pixelCoverage2(x, y, V(2,1), V(-1,0.25f), V(2.5f,-0.25f));

  int dummy = 0;
}

// idea for contour drawing:
// -input: image the function values, array of levels
// -output: image with the level lines / contours
//


void getContourSubPixelPosition1(float z00, float z01, float z10, float z11, float c,
  float* x, float* y)
{
  // average value along pixel borders:
  float L = (z00 + z01) * 0.5f;  // left
  float R = (z10 + z11) * 0.5f;  // right
  float T = (z00 + z10) * 0.5f;  // top
  float B = (z01 + z11) * 0.5f;  // bottom

  // x-offset is determined by where the level would fall between L and R:
  //*x = ((L+R) * 0.5 - c) / (R-L);  // is that correct? what about div-by-zero
  // sometimes |x| > 1 - that should not happen - actually it should be always in the 
  // range 0...1 - so that formula is not yet correct

  //if(L < R)
  //  *x = rsLinToLin(c, L, R, 0.f, 1.f);
  //else
  //  *x = 1.f - rsLinToLin(c, R, L, 0.f, 1.f);  // is that correct?

  *x = rsLinToLin(c, L, R, 0.f, 1.f);
  *y = rsLinToLin(c, T, B, 0.f, 1.f); // top comes before bottom - y increases downward

  // output may go negative - maybe we have to do cases if(L < R) .. else ...
}
// this doesn't work

void getContourSubPixelPosition2(float z00, float z01, float z10, float z11, float c,
  float min, float max, float* x, float* y)
{
  z00 -= c;
  z01 -= c;
  z10 -= c;
  z11 -= c;

  //float zAvg = 0.25f * (z00 + z01 + z10 + z11); 
    // optimize: delete the -= and do zAvg = 0.25f * (z00 + z01 + z10 + z11 - 4*c);

  float L = (z00 + z01) * 0.5f;  // left
  float R = (z10 + z11) * 0.5f;  // right
  float T = (z00 + z10) * 0.5f;  // top
  float B = (z01 + z11) * 0.5f;  // bottom

  *x = rsLinToLin(c, L, R, 0.f, 1.f);
  *y = rsLinToLin(c, T, B, 0.f, 1.f);
}
// not yet tested

void getContourSubPixelPosition3(float z00, float z01, float z10, float z11, float c,
  float* x, float* y)
{
  // this is how the function values are distributed over the pixels: z00 is the value at the pixel
  // under investigation and z01, z10, z11 are bottom, right and diagonal neighbours (in that 
  // order):
  // z00--z10
  //  |    |
  // z01--z11
  // oh - wait - no: z10 should be bottom - or not? no - the first index is the x-coordinate, so 
  // it's right

  int branch = 0;  // for debug

  float x0, x1, y0, y1;
  if((z00 < c && z01 >= c) || (z00 >= c && z01 < c)) // check left border
  {
    // segment goes through left border - we put the first point (x0,y0) there:
    x0 = 0.f;
    y0 = rsLinToLin(c, z00, z01, 0.f, 1.f);
    if((z00 < c && z10 >= c) || (z00 >= c && z10 < c)) // check top border
    {
      branch = 1;
      // segment goes through top border - we put the second point (x1,y1) there:
      x1 = rsLinToLin(c, z00, z10, 0.f, 1.f);
      y1 = 0.f;
    }
    else if((z01 < c && z11 >= c) || (z01 >= c && z11 < c))
    {
      branch = 2;
      // segment goes through bottom border
      x1 = rsLinToLin(c, z01, z11, 0.f, 1.f);
      y1 = 0.f;
    }
    else
    {
      branch = 3;
      // segment goes through right border (i.e. is horizontalish):
      x1 = 1.f;
      y1 = rsLinToLin(c, z10, z11, 0.f, 1.f);
    }
  }
  else
  {
    // z00--z10
    //  |    |
    // z01--z11

    // segment does not go through left border - so it must start either at top or bottom border

    if((z00 < c && z10 >= c) || (z00 >= c && z10 < c)) 
    {
      // segment goes through the top border
      x0 = rsLinToLin(c, z00, z10, 0.f, 1.f);
      y0 = 0.f;

      if((z10 < c && z11 >= c) || (z10 >= c && z11 < c))
      {
        branch = 4;
        // segment goes through right border
        x1 = 1.f;
        y1 = rsLinToLin(c, z10, z11, 0.f, 1.f);
      }
      else
      {
        branch = 5;
        // segment goes through bottom border (is verticalish)
        x1 = rsLinToLin(c, z01, z11, 0.f, 1.f);
        y1 = 1.f;
      }

    }
    else
    {
      branch = 6;
      // segment does not go through top border - so it must go through the bottom and then right
      x0 = rsLinToLin(c, z01, z11, 0.f, 1.f);  // bottom
      y0 = 1.f;
      x1 = 1.f;
      y1 = rsLinToLin(c, z10, z11, 0.f, 1.f);  // right
    }
  }

  // test - invert:
  //x0 = 1-x0; y0 = 1-y0; x1 = 1-x1; y1 = 1-y1; // nope!
  // putting 1-rsLinToLin everywhere also does not help - it actually makes things worse

  // evaluate line equation at midpoint - this gives the center of the segment
  *x = x0 + 0.5f * x1;
  *y = y0 + 0.5f * y1;

  // sanity check - this actually triggers - why?
  //rsAssert(rsIsInRange(*x, 0.f, 1.f));
  //rsAssert(rsIsInRange(*y, 0.f, 1.f));
  // seems to trigger in braches 3..6 - i've not yet seen it in 1 or 2 but didn't check 
  // exhaustively
}
// this is still wrong! it sometimes p

// optimize the calls to rsLinToLin to get rid of divisions where possible - keep this code as 
// prototype for unit testing the optimized code

void drawContour(const rsImageF& z, float level, rsImageF& target)
{
  rsImagePainter<float, float, float> painter(&target);
  // maybe pass this object from outside, also take a color - or maybe the color should be 
  // part of the state of the painter

  bool antiAlias = true; // make parameter

  for(int i = 0; i < z.getWidth()-1; i++) {
    for(int j = 0; j < z.getWidth()-1; j++) {
      float z00 = z(i,   j  );
      float z01 = z(i,   j+1);
      float z10 = z(i+1, j  );
      float z11 = z(i+1, j+1);
      float min = rsMin(z00, z01, z10, z11);
      float max = rsMax(z00, z01, z10, z11);
      if(min < level && max >= level) {
        float x, y;
        //getContourSubPixelPosition1(z00, z01, z10, z11, level, &x, &y); // not yet working
        //getContourSubPixelPosition2(z00, z01, z10, z11, min, max, level, &x, &y);

        x = 0.f; y = 0.f; // preliminary
        if(antiAlias)
          getContourSubPixelPosition3(z00, z01, z10, z11, level, &x, &y);

        painter.paintDot(float(i) + x, float(j) + y, 0.5f); 
      }
    }
  }

  // make anti-aliasing optional

  // todo: figure out subpixel location and draw anti-aliased
  // -there's a contour segment that passes through this pixel 
  // -we approximate it by a line segment
  // -we draw the pixel a the center of the line segment
  // ->figure out the line equation in parametric form
  // ->figure out location for parameter t = 0.5

  // or simpler:
  // -compute z0 = (z00 + z01) / 2, z1 = (z10 + z11) / 2
  //  z0 is the average value on the left, z1 on the right
  // -the x-value/offset is determined by how much this average is above/below the target
  //  level
  // -similar for y
  // -or is it the other way around?
  // -might be even better than the center of the line 
}

rsImageF getContours(const rsImageF& z, const std::vector<float> levels)
{
  rsImageF c(z.getWidth(), z.getHeight());
  for(size_t i = 0; i < levels.size(); i++)
    drawContour(z, levels[i], c);
  return c;
}

void normalize(rsImageF& img)
{
  float* p = img.getPixelPointer(0, 0);
  int N = img.getNumPixels();
  float min = rsArrayTools::minValue(p, N);
  float max = rsArrayTools::maxValue(p, N);
  float scl = 1.f / (max-min);
  for(int i = 0; i < N; i++)
    p[i] = scl * (p[i] - min);
}
// maybe make member

// this *may* be better numerically (less prone to roundoff errors) - needs test
void normalize2(rsImageF& img)
{
  float* p = img.getPixelPointer(0, 0);
  int N = img.getNumPixels();
  float min = rsArrayTools::minValue(p, N);
  for(int i = 0; i < N; i++)
    p[i] -= min;
  float max = rsArrayTools::maxValue(p, N);
  for(int i = 0; i < N; i++)
    p[i] /= max;
}

void contours()
{
  // We plot the 2D function z = f(x,y) = x^2 - y^2 into an image where the height translates
  // to the pixel brightness

  int w = 129;               // width in pixels
  int h = 129;               // height in pixels

  //w = h = 513;

  // test - turn into unit-test
  float x,y;
  getContourSubPixelPosition1(6.f, 2.f, 8.f, 4.f, 5.f, &x, &y); // 0.5, 0.5 - good!
  getContourSubPixelPosition1(6.f, 0.f, 8.f, 2.f, 5.f, &x, &y); // bad
  // try 10,10,10,0  2


  float r = 8;
  float xMin = -r;
  float xMax = +r;
  float yMin = -r;
  float yMax = +r;

  // create image with function values:
  rsImageF imgFunc(w, h);
  for(int i = 0; i < w; i++) {
    for(int j = 0; j < h; j++) {
      float x = xMin + i * (xMax-xMin) / (w-1);
      float y = yMin + j * (yMax-yMin) / (h-1);
      //float z = x*x - y*y;            // make this more flexible - use a lambda function
      //float z = x*x - y*y + 2*x*y;

      float z = x*sin(y) + y*cos(x) + 0.1*x*y;

      //float z = x*x + y*y;  // contours are circles

      imgFunc.setPixelColor(i, j, z);
    }
  }
  normalize2(imgFunc);

  // create image with contours:
  std::vector<float> levels = rsRangeLinear(0.f, 1.f, 8);
  rsImageF imgCont = getContours(imgFunc, levels); 


  // todo: 
  // -make a composited image with function values and contours
  // -maybe use the color-channels to plot more than one function 
  //  -plot complex functions - real -> red, imag -> green or blue
  // -could this be used as a drawing primitive? it can draw circles, for example - but it's 
  //  very inefficient - can this be optimized into a reasonable implicit curve drawing algo?


  writeScaledImageToFilePPM(imgFunc, "Function.ppm", 2);
  writeScaledImageToFilePPM(imgCont, "Contours.ppm", 2);
  // the right column and bottom row has no countour values - no surprise - the loop only goes up 
  // to w-1,h-1
  // maybe use powers of two +1 for the size and cut off bottom-row and right-column aftewards
}

// implicit curve drawing algo: 
// input: f(x,y) = c, f as functor, c as value, one solution x0,y0 that solves the equation
//
// (1) draw/paint pixel at x0,y0
// (2) find neighbouring pixel to draw next among the 8 neighbours
// (3) fix one coordinate and solve for the other by 1D root-finding
// (3) check, if we are back at the same pixel where we started or reached the image boundary 
//     -if yes: return - we are done with the curve
// (4) back to 1
//
// -maybe fill the shape by coloring all pixels for which f(x,y) < c
// -which should we increment or decrement and for which should we solve? x0 or y0? maybe that 
//  should also be decided based on a condition

// maybe make animations with
// http://www.softpedia.com/get/Multimedia/Graphic/Graphic-Others/APNG-Anime-Maker.shtml
