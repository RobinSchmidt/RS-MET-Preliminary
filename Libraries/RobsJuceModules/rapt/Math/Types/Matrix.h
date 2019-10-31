#ifndef RAPT_MATRIX_H
#define RAPT_MATRIX_H


/** A class for representing 2x2 matrices. They are treated as a special case because a lot of
things which are impractical in the general case can be done for the 2x2 case. For example, it's
possible to compute eigenvalues and eigenvectors via closed form formulas. */

template<class T>
class rsMatrix2x2
{

public:

  T a, b, c, d;
  // matrix coefficients |a b|
  //                     |c d|


  /** Stadard constructor. You can pass the matrix elements. If you pass nothing, an identity
  matrix will be created. */
  //rsMatrix2x2(T a = T(1), T b = T(0), T c = T(0), T d = T(1)) { setValues(a, b, c, d); }
  // todo: maybe require arguments to be passed - or initialze teh matrix to the zero matrix
  // by default

  /** Constructor. Initializes elements with  given values. */
  rsMatrix2x2(T a, T b, T c, T d) { setValues(a, b, c, d); }

  /** Standard constructor. Leaves elements uninitialized.
  (...try to avoid using it - prefer RAII) */
  rsMatrix2x2() {}


  /** \name Setup */


  /** Sets up the elements of the matrix. */
  void setValues(T a, T b, T c, T d) { this->a = a; this->b = b; this->c = c; this->d = d; }


  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  // todo: prepend get

  /** Returns the determinant of this matrix. */
  T determinant() const { return a*d - b*c; }

  /** Returns the trace (sum of diagonal elements) of this matrix.  */
  T trace() const { return a+d; }

  /** Returns the first eigenvalue of this matrix. */
  T eigenvalue1() const { return rsLinearAlgebra::eigenvalue2x2_1(a, b, c, d); }

  /** Returns the second eigenvalue of this matrix. */
  T eigenvalue2() const { return rsLinearAlgebra::eigenvalue2x2_2(a, b, c, d); }

  /** Returns the first eigenvector of this matrix. */
  rsVector2D<T> eigenvector1() const
  { rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_1(a, b, c, d, &v.x, &v.y, true); return v; }

  /** Returns the second eigenvector of this matrix. */
  rsVector2D<T> eigenvector2() const
  { rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_2(a, b, c, d, &v.x, &v.y, true); return v; }

  /** Returns the inverse of this matrix. */
  rsMatrix2x2<T> inverse() const
  { T D = determinant(); T s = T(1) / D; return rsMatrix2x2<T>(s*d, -s*b, -s*c, s*a); }

  // maybe these functions should be named getDeterminant, etc. - more consistent with other
  // classes and states more explicitly what they do

  /** Tests, if another matrix B is close to this matrix within a given tolerance (all components
  of the difference must be <= tolerance). */
  bool isCloseTo(const rsMatrix2x2<T>& B, T tol)
  {
    if(rsAbs(a-B.a) <= tol && rsAbs(b-B.b) <= tol && rsAbs(c-B.c) <= tol && rsAbs(d-B.d) <= tol)
      return true;
    return false;
  }
  // doesn't work with complex matrices

  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Adds two matrices: C = A + B. */
  rsMatrix2x2<T> operator+(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a + B.a; C.b = b + B.b; C.c = c + B.c; C.d = d + B.d; return C; }
  // can't we just do:
  // return rsMatrix2x2<T>(a + B.a, b + B.b, c + B.c, d + B.d);
  // and likewise for the other operators

  /** Subtracts two matrices: C = A - B. */
  rsMatrix2x2<T> operator-(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a - B.a; C.b = b - B.b; C.c = c - B.c; C.d = d - B.d; return C; }

  /** Multiplies two matrices: C = A * B. */
  rsMatrix2x2<T> operator*(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a*B.a + b*B.c; C.b = a*B.b + b*B.d;
    C.c = c*B.a + d*B.c; C.d = c*B.b + d*B.d; return C; }

  /** Multiplies the left matrix operand with the inverse of the right matrix operand. */
  rsMatrix2x2<T> operator/(const rsMatrix2x2<T>& B) const { return *this * B.inverse(); }

  /** Compares matrices for equality */
  bool operator==(const rsMatrix2x2<T>& B) const
  { return a == B.a && b == B.b && c == B.c && d == B.d; }

  /** Multiplies matrix by a vector: w = A*v */
  rsVector2D<T> operator*(const rsVector2D<T>& v) const
  {
    rsVector2D<T> w;
    w.x = a * v.x  +  b * v.y;
    w.y = c * v.x  +  d * v.y;
    return w;
  }
  // todo: left multiplication w = v^H * A

  // todo: operators that take a scalar as left or right argument


  //-----------------------------------------------------------------------------------------------
  /** \name Factory */

  static rsMatrix2x2<T> zero()     { return rsMatrix2x2<T>(T(0), T(0), T(0), T(0)); }

  static rsMatrix2x2<T> identity() { return rsMatrix2x2<T>(T(1), T(0), T(0), T(1)); }


  /** Returns the commutator of the two matrices A and B: C = A*B - B*A. In general, matrix
  multiplication is non-commutative, but for some special cases, it may be commutative nonetheless.
  The commutator captures, how non-commutative two matrices behave when being multiplied. If the
  two matrices commute (i.e. behave commutatively), their commutator is the zero matrix. */
  static rsMatrix2x2<T> commutator(const rsMatrix2x2<T>& A, const rsMatrix2x2<T>& B)
  {
    return A*B - B*A;
  }

};

/** Multiplies a scalar and a matrix. */
template<class T>
inline rsMatrix2x2<T> operator*(const T& s, const rsMatrix2x2<T>& A)
{
  return rsMatrix2x2<T>(s*A.a, s*A.b, s*A.c, s*A.d);
}


//=================================================================================================

/** This is a class for treating C-arrays as matrices. It does not store/own the actual matrix
data, it just acts as wrapper around an existing array for more conveniently accessing and
manipulating matrix elements. */

template<class T>
class rsMatrixView
{

public:

  /** \name Construction/Destruction */

  /** Default constructor */
  rsMatrixView() {}


  /**  */
  rsMatrixView(int numRows, int numColumns, T* data)
  {
    rsAssert(numRows >= 1 && numColumns >= 1 && data != nullptr);
    this->numRows = numRows;
    this->numCols = numColumns;
    dataPointer = data;
  }
  // can this be optimized by turning the assignments into copy-constructions?



  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Sets all elements in the matrix to the given value. */
  inline void setAllValues(T value) { rsArray::fillWithValue(dataPointer, getSize(), value); }

  /** Sets all elements on the main diagonal to the given value. If the matrix is not square, only
  the top-left square submatrix will be affected. */
  inline void setDiagonalValues(T value)
  {
    for(int i = 0; i < rsMin(numRows, numCols); i++)
      dataPointer[i*numCols + i] = value;
  }
  // needs test

  /** Scales all elements in the matrix by a given factor. */
  inline void scale(T factor) { rsArray::scale(dataPointer, getSize(), factor); }


  inline void reshape(int newNumRows, int newNumColumns)
  {
    rsAssert(newNumRows*newNumColumns == numRows*numCols);
    numRows = newNumRows;
    numCols = newNumColumns;
  }

  /** Resets the number of rows and columns to zero and the dataPointer to nullptr. */
  inline void reset()
  {
    numRows = 0;
    numCols = 0;
    dataPointer = nullptr;
  }

  // void setToIdentityMatrix(T scaler = 1);



  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  /**  */
  static bool areSameShape(const rsMatrixView<T>& A, const rsMatrixView<T>& B)
  {
    return A.numRows == B.numRows && A.numCols == B.numCols;
  }

  static bool areMultiplicable(const rsMatrixView<T>& A, const rsMatrixView<T>& B)
  {
    return A.numCols == B.numRows;
  }

  int getNumRows()    const { return numRows; }

  int getNumColumns() const { return numCols; }

  int getSize()       const { return numRows * numCols; }

  bool isRowVector() const { return numRows == 1; }

  bool isColumnVector() const { return numCols == 1; }

  bool isSquare() const { return numRows == numCols; }


  /** Returns a pointer to the stored data. When using this, be sure that you know exactly what
  you are doing.... */
  //T* getData() { return dataPointer; }

  /** Returns a const pointer to the data for read access as a flat array. */
  const T* getDataConst() const { return dataPointer; }


  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Initializes all elements with given value. */
  void init(T value = T(0)) { RAPT::rsArray::fillWithValue(dataPointer, getSize(), value); }


  //-----------------------------------------------------------------------------------------------
  /** \name Arithmetic */

  /** Adds elements of A to corresponding elements in B and stores results in C. */
  static void add(const rsMatrixView<T>* A, const rsMatrixView<T>* B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(*A, *B) && areSameShape(*A, *C), "arguments incompatible");
    rsArray::add(A->dataPointer, B->dataPointer, C->dataPointer, A->getSize());
  }

  /** Subtracts elements of B from corresponding elements A in and stores results in C. */
  static void sub(const rsMatrixView<T>* A, const rsMatrixView<T>* B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(*A, *B) && areSameShape(*A, *C), "arguments incompatible");
    rsArray::subtract(A->dataPointer, B->dataPointer, C->dataPointer, A->getSize());
  }

  /** Computes the matrix product C = A*B. */
  static void mul(const rsMatrixView<T>* A, const rsMatrixView<T>* B, rsMatrixView<T>* C)
  {
    rsAssert(A->numCols == B->numRows);
    rsAssert(C->numCols == B->numCols);
    rsAssert(C->numRows == A->numRows);
    for(int i = 0; i < C->numRows; i++) {
      for(int j = 0; j < C->numCols; j++) {
        (*C)(i,j) = T(0);
        for(int k = 0; k < A->numCols; k++)
          (*C)(i,j) += A->at(i,k) * B->at(k,j); }}
  }

  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Read and write access to matrix elements with row-index i and column-index j. */
  inline T& operator()(const int i, const int j) { return dataPointer[flatIndex(i, j)]; }

  /** Read only accees - used mainly internally with const reference arguments (for example,
  in add). */
  inline const T& at(const int i, const int j) const { return dataPointer[flatIndex(i, j)]; }

  /** Converts a row index i and a column index j to a flat array index. */
  inline int flatIndex(const int i, const int j) const
  {
    return numCols*i + j;
    // todo:
    //  -be more general: colStride*i + rowStride*j. goal: allow row-major and column-major storage
    //   while the syntax of the operator is always row-major (as is conventional in math)
    //   regardless whatever the internal storage format is - column major storage is required for
    //   compatibility with lapack
    // -maybe be even more general: colOffset + colStride*i + (rowOffset + rowStride)*j
    //  -> may allow to access sub-matrices with the same syntax (todo: verify formula)
    //  ...but maybe that should be done in a class rsSubMatrixView
  }


protected:

  /** \name Data */

  int numRows = 0, numCols = 0;  // number of rows and columns
  T *dataPointer = nullptr;      // pointer to the actual data

};


//=================================================================================================

/** This is a class for representing matrices and doing mathematical operations with them. It's 
implemented as subclass of rsMatrixView and stores the actual matrix data in a std::vector. Copy- 
and move constructors and -assignment operators have been implemented in order to avoid 
unnecessary heap allocations in arithmetic expressions with matrices. */

template<class T>
class rsMatrixNew : public rsMatrixView<T>
{

public:

  //-----------------------------------------------------------------------------------------------
  /** \name Construction/Assignment/Destruction */

  /** Standard constructor. You must pass the initial number of rows and columns */
  rsMatrixNew(int numRows = 0, int numColumns = 0)
  {
    setSize(numRows, numColumns);
    // todo: optionally init with zeros
  }

  /** Destructor. */
  ~rsMatrixNew()
  {
    int dummy = 0; // to figure out, when it gets called for debugging
  }

  /** Creates matrix from a std::vector.  */
  rsMatrixNew(int numRows, int numColumns, const std::vector<T>& newData) : data(newData)
  {
    numHeapAllocations++;   // data(newData) allocates
    rsAssert(numRows*numColumns == newData.size());
    this->numRows = numRows;
    this->numCols = numColumns;
    updateDataPointer();
  }

  /** Creates matrix from an unnamed/temporary/rvalue std::vector - convenient to initialize 
  elements. You can initialize matrices like this:
    rsMatrix<double> A(2, 3, {1.,2.,3., 4.,5.,6.});   */
  rsMatrixNew(int numRows, int numColumns, std::vector<T>&& newData) : data(std::move(newData))
  {
    numHeapAllocations++;             // we count the allocation that took place in the caller
    rsAssert(newData.size() == 0);
    rsAssert(numRows*numColumns == data.size());
    this->numRows = numRows;
    this->numCols = numColumns;
    updateDataPointer();
  }

  /** Copy constructor. Copies data from B into this object.  */
  rsMatrixNew(const rsMatrixNew& B)
  {
    setSize(B.numRows, B.numCols);
    rsArray::copy(B.dataPointer, this->dataPointer, this->getSize());
  }

  /** Move constructor. Takes over ownership of the data stored in B. */
  rsMatrixNew(rsMatrixNew&& B) : data(std::move(B.data))
  {
    rsAssert(B.data.size() == 0); // B's data has now become our data
    this->numRows = B.numRows;
    this->numCols = B.numCols;
    updateDataPointer();
    B.reset();                    // invalidates pointer in B
  }

  /** Copy assignment operator. Copies data from rhs into this object. */
  rsMatrixNew<T>& operator=(const rsMatrixNew<T>& rhs)
  {
    if (this != &rhs) { // self-assignment check expected
      setSize(rhs.numRows, rhs.numCols);
      rsArray::copy(rhs.dataPointer, this->dataPointer, this->getSize());
    }
    return *this;
  }

  /** Move assignment operator. Takes over ownership of the data stored in rhs. */
  rsMatrixNew<T>& operator=(rsMatrixNew<T>&& rhs)
  {
    data = std::move(rhs.data);
    rsAssert(rhs.data.size() == 0);
    this->numRows = rhs.numRows;
    this->numCols = rhs.numCols;
    updateDataPointer();
    rhs.reset();
    return *this;
  }

  // todo: make factory-functions: zero(numRows, numCols), identity(size), diag(vector), 
  // etc.

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Sets the number of rows and columns, this matrix should have. ToDo: provide a way to retain
  the data (optionally) - what does std::vector's resize do? Does it retain data...but if it does,
  it would be useless anyway in case the number of columns changed. */
  void setSize(int numRows, int numColumns)
  {
    if(numRows == this->numRows && numColumns == this->numCols)
      return;  // nothing to do

    this->numRows = numRows;
    this->numCols = numColumns;
    data.resize(this->numRows * this->numCols);
    numHeapAllocations++;                        // data.resize() may have re-allocated heap memory
    updateDataPointer();
    // optionally initialize with zeros
  }


  /** Computes the Kronecker product between matrices A and B. For a 3x2 matrix A, it looks like
  that:
              |a11*B a12*B|
  A (x) B  =  |a21*B a22*B|
              |a31*B a32*B|

  Where each entry aij*B is a submatrix of dimensions of B with the entries of B scaled by an
  appropriate element from A. */
  static rsMatrixNew<T> kroneckerProduct(const rsMatrixNew<T>& A, const rsMatrixNew<T>& B)
  {
    rsMatrixNew<T> C(A.numRows*B.numRows, A.numCols*B.numCols);

    // factor out into rsMatrixView, so we can call
    // this->kroneckerProduct(A, B, C)
    for(int ia = 0; ia < A.numRows; ia++) {
      for(int ja = 0; ja < A.numCols; ja++) {
        int startRow = ia*B.numRows;
        int startCol = ja*B.numCols;
        for(int ib = 0; ib < B.numRows; ib++) {
          for(int jb = 0; jb < B.numCols; jb++) {
            C(startRow+ib, startCol+jb) = A.at(ia,ja) * B.at(ib, jb); }}}}


    return C;
  }
  // maybe rename to tensorProduct
  // see https://rosettacode.org/wiki/Kronecker_product#C



  //-----------------------------------------------------------------------------------------------
  /** \name Manipulations */

  // transpose, negate, 

  /** Negates all values of the matrix, i.e. inverts their sign. */
  void negate() { rsArray::negate(&data[0], &data[0], getSize()); }



  //void conjugate


  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  // getDeterminant, getInverse, getFrobeniusNorm, get[Other]Norm, isPositiveDefinite, 
  // getEigenvalues, getTrace, isUpperLeftTriangular, getTransposed, getConjugateTransposed



  //-----------------------------------------------------------------------------------------------
  /** \name Decompositions */

  // getLowerUpperDecomposition ...or decomposeLU, decomposeQR, decomposeSVD


  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Compares matrices for equality */
  bool operator==(const rsMatrixNew<T>& rhs) const
  {
    if(this->numRows != rhs.numRows || this->numCols != rhs.numCols)
      return false;
    return rsArray::equal(this->dataPointer, rhs.dataPointer, this->getSize());
  }
  // maybe move to rsMatrixView, if possible

  /** Compares matrices for inequality */
  bool operator!=(const rsMatrixNew<T>& rhs) const { return !(*this == rhs); }

  /** Returns the negative of this matrix. */
  rsMatrixNew<T> operator-()
  {
    rsMatrixNew<T> C(this->numRows, this->numCols);
    for(int i = 0; i < this->getSize(); i++)
      C.dataPointer[i] = -this->dataPointer[i]; // maybe factor out into "neg" function in baseclass
    return C;
  }


  /** Adds two matrices: C = A + B. */
  rsMatrixNew<T> operator+(const rsMatrixNew<T>& B) const
  { rsMatrixNew<T> C(this->numRows, this->numCols); this->add(this, &B, &C); return C; }

  /** Subtracts two matrices: C = A - B. */
  rsMatrixNew<T> operator-(const rsMatrixNew<T>& B) const
  { rsMatrixNew<T> C(this->numRows, this->numCols); this->sub(this, &B, &C); return C; }

  /** Multiplies two matrices: C = A * B. */
  rsMatrixNew<T> operator*(const rsMatrixNew<T>& B) const
  { 
    rsMatrixNew<T> C(this->numRows, B.numCols); 
    this->mul(this, &B, &C); 
    return C; 
  }


  /** Adds another matrix to this matrix and returns the result. */
  rsMatrixNew<T>& operator+=(const rsMatrixNew<T>& B)
  { this->add(this, &B, this); return *this; }

  /** Subtracts another matrix from this matrix and returns the result. */
  rsMatrixNew<T>& operator-=(const rsMatrixNew<T>& B)
  { this->sub(this, &B, this); return *this; }

  /** Multiplies this matrix by another and returns the result. This is not an in-place process, i.e. it 
  will allocate temporary heap-memory. */
  rsMatrixNew<T>& operator*=(const rsMatrixNew<T>& B)
  { *this = *this * B; return *this; } 



  /** Multiplies this matrix with a scalar s: B = A*s. The scalar is to the right of the matrix. */
  rsMatrixNew<T> operator*(const T& s) const
  { rsMatrixNew<T> B(*this); B.scale(s); return B; }

  /** Multiplies this matrix by a scalar and returns the result. */
  rsMatrixNew<T>& operator*=(const T& s)
  { scale(s); return *this; }

  /** Divides this matrix by a scalar and returns the result. */
  rsMatrixNew<T>& operator/=(const T& s)
  { scale(T(1)/s); return *this; }




  static int numHeapAllocations;
    // instrumentation for unit-testing - it's actually the number of *potential* heap-allocations,
    // namely, the number of calls to data.resize() which may or may not re-allocate memory

protected:

  /** Updates the data-pointer inherited from rsMatrixView to point to the begin of our std::vector
  that holds the actual data. */
  void updateDataPointer()
  {
    if(data.size() > 0)
      this->dataPointer = &data[0];
    else
      this->dataPointer = nullptr;
  }


  /** \name Data */

  std::vector<T> data;
  // maybe we should just work with our inherited dataPointer and use new/delete
  // -saves a little bit of storage
  // -but then we can't look easily at the data in the debugger anymore -> very bad! so, nope!
  // -the little storage overhead of std::vector becomes negligible for all but the smallest
  //  matrices - on the other hand, very small matrices may be common
  // -maybe use std::vector in debug builds and new/delete in release builds

};

template<class T> int rsMatrixNew<T>::numHeapAllocations = 0;

/** Multiplies a scalar and a matrix. */
template<class T>
inline rsMatrixNew<T> operator*(const T& s, const rsMatrixNew<T>& A)
{
  rsMatrixNew<T> B(A);
  B.scale(s);
  return B;
}




// Notes:
// -class is under construction
// -design goals:
//  -use std::vector to hold the data in a flat array (so we can inspect it in the debugger)
//  -storage format should be compatible with lapack routines (maybe not by default, but can be
//   made so) - that means to support column major storage
//  -in expressions like rsMatrix<float> C = B*(A + B) + B; we want to avoid copying the data
//   unnecessarily - i.e. avoid that the temporaries that occur inside this expression use heap
//   allocation only when absolutely necessarry
//   ...this especially means, we need to pass the return values of the arithmetic operators by
//   reference rather than by value - typically, in an implementation like
//  -ideally, i want to be able to use it in production code for realtime processing...but that
//   may not be possible...but maybe with rsMatrixView, it is?
//
//    rsMatrixNew<T> operator+(const rsMatrixNew<T>& B) const
//    { rsMatrixNew<T> C(this->numRows, this->numCols); this->add(this, &B, &C); return C; }
//
//   we have one constructor call (-> heap allocation) to create C - but C is a local variable - to
//   return it to the caller, there would be *another* (copy?)constructor call - ...right? and that
//   second call is what we want to get rid of

// ToDo: 
//  -maybe rename to rsMatrixOnHeap and have a similar class rsMatrixOnStack (maybe using
//   std::dynarray instead of std::vector)...or by defining the size via template parameters at 
//   compile time ...maybe the std::vector vs std::dynarrray distinction can determined by passing
//   the storage container as template argument like so:
//   template<class ElemType, class ContainerType>
//   class rsMatrix { ContainerType<ElemType> data; };


// maybe see here:
// https://www.youtube.com/watch?v=PNRju6_yn3o
// https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/RVO_V_S_std_move?lang=en
// https://en.cppreference.com/w/cpp/language/copy_elision

#endif
