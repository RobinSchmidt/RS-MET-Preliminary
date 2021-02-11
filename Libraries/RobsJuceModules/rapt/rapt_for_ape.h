#ifndef RAPT_FOR_APE_H_INCLUDED
#define RAPT_FOR_APE_H_INCLUDED

/*
Include file for RAPT for use in APE (Audio Programming Environment). There, it has to be 
included as relative path from "Audio Programming Environment\includes", for example, like:

  #include "../../../../RS-MET/Libraries/RobsJuceModules/rapt/rapt_for_ape.h"
  
*/

/*
// These don't seem to be needed anymore because APE already includeds them by itself?:
#include <complex>       // included already by ctgmath ...but not on gcc/windows?
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <cstring>
#include <climits>
#include <atomic>
*/

#include "Basics/Basics.h"               // type definitions, constants, functions, etc.
#include "Data/Data.h"                   // data structures like arrays, lists, etc.
#include "Math/Math.h"                   // interpolation, transforms, linear algebra, numerical analysis, ...
#include "AudioBasics/AudioBasics.h"
//#include "Music/Music.h"               // scales (pitchToFreq, ..), time signatures, sequences, patterns, notes, MIDI, etc.
#include "Filters/Filters.h"             // butterworth, ladder, biquad, elliptic, SVF, etc.
#include "Analysis/Analysis.h"           // envelope follower, smoother, pitch-detector, etc.
#include "Physics/Physics.h"             // waveguides, finite difference approximations of PDEs, etc.
//#include "Circuits/Circuits.h"         // circuit modeling
//#include "Spectral/Spectral.h"         // phase vocoder, source/filter modeling
#include "Visualization/Visualization.h" // buffers for scopes, waveform displays etc
#include "Generators/Generators.h"     // oscillator, sample player, etc. - maybe rename to sources
#include "Modulators/Modulators.h"       // ADSR, LFO, step-sequencer, breakpoint-modulator, etc.
//#include "Effects/Effects.h"           // reverb, distortion, dynamics, chorus, etc.
//#include "Framework/Framework.h"       // parameter handling, save/recall, threading, polyphony, etc.
//#include "Instruments/Instruments.h"   // full blown instruments with polyphony, state-recall, etc.

#include "Unfinished/Unfinished.h"       // code under construction - should go to the bottom!
#include "Spectral/Spectral.h"           // move up, when cycle-mark finder was moved to analysis folder



#endif
