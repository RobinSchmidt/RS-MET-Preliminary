#pragma once

// move to somewhere else:
template<class T>
bool doNothing(T x)
{
  return x == x;
}

/** This function should be used to indicate a runtime error. */
inline void rsError(const char *errorMessage = nullptr)
{
  doNothing(errorMessage);
  // fixes "unreferenced formal parameter" warning - we don't do anything with the message, but 
  // we may want to see it in the debugger

  //printf("%s", errorMessage);
  RS_DEBUG_BREAK;
  // \todo have some conditional compilation code based on the DEBUG macro (trigger a break),
  // maybe open an error message box, etc.
}

inline void rsWarning(const char* message = nullptr)
{
  std::cout << "Warning: " << message << "\n";
}


/** This function should be used for runtime assertions. */
inline void rsAssert(bool expression, const char *errorMessage = nullptr)
{
  if( expression == false )
    rsError(errorMessage);
}

inline void rsAssertFalse(const char *errorMessage = nullptr) { rsAssert(false, errorMessage); }
// lol! this is stupid! remove and use rsError directly!

// todo: have functions for logging and printing


/*
Some Notes on hard to catch bugs

Heap Corruptions:

I think, heap corruptions may occur erratically when trying to write beyond the end of a 
std::vector when the function, that accesses the vector, does so via a pointer to its first 
element. In this case, the range-check in std::vector itself is bypassed, so we don't trigger the 
debug assertion there. Whether or not the heap actually gets corrupted depends on how much memory 
the vector has allocated which may be more than its current size. The heap corruption only occurs
when we try to write beyond its capacity, which we have no control over, because the growing is 
controlled by the standard library - which is why it's so erratic and hard to reproduce reliably 
(...i think...although, the capacity should actually be deterministic...hmmm - but i sometimes get
or don't those heap corruptions depending on some change in a totally unrelated piece of code)

*/
