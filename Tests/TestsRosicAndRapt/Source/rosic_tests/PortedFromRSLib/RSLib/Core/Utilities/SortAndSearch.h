#ifndef RS_SORTANDSEARCH_H
#define RS_SORTANDSEARCH_H

namespace RSLib
{

  // this file contains functions for sorting arrays, finding elements and related stuff

  /** Re-orders the elements in the array such that they fullfill the max-heap property. A max-heap
  is a binary tree which is completely filled on all levels except possibly the lowest which is 
  filled from the left up to a point. The value at a node is always greater than or equal to the 
  values at the child nodes. Thus, the largest element is found at the root. When representing 
  such a tree as array, we use the following convention: array[0] is the root, array[2*i+1] is the 
  left child of node i, array[2*i+2] is the right child of node i. The complexity is O(n). 
  Reference: Introduction to Algorithms, 2nd Ed, p. 133 */
  template <class T>
  void rsBuildMaxHeap(T *buffer, int length);

  /** Assuming that the sub-trees rooted at 2*i+1 (left subtree) and 2*i+2 (right subtree) are
  max-heaps but buffer[i] itself violates the max-heap property (by being smaller that one of 
  its child nodes), this function lets buffer[i] 'float down' the max-heap so that the tree rooted 
  at i becomes itself a max-heap. heapSize is the number of elements in the array for which the 
  max-heap property holds, so heapSize <= length. The complexity is O(log(n)) 
  Reference: Introduction to Algorithms, 2nd Ed, p. 130 */
  template <class T>
  void rsMaxHeapify(T *buffer, int length, int i, int heapSize);

  /** Sorts an array by first turning it into a max-heap (via buildMaxHeap). The largest element 
  is now found at array[0] and can be seen as the root of the max-heap. We may now exchange it 
  with the last array element to put the largest element into the last position (where it should 
  be). Due to the exchange, the new root now violates the max-heap property, but for its 
  child-nodes, the max-heap property remains unchanged. We may thus invoke maxHeapify to restore 
  the max-heap property of the root node (let it float down). Then we iterate the invokation of 
  maxHeapify for subarrays with linearly shrinking size. The (worst case) complexity is 
  O(n*log(n)). 
  Reference: Introduction to Algorithms, 2nd Ed, p. 136 
  \todo let it take a pointer to a comparator function as optinal argument (if NULL is passed, it 
  will use a default comparator function which is based on the < operator of the class)
  */
  template <class T>
  void rsHeapSort(T *buffer, int length);

  /** Checks whether the buffer is sorted in ascending order, that is buffer[i] <= buffer[i+1] for 
  all i. */
  template <class T>
  bool rsIsSortedAscending(T *buffer, int length);

  /** Finds all occurrences of the given pattern in the buffer and returns the start-indices for 
  the pattern inside the buffer as a std::vector which is in ascending order. The function uses the
  Knuth-Morris-Pratt algorithm 
  Reference: Introduction to Algorithms, 2nd Ed, p. 923 */
  template <class T>
  std::vector<int> rsFindAllOccurencesOf(T* buffer, int bufferLength, 
                                         T* pattern, int patternLength);

  /** Returns the first occurrence of an element in the passed buffer and its index, -1 if the
  element is not found in the buffer. The optional searchStart parameter can be used to skip an
  initial section in the search. */
  template <class T>
  int rsFindFirstOccurrenceOf(const T *buffer, const int length, const T &elementToFind,
                              const int searchStart = 0);

  /** Searches for the first occurrence of a pattern in the passed buffer and returns the index
  where it starts, -1 if the searched pattern is not found in the buffer. The optional searchStart
  parameter can be used to skip an initial section in the search. */
  template <class T>
  int rsFindFirstOccurrenceOf(const T *buffer, const int length, const T *patternToMatch,
                              const int patternLength, const int searchStart = 0);

  /** Finds the index where the highest peak occurs inside the passed buffer. The condition for a
  peak to occur at index i is: buffer[i] > buffer[i-1] && buffer[i] <= buffer[i+1]. So, if there's
  stretch of equal maximum values, it will return the index of the first of them. If there's no
  value inside the buffer that satifies the above condition, 0 will be returned. */
  template<class T>
  int rsFindHighestPeakIndex(T *buffer, int length);

  /** Returns the last occurrence of an element in the passed buffer and its index, -1 if the
  element is not found in the buffer. The optional searchStart parameter can be used to specify,
  where the search should start. If a value < 0 is passed (for example, -1), the search will start
  at the very end of the buffer, which is the default. */
  template <class T>
  int rsFindLastOccurrenceOf(const T *buffer, const int length, const T &elementToFind,
                             const int searchStart = -1);

  /** Finds the (non-integer) position of the upward zero crossing in the buffer which is nearest
  to 'searchStart'. The fractional part of the zero crossing is determined by linearly
  interpolating betwen the points below and above zero and solving fo the zero-crossing of the
  connecting line. */
  template<class T>
  T rsFindNearestUpwardZeroCrossing(T* buffer, int length, T searchStart);

}

#endif
