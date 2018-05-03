#ifndef RAPT_PLOTTING_H
#define RAPT_PLOTTING_H

//#include "GNUPlotter.h"
#include "DSPPlotters.h"
using namespace std;  // try to get rid

///** Plots at most five y-functions against a common x-axis. */
//void plotData(int N, float *x, float *y1, float *y2 = nullptr, float *y3 = nullptr,
//  float *y4 = nullptr, float *y5 = nullptr);

/** Returns N samples of the impulse response of the passed filter as std::vector. It is necessary 
for you to pass a scale factor of the type of the filter's output signal (for example: double), 
such that the compiler can deduce the template parameter. We also use it to scale the impulse 
response, so it actually gets some purpose besides satisfying the compiler. The filter class must 
support the functions reset() and getSample() */
template<class TSig, class TFlt>
vector<TSig> impulseResponse(TFlt &filter, int length, TSig scale);
 // maybe move to a file ResponseGetters

/** Plots N samples of the impulse response of the passed filter. */
template<class TSig, class TFlt>
void plotImpulseResponse(TFlt &filter, int length, TSig scale);



// new, dragged over from RSLib tests (TestUtilities.h):

// convenience functions for interfacing with the Plotter class (they manage instantiation and 
// setup of Plotter objects for frequently used cases):
void plotData(int N, double *x, double *y1, double *y2 = NULL, double *y3 = NULL, 
  double *y4 = NULL, double *y5 = NULL);

void plotData(int N, double x0, double dx, double *y1, double *y2 = NULL, double *y3 = NULL, 
  double *y4 = NULL, double *y5 = NULL);
// for equidistant data, abscissa values start at x0 with increment dx

void plotDataLogX(int N, double *x, double *y1, double *y2 = NULL, double *y3 = NULL, 
  double *y4 = NULL, double *y5 = NULL);

void plotVector(std::vector<double> v);

/** Plots the magnitude spectrogram given in s against time axis t (of length numFrames) and
frequency axis f (of length numBins). */
void plotSpectrogram(int numFrames, int numBins, double **magnitudes, double sampleRate, 
  int hopSize);
// introduce parameters to control scaling of time- and frequency axis..



#endif