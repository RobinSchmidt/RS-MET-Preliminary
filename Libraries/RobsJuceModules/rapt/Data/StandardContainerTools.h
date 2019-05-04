#ifndef RAPT_STANDARDCONTAINER_H_INCLUDED
#define RAPT_STANDARDCONTAINER_H_INCLUDED

/** A collection of convenience functions for the container classes of the C++ standard template 
library (STL), such as std::vector, std::map, etc. */

//=================================================================================================
// functions for std::vector

template<class T>
inline std::vector<T> rsConstantVector(size_t size, T value)
{
  std::vector<T> v(size);
  for(size_t i = 0; i < size; i++)
    v[i] = value;
  return v;
}

template<class T>
inline size_t rsSize(const std::vector<T>& v)
{
  return v.size();
}

template<class T>
inline void rsAppend(std::vector<T>& v, T newElement)
{
  v.push_back(newElement);
}

/** Appends vector w to vector v. */
template<class T>
inline void rsAppend(std::vector<T>& v, const std::vector<T>& w)
{
  size_t nv = v.size();  // old size of v
  size_t nw = w.size();  // 
  v.resize(v.size() + w.size()); // if v and w are the same, this will also change the size of w, 
  rsArray::copyBuffer(&w[0], &v[nv], (int)nw);  // ...that's why we needed to figure out nw before

  // another implementation - looks safer but the above works, too
  //if(&v[0] == &w[0]) {       // appending a vector to itself, we need a temporary...
  //  std::vector<T> tmp(w);
  //  rsAppend(v, tmp);
  //}
  //else {
  //  size_t nv = v.size();    // old size of v
  //  v.resize(v.size() + w.size());
  //  rsArray::copyBuffer(&w[0], &v[nv], (int)w.size());
  //}
}


template<class T>
inline std::vector<T> rsConcatenate(std::vector<T>& v, const std::vector<T>& w)
{
  std::vector<T> r(v.size() + w.size());
  for(size_t i = 0; i < v.size(); i++) r[i]            = v[i];
  for(size_t i = 0; i < w.size(); i++) r[i + v.size()] = w[i];
  return r;
}

template<class T>
inline bool rsAreVectorsEqual(const std::vector<T>& v, const std::vector<T>& w, double tolerance)
{
  if(v.size() != w.size())
    return false;
  for(size_t i = 0; i < v.size(); i++) {
    if(abs(v[i]-w[i]) > tolerance)
      return false;
  }
  return true;
}

template<class T>
inline void rsInsert(std::vector<T>& v, const T& newElement, size_t index)
{
  v.insert(v.begin() + index, newElement);
}

template<class T>
inline void rsInsertValue(std::vector<T>& v, T newElement, size_t index)
{
  v.insert(v.begin() + index, newElement);
}

template<class T>
inline void rsPrepend(std::vector<T>& v, const T& newElement)
{
  v.insert(v.begin(), newElement);
}

template<class T>
inline void rsRemove(std::vector<T>& v, size_t index)
{
  v.erase(v.begin() + index);
}

/** Removes the range indexed from first to last, both ends inclusive! Attention: in 
std::vector::erase, the "last" would be excluded - but that's really counter-intuitive and provokes
off-by-one bugs, that's why i use a both-ends-inclusive convention. */
template<class T>
inline void rsRemoveRange(std::vector<T>& v, size_t first, size_t last)
{
  v.erase(v.begin() + first, v.begin() + last + 1);
}

template<class T>
inline std::vector<T> rsSelect(std::vector<T>& v, std::vector<size_t> indices)
{
  std::vector<T> r(indices.size());
  for(size_t i = 0; i < indices.size(); i++)
    r[i] = v[indices[i]];
  return r;
}

template<class T>
inline size_t rsFind(std::vector<T>& v, T elementToFind)
{
  for(size_t i = 0; i < v.size(); i++)
    if(v[i] == elementToFind)
      return i;
  return v.size(); // as convention, return vector-length, if element is not found
}

template<class T>
inline T rsLast(const std::vector<T>& v)
{
  return v[v.size()-1];
} // make also an rsFirst

template<class T>
inline bool rsRemoveFirstOccurrence(std::vector<T>& v, T elementToRemove)
{
  for(size_t i = 0; i < v.size(); i++)
    if(v[i] == elementToRemove){
      rsRemove(v, i);
      return true; }
  return false;
}

template<class T>
inline void rsReverse(std::vector<T>& v)
{
  rsArray::reverse(&v[0], (int) v.size());
}

template<class T>
inline void rsSetAllValues(std::vector<T>& v, T value)
{
  for(size_t i = 0; i < v.size(); i++)
    v[i] = value;
}

template<class T>
inline void rsSetZero(std::vector<T>& v)
{
  rsSetAllValues(v, T(0));
}

template<class T>
inline bool rsContains(std::vector<T>& v, T elementToCheckFor)
{
  for(size_t i = 0; i < v.size(); i++)
    if(v[i] == elementToCheckFor)
      return true;
  return false;
}

template<class T>
inline void rsAppendIfNotAlreadyThere(std::vector<T>& v, T newElement)
{
  if(!rsContains(v, newElement))
    rsAppend(v, newElement);
}

template<class T>
inline T rsGetAndRemoveLast(std::vector<T>& v)
{
  T result = v[v.size()-1];
  v.pop_back();
  return result;
}

/** Resizes the vector to the given new size and if this new size is greater than the old size, 
intializes all the new entries at the end with "value". */
template<class T>
inline void rsResizeWithInit(std::vector<double>& v, size_t newSize, T value)
{
  size_t oldSize = v.size();
  v.resize(newSize);
  for(size_t i = oldSize; i < newSize; i++)
    v[i] = value;
}

/** Prepends "amount" copies of "value" to the vector, i.e. increases the size of the vector by 
"amount", shifts all values "amount" places to the right and inserts "amount" number of "value"s 
at the front. */
template<class T>
inline void rsPadLeft(std::vector<double>& v, size_t amount, T value)
{
  if(amount == 0) 
    return;
  size_t oldSize = v.size();
  v.resize(oldSize+amount);
  for(size_t i = v.size()-1; i >= amount; i--)
    v[i] = v[i-amount];
  for(size_t i = 0; i < amount; i++)
    v[i] = value;
  // maybe, it can be done with memmove and memset more efficiently?
}


template<class T>
std::vector<T> rsDifference(const std::vector<T> x)
{
  if(x.size() < 2)
    return std::vector<T>();  // result is empty
  std::vector<T> d(x.size()-1);
  for(size_t i = 0; i < d.size(); i++)
    d[i] = x[i+1] - x[i];
  return d;
}

/** Converts a vector of amplitudes to decibel values with a given floor decibel value to avoid 
negative infinity for zero amplitudes. */
template<class T>
std::vector<T> rsAmpToDb(const std::vector<T>& a, T floorDb = -std::numeric_limits<T>::infinity())
{
  std::vector<T> db(a.size());
  for(size_t i = 0; i < a.size(); i++)
    db[i] = std::max(floorDb, 20*log10(a[i]));
  return db;
}


//template<class T>
//T rsMinValue(T 

template<class T>
T rsMax(const std::vector<T>& x)
{
  T max = std::numeric_limits<T>::min(); 
  // we should instead use -inf for double/float? -> make explicit specilizations

  for(size_t i = 0; i < x.size(); i++) {
    if(x[i] > max)
      max = x[i];
  }
  return max;
}

template<class T>
T rsSum(const std::vector<T>& x)
{
  T sum = T(0); 
  for(size_t i = 0; i < x.size(); i++)
    sum += x[i];
  return sum;
}

template<class T>
T rsMean(const std::vector<T>& x)
{
  return rsSum(x) / T(x.size());
}

template<class T>
void rsScale(std::vector<T>& x, T scaler)
{
  for(size_t i = 0; i < x.size(); i++)
    x[i] *= scaler;
}




/** Converts C-array to std::vector. */
template<class T>
inline std::vector<T> toVector(T* theArray, size_t size) // rename to rsToVector
{
  std::vector<T> v(size);
  for(size_t i = 0; i < size; i++)
    v[i] = theArray[i];
  return v;
}

/** Multiplies a scalar and a vector. */
template<class T>
inline std::vector<T> operator*(const T& x, const std::vector<T>& v)
{
  std::vector<T> result(v.size());
  for(int i = 0; i < v.size(); i++)
    result[i] = x * v[i];
  return result;
}

/** Divides a scalar by a vector. */
template<class T>
inline std::vector<T> operator/(const T& x, const std::vector<T>& v)
{
  std::vector<T> result(v.size());
  for(int i = 0; i < v.size(); i++)
    result[i] = x / v[i];
  return result;
}


/** Adds a scalar to a vector. */
template<class T>
inline std::vector<T> operator+(const std::vector<T>& v, const T& x)
{
  std::vector<T> result(v.size());
  for(int i = 0; i < v.size(); i++)
    result[i] = v[i] + x;
  return result;
}

/** Subtracts a scalar from a vector. */
template<class T>
inline std::vector<T> operator-(const std::vector<T>& v, const T& x)
{
  std::vector<T> result(v.size());
  for(int i = 0; i < v.size(); i++)
    result[i] = v[i] - x;
  return result;
}

/** Adds two vectors element wise. */
template<class T>
inline std::vector<T> operator+(const std::vector<T>& x, const std::vector<T>& y)
{
  size_t Nmax = std::max(x.size(), y.size());
  size_t Nmin = std::min(x.size(), y.size());
  std::vector<T> result(Nmax);
  for(int i = 0; i < Nmin; i++)
    result[i] = x[i] + y[i];
  return result;
}

/** Subtracts two vectors element wise. */
template<class T>
inline std::vector<T> operator-(const std::vector<T>& x, const std::vector<T>& y)
{
  size_t Nmax = std::max(x.size(), y.size());
  size_t Nmin = std::min(x.size(), y.size());
  std::vector<T> result(Nmax);
  for(int i = 0; i < Nmin; i++)
    result[i] = x[i] - y[i];
  return result;
}

/** Multiplies two vectors element wise. */
template<class T>
inline std::vector<T> operator*(const std::vector<T>& x, const std::vector<T>& y)
{
  size_t Nmax = std::max(x.size(), y.size());
  size_t Nmin = std::min(x.size(), y.size());
  std::vector<T> result(Nmax);
  for(int i = 0; i < Nmin; i++)
    result[i] = x[i] * y[i];
  return result;
}

//=================================================================================================
// functions for std::map

/** Checks, if a map contains a given key. */
template<class Key, class Value>
inline bool rsContains(const std::map<Key, Value>& map, const Key& key)
{
  auto iterator = map.find(key);
  if( iterator == map.end() )
    return false;
  return true;
}



#endif