#include "ImageUnitTests.h"

using namespace RAPT;

void fillWithCheckerBoardPattern(rsImageF& image)
{
  image.clear();
  for(int x = 0; x < image.getWidth(); x++) {
    for(int y = 0; y < image.getHeight(); y++) {
      if( (x+y) % 2 == 0) // x+y even -> fill pixel
        image(x, y) = 1;
    }
  }
}

void fillWithCross(rsImageF& image)
{
  image.clear();
  int cx = image.getWidth()  / 2;
  int cy = image.getHeight() / 2;
  for(int x = 0; x < image.getWidth(); x++)
    image(x, cy) = 1;
  for(int y = 0; y < image.getHeight(); y++)
    image(cx, y) = 1;
}

bool testAlphaMaskPaintingAntiAliased(rsImagePainterFFF& painter, float x, float y)
{
  // We clear the image and paint a "dot" into the image (that is pointed to in the passed 
  // ImagePainter) and then check, if all pixels outside the rectangle of the dot are still
  // zero after that.
  // todo: maybe check also, if the pixels inside the rectangle have expected values
  // ...but this is complicated, maybe first, we should check if they are nonzero (but this 
  // assumes that the dot mask is nonzero everywhere) - maybe we can do it by providing a prototype
  // paint function that loops through the source pixels in the mask (instead of target pixels in 
  // the image), computes the non-integer position where to put the pixel and calla 
  // plotPixel(float x, float y) function for each target point, which performs bounds checking
  // internally - and then compare the pictures drawn with the regular and prototype function.

  bool result = true;

  // initializations:
  int wi, hi, wm, hm, xs, xe, ys, ye;
  rsImageF     *img = painter.getImage();
  rsAlphaMaskF *msk = painter.getAlphaMask();
  wi = img->getWidth();       // image width
  hi = img->getHeight();      // image height
  wm = msk->getWidth();       // mask width
  hm = msk->getHeight();      // mask height
  xs = (int)floor(x-0.5*wm);  // x start (of possibly nonzero values)
  xe = (int)ceil( x+0.5*wm);  // x end
  ys = (int)floor(y-0.5*hm);  // y start
  ye = (int)ceil( y+0.5*hm);  // y end

  // clear the image and paint the dot:
  img->clear();
  painter.paintDotViaMask(x, y, 1.f);

  // loop through the pixels of the image and check if those which should still be zero are zero 
  // indeed:
  for(int yi = 0; yi < hi; yi++)
  {
    for(int xi = 0; xi < wi; xi++)
    {
      if(xi < xs || xi > xe || yi < ys || yi > ye)
        result &= (*img)(xi, yi) == 0.f;
      else
      {
        //result &= (*img)(xi, yi) != 0.f;
        // whether or not the pixel should be nonzero depends on the mask ...maybe we could figure
        // out the correct target value and check against that...
      }
    }
  }

  return result;
}


bool imagePainterUnitTest()
{
  bool result = true;

  rsImageF image;
  rsAlphaMaskF mask;  // maybe use a regular image as mask
  rsImagePainterFFF painter(&image, &mask);


  int imageWidth  = 30;
  int imageHeight = 30; // 50x50 image with 3x3 mask gives an access violation
  int maskSize    = 7;

  // maybe, we should 1st use the simplest case: 1x1 mask

  image.setSize(imageWidth, imageHeight);
  image.clear();

  mask.setSize(maskSize);
  result &= mask.getWidth()  == maskSize;
  result &= mask.getHeight() == maskSize;
  //fillWithCheckerBoardPattern(mask);
  //fillWithCross(mask);
  mask.fillAll(1.f);  // full white

  //painter.paintDotViaMask(0.25f, 0.75f, 1);
  //painter.paintDotViaMask(2.25f, 3.75f, 1);
  //painter.paintDotViaMask(3.25f, 3.75f, 1);

  // draw in center and at all 4 edges:
  float dx = 0.5;
  float dy = 0.5;
  float w  = (float)imageWidth;
  float w2 = w/2;
  float h  = (float)imageHeight;
  float h2 = h/2;
  float b  = 0.75f; // brightness

  // do unit tets for various cases:
  dx = 0.5;
  dy = 0.5;
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx,     dy);   // top-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx,     dy);   // top-center
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx,     dy);   // top-right
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx, h2 +dy);   // center-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx, h2 +dy);   // center-right
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx, h-1+dy);   // bottom-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx, h-1+dy);   // bottom-center
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx, h-1+dy);   // bottom-right
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx, h2 +dy);   // center
  rsAssert(result);

  // try some cases where the dot is painted outside the image:
  dx = -3.0; // when the mask size is 5, -3.0 still works, but -3.1 doesn't
  dy = -3.0;
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx,     dy);   // top-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx,     dy);   // top-center
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx,     dy);   // top-right
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx, h2 +dy);   // center-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx, h2 +dy);   // center-right
  result &= testAlphaMaskPaintingAntiAliased(painter,     dx, h-1+dy);   // bottom-left
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx, h-1+dy);   // bottom-center
  result &= testAlphaMaskPaintingAntiAliased(painter, w-1+dx, h-1+dy);   // bottom-right
  result &= testAlphaMaskPaintingAntiAliased(painter, w2 +dx, h2 +dy);   // center
  rsAssert(result);



  // paint some dots and write image to file for visual inspection:
  dx = 0.5;
  dy = 0.5;
  image.clear();
  painter.paintDotViaMask(    dx,     dy, b);   // top-left
  painter.paintDotViaMask(w2 +dx,     dy, b);   // top-center
  painter.paintDotViaMask(w-1+dx,     dy, b);   // top-right
  painter.paintDotViaMask(    dx, h2 +dy, b);   // center-left
  painter.paintDotViaMask(w-1+dx, h2 +dy, b);   // center-right
  painter.paintDotViaMask(    dx, h-1+dy, b);   // bottom-left
  painter.paintDotViaMask(w2 +dx, h-1+dy, b);   // bottom-center
  painter.paintDotViaMask(w-1+dx, h-1+dy, b);   // bottom-right
  painter.paintDotViaMask(w2 +dx, h2 +dy, b);   // center

  //painter.paintDotViaMask(10.2f, 10.6f, 1);
  // we use the 1000 here, because the painter uses this strange saturating function - maybe, we 
  // should introduce a blend-mode: mix, add, add-and-clip, add-and-saturate, multiply, ...
  // for testing here, we should use either alpha-blend or add-and-clip (should give same results)

  //writeImageToFilePPM(mask,  "PaintTestMask.ppm");
  //writeImageToFilePPM(image, "PaintTestImage.ppm");

  // ...

  return result;
}

bool triangleRasterizationUnitTest()
{
  // We have 3 versions of the triangle drawing function - the naive, slow version, and the 
  // box-based and span-based optimizations - we check here, if the optimized versions produce the 
  // same results as the naive version.

  bool r = true;
  int numTriangles = 1; // it takes quite a lot of time to draw triangles, so we use a small number

  // create and set up images and drawer objects:
  int w = 20;
  int h = 20;
  rsImageF img1(w, h), img2(w, h), img3(w, h);
  rsImageDrawerFFF drw1(&img1), drw2(&img2), drw3(&img3);
  drw1.setBlendMode(rsImageDrawerFFF::BLEND_ADD_CLIP);
  drw2.setBlendMode(rsImageDrawerFFF::BLEND_ADD_CLIP);
  drw3.setBlendMode(rsImageDrawerFFF::BLEND_ADD_CLIP);

  // random number generators for the triangle vertices and color:
  rsNoiseGeneratorF coordinateGenerator;
  coordinateGenerator.setRange(-10, 30);
  rsNoiseGeneratorF colorGenerator;
  colorGenerator.setRange(0, 1);

  // draw triangles using the different functions and compare results:
  float color;
  rsVector2DF a, b, c;
  for(int i = 1; i <= numTriangles; i++)
  {
    // we clear because otherwise, it's likely that all pixel soon saturate to white:
    img1.clear();
    img2.clear();
    img3.clear();

    // produce a random triangle with random color:
    a.x = coordinateGenerator.getSample();
    a.y = coordinateGenerator.getSample();
    b.x = coordinateGenerator.getSample();
    b.y = coordinateGenerator.getSample();
    c.x = coordinateGenerator.getSample();
    c.y = coordinateGenerator.getSample();
    color = colorGenerator.getSample();

    // draw it with the different versions of the drawing
    // maybe draw only if the triangle is counterclockwise? hmm...
    drawTriangleAntiAliasedProto(   drw1, a, b, c, color);
    drawTriangleAntiAliasedBoxBased(drw2, a, b, c, color);

    drawTriangleAntiAliasedSpanBased(drw3, a, b, c, color);
    // bottom scanline looks wrong


    writeScaledImageToFilePPM(img1, "TriangleNaive.ppm", 16);
    writeScaledImageToFilePPM(img2, "TriangleBoxOptimization.ppm", 16);
    writeScaledImageToFilePPM(img3, "TriangleSpanOptimization.ppm", 16);



    // compare drawing results:
    r &= img2.areAllPixelsEqualTo(&img1);
    r &= img3.areAllPixelsEqualTo(&img1);
    int dummy = 0;
  }

  //writeImageToFilePPM(img1, "RandomTriangles.ppm");
  return r;
}