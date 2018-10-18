#include "../JuceLibraryCode/JuceHeader.h"

// includes for unity build:
#include "Shared/Shared.h"
#include "Experiments/Experiments.h"
#include "UnitTests/UnitTests.h"
#include "PerformanceTests/PerformanceTests.h"
#include "Misc/Misc.h"  // demos, examples, rendering, ... // todo: make unity build cpp file

//#include "../../JUCE/
//#include "../../../Libraries/JUCE/modules/romos/TestSuite/TestsMain.h" // doesn't exist on new pc. forgotten to add to the repo?
//#include "../../../Libraries/JUCE/modules/romos/TestSuite/TestsMain.cpp"
//#include "../../../Libraries/JUCE/modules/romos/romos.h"

// the files
// romos/TestSuite/TestHelpers.cpp and
// rapt_tests/UnitTests/Math/DifferentiaEquationTests.cpp
// oth define arrays x,y in the global namespace which clash. For some reason, only gcc complains
// about this. solution: wrap into namespace

int main(int argc, char* argv[])
{

  // tempoarary throw-away-code:
  testCrossoverNewVsOld();

  //===============================================================================================
  // RAPT tests:

  //-----------------------------------------------------------------------------------------------
  // Unit tests:

  runAllUnitTests();  // todo: merge with unit tests for RSLib

  //mathUnitTests();    // doesn't exist anymore ...it's all in runAllUnitTests now
  //filterUnitTests();  // dito (?)


  //-----------------------------------------------------------------------------------------------
  // Performance tests:

  //callbackPerformance();
  //matrixAdressingTest();
  //simdPerformanceFloat64x2();
  //sinCosPerformance();

  //fftPerformance();
  //filterSignConventionPerformance();
  //ladderPerformance();
  //engineersFilterPerformance();
  //turtleGraphicsPerformance();

  //-----------------------------------------------------------------------------------------------
  // Experiments:

  // Prototypes (not yet in library):
  //particleBouncerExperiment();

  // Math:
  //ellipseLineIntersections();
  //expBipolar();
  //expGaussBell();
  //interpolatingFunction();
  //linearRegression();
  //probabilityLogic();
  //productLogPlot();
  //sinCosTable();
  //twoParamRemap();

  // Filter:
  //bandSplittingTwoWay();
  //bandSplittingMultiWay();   // turn into unit test (it currently hits an assert on fail)
  //bandSplittingTreeAlgo();
  //bandSplitFreqResponses();
  //complementaryFiltersIIR();
  //firstOrderFilters();
  //ladderResonanceManipulation();
  //nonUniformMovingAverage();
  //smoothingFilterOrders();
  //smoothingFilterTransitionTimes();
  //prototypeDesign();  // old implementation - tddo: check gains of prototype filters
  //poleZeroPrototype();  // new implementation - but we don't need that

  // Physics:
  //particleForceDistanceLaw();
  //particleSystem();

  // Generators:
  //bouncillator();
  //bouncillatorFormula();
  //freqVsPhaseMod();
  //rayBouncer();
  //hilbertCurve();
  //circleFractals(); // rename to spirograph
  //lindenmayer();
  //triSawOsc();
  //triSawOscAntiAlias();
  //xoxosOsc();

  // Graphics:
  //lineDrawing();
  //lineDrawingThick();
  ///lineDrawingThick2(); // obsolete
  //lineJoints();
  //lineTo();
  //polyLineRandom();
  //phaseScopeLissajous();
  //splineArc();
  //triangles();
  //pixelCoverage();


  // just for fun:
  //groupString();
  //primeAlternatingSums();
  divisibility();


  //===============================================================================================
  // Tests for dragged over RSLib code:

  //int dummy;
  //std::string str;
  //bool passed = true;

  //-----------------------------------------------------------------------------------------------
  // Unit Tests:

  //passed &= testBufferFunctions(str);
  //passed &= testCopySection(    str);
  //passed &= testMoveElements(   str);
  //passed &= testRemoveElements( str);


  //passed &= testFilterPolynomials(str);

  //passed &= testHighOrderFilter(  str);  // fails


  ////passed &= testModalFilter2(str);       // fails
  ////passed &= testModalSynth(str);         // triggers assert

  ////passed &= testNumberManipulations( str); // triggers assert (calls the two below)
  ////passed &= testDoubleIntConversions(str); // triggers same assert (called by function above)
  //passed &= testExponentExtraction(str);

  //passed &= testAutoCorrelationPitchDetector(str);

  //passed &= testSortAndSearch(str);          // calls the two below (redundant)
  //passed &= testHeapSort(str);
  //passed &= testKnuthMorrisPrattSearch(str);

  //passed &= testTypeSizes(str);

  //-----------------------------------------------------------------------------------------------
  // Experiments:

  // Math:
  //binomialDistribution();
  //sineParameters();
  //bandLimitedStep();
  //cubicInterpolationNonEquidistant();   // move to unit tests
  //hyperbolicFunctions();
//  splineInterpolationNonEquidistant();
  //rationalInterpolation();
//splineInterpolationAreaNormalized();
  //numericDiffAndInt();
  //shiftPolynomial();
  ////void stretchPolynomial();  // commented in header
  //monotonicPolynomials();
  //parametricBell();
  //partialFractionExpansion();
  //partialFractionExpansion2();
  //partialFractionExpansionQuadratic();
  //dampedSineEnergy();
  //sineIntegral();
  //logarithmQuotient();
  //stirlingNumbers();
  //bernoulliNumbers();
  //sequenceSquareRoot();
  //conicSystem();
  ////logisticMapNoise(); // takes long to compute
  //bigFloatErrors();
  //primeRecursion();
  ////primeSieveSchmidt1(); // crashes
  ////primeSieveSchmidt2(); // crashes
  //primeSieveAtkin();
  ////primeSieve();  // crashes
  //primeDistribution();
  ////numberTheoreticTransform(); // triggers assert

  // Analysis:
  //autoCorrelation();
  //autocorrelationPeakVariation();
  //autoCorrelationPitchDetector();
  //autoCorrelationPitchDetectorOffline();
  //crossCorrelationBestMatch();
  //combineFFTs(); // move to math experiments
  ////zeroCrossingPitchDetector(); // commented in header - why?
  //instantaneousFrequency();
  ////instantaneousPhase();  // triggers assert (there's something not yet implemented)
  //zeroCrossingFinder();
  //zeroCrossingFinder2();
  //zeroCrossingFinder3();
  //cycleMarkFinder();
  //cycleMarkErrors();
  ////zeroCrossingPitchDetector(); // triggers assert
  //zeroCrossingPitchDetectorTwoTones();

  // Delay:
  //basicIntegerDelayLine();

  // Filter:
  //bandwidthScaling();
  //stateVariableFilter();
  //stateVariableFilterMorph();
  //stateVectorFilter();   // just a stub, at the moment
  //biquadModulation();   // compares modulation properties of various biquad structures
  ////transistorLadder(); // triggers assert
  //phonoFilterPrototypePlot();
  //magnitudeMatchedOnePoleFilter();
  //phonoFilterModelPlot();
  //phonoFilterSimulation();
  //serialParallelBlend();
  //averager();
  ////movingAverage();  // hangs
  ////trapezAverager();   // hangs

  //gaussianPrototype();
  //halpernPrototype();
  //compareApproximationMethods();
  //compareOldAndNewEngineersFilter();
  //testPoleZeroMapper();

  //ringingTime();
  //butterworthSquaredLowHighSum();
  //maxFlatMaxSteepPrototypeM1N2();
  //maxFlatMaxSteepPrototypeM2N2();
  ////experimentalPrototypeM1N2();  // commented in header
  //splitLowFreqFromDC();
  //ladderResonanceModeling();
  //ladderResoShape();
  //ladderThresholds();           // maybe remove - this seemed to be a dead end
  //ladderFeedbackSaturation();
  //ladderFeedbackSaturation2();
  //ladderFeedbackSaturation3();
  //ladderFeedbackSatDCGain();
  //ladderFeedbackSatReso();
  //ladderFeedbackSatGrowl();
  //ladderFeedbackSatGrowl2();
  //ladderZDF();
  //ladderZDFvsUDF();   // compares modulation properties
  //resoShapeFeedbackSat();
  //resoSaturationModes();
  //resoShapeGate();
  //resoShapePseudoSync();
  //resoSeparationNonlinear();
  //resoReplace();
  //resoReplacePhaseBumping();
  //resoReplaceScream();
  //fakeResonance();
  //fakeResoLowpassResponse();
  //fakeResoDifferentDelays();

  // Modal Filters/Synthesis:
  //modalFilter();        // impulse response of decaying-sine filter
  //modalFilterFreqResp();  // frequency response of attack/decay-sine filter - rename
  //modalTwoModes();
  //attackDecayFilter();  // ...hmm..almost redundant
  //dampedSineFilterDesign();
  //biquadImpulseResponseDesign();
  //modalBankTransient();

  // Misc Audio:
  //centroid();
  //cubicCrossfade();
  //recursiveSineSweep();
  //ringModNoise();
  //slewRateLimiterLinear();
  //stretchedCorrelation();
  //taperedFourierSeries();
  //transientModeling();
  //windowFunctionsContinuous();
  //windowedSinc();

  // Modulator:
  //breakpointModulator();
  //breakpointModulatorSmoothFadeOut();
  //triSawModulator();

  // Oscillator:
  //triSaw();
  //phaseShapingCurvePoly4();
  //phaseShapingCurvesRational();
  //phaseShaping();
  //phaseShapingSkew();

  // Partial Extraction:
  //biDirectionalFilter();    // maybe move to filter tests
// envelopeDeBeating();
  //sineRecreation();         // maybe move elsewhere
  //sineWithPhaseCatchUp();   // dito
  //partialExtractionTriple();
  //partialExtractionViaBiquadTriple();
  ////partialExtractionBell();  // crashes because sample not available
  ////partialExtractionSample();  // dito

  // Phase Vocoder:
  //phaseRepresentation();
  //grainRoundTrip();        // under construction
  //plotWindows();
  //spectrogramSine();

  // Physics:
  //doublePendulum(); // takes long

  // Resampling:
  //fadeOut();  // move to a new file SampleEditingExperiments
  //resampler();
  //sincResamplerAliasing();
  //sincResamplerModulation();
  //sincResamplerPassbandRipple();
  //sincResamplerSumOfTapWeights();
  //timeWarp();
  //pitchDemodulation();
  //phaseLockedCrossfade();
  //phaseLockedCrossfade2();
  //sineShift();
  //sineShift2();
  //pitchDetectWithSilence();
  ////// tests with Elan's example files:
  ////pitchDetectA3();
  ////phaseLockSaxophone();
  ////phaseLockSaxophone2();
  ////autoTuneHorn();
  ////autoTuneHorn2();
  ////sylophoneCycleMarks();
  ////autoTuneSylophone();
  ////bestMatchShift();

  // Saturation:
  //powRatioParametricSigmoid();
  //parametricSigmoid();
  //parametricSigmoid2();
  //quinticParametricSigmoid();
  //septicParametricSigmoid();
  //saturator();
  //sigmoidScaleAndShift();
  //quarticMonotonic();
  //sigmoidPrototypes();
  //sixticPositive();

  //-----------------------------------------------------------------------------------------------
  // Performance Tests:

  // Analysis:
  //testFourierTransformer(str);
  //testAutoCorrelationPitchDetector2(str);

  // Core:
  //testFlagArray(str);

  // Math:
  //testAbsAndSign2(str);              // rename (mabye "Perf" or sth)
  //testMultinomialCoefficients2(str); // rename
  //testPrimeSieves(str);
  ////testMatrix(str);                 // triggers assert
  //testMatrixAddressing(str);

  // Misc Audio:
  //testSincInterpolator(str);

  // Modal:
  //testModalFilter2(str);
  //testModalFilterBank(str);

  //-----------------------------------------------------------------------------------------------
  // Examples:

  // Modal:
  //createInsertionSortSound();  // move somewhere else
  //createModalFilterExamples();
  //createModalFilterBankExamples(); // takes long
  // sample-map creations (they take long):
  //createBass1();
  //createGong1();
  //createPluck1();

  //===============================================================================================
  // RoSiC tests:

  //-----------------------------------------------------------------------------------------------
  // Unit tests:

  testAllRosicClasses();
  //testRosicAnalysis();
  //testRosicBasics();
  //testRosicFile();
  //testRosicEffects();
  //testRosicGenerators();
  //testRosicFilter();
  //testRosicNumerical();
  //testRosicMath();
  //testRosicNonRealTime();
  //testRosicOthers();

  //-----------------------------------------------------------------------------------------------
  // Experiments:

  // Analysis:

  // Basics:

  // Effects:
  //testFastGeneralizedHadamardTransform();
  //testFeedbackDelayNetwork();
  //testMultiComp();

  // File:

  // Filters:
  //testLadderFilter();
  //testModalFilter();
  //testModalFilterWithAttack();
  //testBiquadPhasePlot();
  //testFiniteImpulseResponseDesigner();
  //testConvolverPartitioned();
  //testFiniteImpulseResponseFilter();
  //testFilterAnalyzer();
  //testBiquadCascade();
  //testCrossover4Way();
  //testCrossover4Way2();
  //testSlopeFilter();
  //testPrototypeDesigner();
  //testLowpassToLowshelf();
  //testBesselPrototypeDesign();
  //testPapoulisPrototypeDesign();
  //testEngineersFilter();
  //testPoleZeroMapping();
  //highOrderFilterPolesAndZeros();

  // Genrators:
  //testOscillatorStereo();
  //testLorentzSystem();
  //testSnowflake();
  //testResetter();
  //testTurtleReverse();
  //testTurtleSource();

  //-----------------------------------------------------------------------------------------------
  // Demos:
  // ...


  //-----------------------------------------------------------------------------------------------
  // Rendering:
  // ...

  //===============================================================================================
  // Modular:

  //runModularUnitTests();
  //runModularPerformanceTests(true);  // produces a memleak
  //testModularCodeGenerator();
  //runModularInteractiveTests();  // triggers assert due to plotting code
  romos::moduleFactory.clearRegisteredTypes(); // avoids memleak in unit tests

  // important atomic modules for performance tests:
  // Biquad: pure code, atomic module, wired model
  // Phasor

  //runModularTests(); // we need to make a .h file with the declarations


  //DEBUG_HOOK;
  //int* test = new int;  // uncomment, to see, if it fires correctly
  if( detectMemoryLeaks() )
    std::cout << "\n\n!!! Memory leaks detected (pre exit of main()) !!! \n";
    //std::cout << "\n\n!!! Memory leaks detected !!! \n";
  // If memory leaks occur even though no objects are actually created on the heap, it could mean
  // that some class in a library module has a static data member that does a dynamic memory
  // allocation or some global object is created somewhere that dynamically allocates memory.

  // todo: fix the memory leak - i guess it's in rosic - test it by building the project with
  // rapt only - doesn't work - try to figure out, where heap memory is allocated..

  // set a debug-breakpoint _malloc_dbg in debug_heap.cpp and run the program - it gets called a
  // bunch of times from the startup code - skipping through these, later there will be a call
  // from the offending memory allocating code from our codebase
  // ..it seems to come from romos - compiling with rapt and rosic only doesn't produce memleaks

  // ok - its in:
  // void BlitIntegratorInitialStates::createStateValueTables()
  // deleteStateValueTables(); is called after the memleaks were detected hmmm..
  // solution: don't use global objects that freely lie around, instead use a
  // GlobalData class which is a singleton and encapsulates all sorts of data that should be
  // globally accessible
  // write a unit test for blit-saw (maybe there is one already) and change BlitSaw module to use
  // that - then, before checking for memory leak, call GlobalData::cleanUp (there may be a
  // GlobalData::initialize method as well that computes all the data/tables once and for all
  // or: allocate memory and compute the data-tables only when they are needed the first time
  // (lazy initialization)...can also be used for blep-tables, etc. - anything that needs globally
  // constant tables
  // maybe class ProcessingStatus can be extended for that - it would fit well there, too


  getchar();
  return(EXIT_SUCCESS);
}

// ToDo:
// -fix access violation in rsPrimeFactors - done
// -use more efficient implementation for rsPowInt
// -check that fabs or rsAbs is used everywhere where floating point numbers can occurr
//  -maybe use rsAbs preferably because it may also be used for modular integers