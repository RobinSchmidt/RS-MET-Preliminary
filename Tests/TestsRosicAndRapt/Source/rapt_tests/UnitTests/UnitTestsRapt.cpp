#include "UnitTests.h"

using namespace RAPT;

bool runUnitTest(bool (*test)(), const string& name)
{
  //cout << "Testing: " + name + ": ";
  cout << name + ": ";
  bool passed = test();
  //rsAssert(passed); // break, if test fails
  if(passed)
    cout << "Passed\n";
  else
    cout << "!!!!----> F A I L E D <----!!!!\n";
  return passed;
}

bool runAllUnitTests()
{
  bool passed = true;  // test result

  // Data:
  passed &= runUnitTest(&arrayUnitTest,            "rsArray and std::vector stuff");
  passed &= runUnitTest(&testBufferFunctions,      "BufferFunctions");  // merge with rsArray tests
  passed &= runUnitTest(&testSortAndSearch,        "SortAndSearch");
  passed &= runUnitTest(&doubleEndedQueueUnitTest, "rsDoubleEndedQueue");   // fails! -> fix that!
  passed &= runUnitTest(&float64x2UnitTest,        "rsFloat64x2");
  passed &= runUnitTest(&float32x4UnitTest,        "rsFloat32x4");
  passed &= runUnitTest(&complexFloat64x2UnitTest, "std::complex<rsFloat64x2>");
    // fails on linux ("illegal instruction") ...seems that illegal instruction is our
    // rsAsserFalse debug-break

  // Math:
  passed &= runUnitTest(&coordinateMapperUnitTest,       "rsCoordinateMapper2D");
  passed &= runUnitTest(&fitRationalUnitTest,            "fit rational");   // fails on linux ("illegal instruction") - encounters singular matrix
  passed &= runUnitTest(&interpolatingFunctionUnitTest,  "rsInterpolatingFunction");
  passed &= runUnitTest(&resampleNonUniform,             "resampleNonUniform");
  passed &= runUnitTest(&rootFinderUnitTest,             "rsRootFinder");
  passed &= runUnitTest(&correlationUnitTest,            "correlation");
  passed &= runUnitTest(&testVector,                     "rsVector");
  passed &= runUnitTest(&testMatrix,                     "rsMatrix");
  passed &= runUnitTest(&testMiscMath,                   "misc math");  // fails on linux ("illegal instruction") - encounters a singular matrix
  passed &= runUnitTest(&testLinearAlgebra,              "rsLinearAlgebra");  // fails on linux ("illegal instruction")
  passed &= runUnitTest(&testPolynomial,                 "rsPolynomial");
  //passed &= runUnitTest(&polynomialRootsUnitTest,       "rsPolynomial: root finding"); // absorb in rsPolynomial
  passed &= runUnitTest(&testDifferentialEquationSystem, "rsDifferentialEquationSystem");
  passed &= runUnitTest(&testIntegerFunctions,           "integer functions");
  passed &= runUnitTest(&testMoebiusTransform,           "rsMoebiusTransform");
  passed &= runUnitTest(&testNumberTheory,               "number theory");
  passed &= runUnitTest(&testRealFunctions,              "real functions");
  passed &= runUnitTest(&testTransforms,                 "transforms");
  passed &= runUnitTest(&testTriangle2D,                 "rsTriangle2D");
  passed &= runUnitTest(&testPoint2D,                    "rsPoint2D");
  passed &= runUnitTest(&testPolygon2D,                  "rsPolygon2D");
  passed &= runUnitTest(&testMultiArray,                 "rsMultiArray");


  // Filters:
  //passed &= runUnitTest(&prototypeDesignUnitTest, "rsPrototypeDesigner"); // why commented?
  passed &= runUnitTest(&filterSpecUnitTest,    "rsFilterSpecification (BA/ZPK)");
  passed &= runUnitTest(&movingMaximumUnitTest, "moving maximum filter");


  // Visualization:
  passed &= runUnitTest(&imagePainterUnitTest,   "rsImagePainter");
  passed &= runUnitTest(&triangleRasterization,  "Triangle Rasterization");
  //passed &= runUnitTest(&triangleRasterization2, "Triangle Rasterization 2"); // merge

  // Misc:
  passed &= runUnitTest(&blepUnitTest,  "Blit/Blep/Blamp");  // move to generator unit tests
  passed &= runUnitTest(&syncUnitTest,  "Osc Sync");         // dito

  passed &= runUnitTest(&spectrogramUnitTest,   "rsSpectrogram");
  passed &= runUnitTest(&sineModelingUnitTest,  "SineModeling");





  //...
  //...more to come...

  return passed;
}
