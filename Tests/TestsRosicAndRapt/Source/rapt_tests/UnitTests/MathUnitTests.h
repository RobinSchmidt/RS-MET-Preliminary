#ifndef RAPT_MATHUNITTESTS_H
#define RAPT_MATHUNITTESTS_H


// get rid of these includes:

//#include "../../Shared/Shared.h"
#include "Math/LinearAlgebraUnitTests.h"
#include "Math/PolynomialUnitTests.h"
#include "Math/VectorUnitTests.h"
#include "Math/MatrixUnitTests.h"
#include "Math/MiscMathUnitTests.h"

// under construction:
#include "Math/DifferentialEquationTests.h"
#include "Math/IntegerFunctionTests.h"
#include "Math/MoebiusTransformTests.h"
#include "Math/NumberTheoryTests.h"
#include "Math/RealFunctionTests.h"
#include "Math/TransformsTests.h"
//#include "Math/StatisticsTests.h"  // there is no such file - why?
#include "Math/Point2DTests.h"
#include "Math/Polygon2DTests.h"
#include "Math/Triangle2DTests.h"

#include "Math/MultiArrayTests.h"

bool coordinateMapperUnitTest();
bool correlationUnitTest();
bool fitRationalUnitTest();
bool interpolatingFunctionUnitTest();
bool resampleNonUniform();
bool rootFinderUnitTest();
bool polynomialRootsUnitTest(); // the new explicit formulas - move to PolynomialUnitTests



#endif