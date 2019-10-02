#pragma once

//-------------------------------------------------------------------------------------------------
// Experiments

// Filters Experiments:

void bandSplittingTwoWay();
void bandSplittingMultiWay();
void bandSplittingTreeAlgo();
void bandSplitFreqResponses();
void complementaryFiltersIIR();
void firstOrderFilters();
void ladderResonanceManipulation();
// insert movingAverage here
void nonUniformMovingAverage();
void nonUniformOnePole1();
void nonUniformOnePole2();
void nonUniformComplexOnePole();
void nonUniformAllpole();
void nonUniformBiquad();
void nonUniformBiDirectional();
void smoothingFilterOrders();
void smoothingFilterTransitionTimes();
void prototypeDesign();    // still using the old code
void poleZeroPrototype();  // new implementation


// Generator Experiments:

void blit();
void blep();
//void blamp();
void polyBlep();
void superBlep();
void superSawStereo();
void twoPieceOsc();
void syncSweep();
void syncPhasor();
void syncPhasor2();
void syncOsc();
void dualBlepOsc();
void bouncillator();
void bouncillatorFormula();
void freqVsPhaseMod();
void rayBouncer();
void circleFractals();
void hilbertCurve();
void lindenmayer();
void snowFlake();
void triSawOsc();
void triSawOscAntiAlias();
void xoxosOsc();


// Graphics Experiments:

void lineDrawing();
void lineDrawingThick();
//void lineDrawingThick2(); // obsolete
void lineJoints();
void lineTo();
void polyLineRandom();
void phaseScopeLissajous();
void splineArc();
void triangles();
void pixelCoverage();


// Math Experiements:

void ellipseLineIntersections();
void interpolatingFunction();
void linearRegression();
void polynomialPrediction();
void probabilityLogic();
void productLogPlot();
void ratioGenerator();
void ratiosLargeLcm();
void ratiosMetallic();
void sinCosTable();
void expBipolar();
void expGaussBell();
void twoParamRemap();
// move to something like FunExperiments (maybe copy the mandelbrot stuff over there, too)
bool groupString();
void primeAlternatingSums();
void divisibility();
void arithmeticDerivative();


// Modulator Experiments:

void attackDecayEnvelope();

//-------------------------------------------------------------------------------------------------
// Performance Tests

// Audio Performance Tests:

void filterSignConventionPerformance();
void ladderPerformance();
void stateVectorFilterPerformance();
void engineersFilterPerformance();
void turtleGraphicsPerformance();


// Math Performance Tests:

void fftPerformance();
void matrixAdressingTest();
//void simdPerformanceFloat64x2();

template<class TScalar, class TVector>
void simdPerformance(TScalar scl, TVector vec);

void sinCosPerformance();

// Misc Performance Tests:

void callbackPerformance();