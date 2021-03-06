#pragma once

//namespace RAPT
//{

/** A class for representing rational functions R(x) = P(x) / Q(x) where P and Q are both 
polynomials in x. Rational functions are important in signal processing because the transfer 
functions of linear time invariant systems (a.k.a. filters) are of that type. The class provides 
facilities for performing arithmetic with rational functions (including composition), evaluation,
partial fraction expansion, etc. 

Note that instances of this class are not suitable for realtime code - there's a lot of memory 
allocation going on in the operators (std::vectors are created, resized, assigned, etc.) and the
algorithms are not optimized either.

to verify:
I think, the rational functions over a field (like the real or complex numbers) form themselves a 
field - as opposed to the polynomials, which form only a ring. That's why we can always do a 
division of two rational functions without having to worry about a remainder (which may occur in 
polynomial division). */

template<class T>
class rsRationalFunction
{

public:

  //-----------------------------------------------------------------------------------------------
  /** \name Construction/Destruction */

  rsRationalFunction() {}

  rsRationalFunction(
    const std::vector<T>& numeratorCoeffs, 
    const std::vector<T>& denominatorCoeffs,
    const T& reductionTolerance = T(0) )
    : num(numeratorCoeffs), den(denominatorCoeffs), tol(reductionTolerance)
  {

  }

  rsRationalFunction(const T& number) : num(number), den(T(1))
  { 

  }


  /*
  rsRationalFunction(const T& number, const T& reductionTolerance = T(0)) 
    : num(number), den(T(1)), tol(reductionTolerance)
  { 

  }
  */
  // get rid of this constructor - when calling it like
  // r = rsRationalFunction<double>({1}, {1});  we don't get the constant "1" function but one with
  // a tolerance of 1 - the pattern matching does not work as one might want or expect
  // ...but damn - we need a constructor that takes an int - it gets called, for example, in 
  // rsMatrix::getDiagonalProduct - but this construtor cannot take an optional tolerance - that
  // messes up the pattern matching that decides which constructor is called (and which implicit 
  // conversions are made)

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  // setNumerator, setDenominator // for polynomial and std::vector and maybe plain arrays

  bool reduce(T tol);


  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  rsPolynomial<T> getNumerator()   const { return num; }
  rsPolynomial<T> getDenominator() const { return den; }

  int getNumeratorDegree()   const { return num.getDegree(); }
  int getDenominatorDegree() const { return den.getDegree(); }


  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Returns a new rational function that is the negative of this one. */
  rsRationalFunction<T> operator-() const 
  {
    rsRationalFunction<T> r = *this;
    r.num.negate();
    return r;
  }

  /** Adds two rational functions. */
  rsRationalFunction<T> operator+(const rsRationalFunction<T>& q) const
  {
    rsRationalFunction<T> r;
    ratAdd(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, r.num.coeffs, r.den.coeffs, 
      rsBiggest(tol, q.tol)); 
    //ratAdd(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, r.num.coeffs, r.den.coeffs);
    return r;
  }

  /** Subtracts two rational functions. */
  rsRationalFunction<T> operator-(const rsRationalFunction<T>& q) const
  {
    rsRationalFunction<T> r;
    ratAdd(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, r.num.coeffs, r.den.coeffs, 
      rsBiggest(tol, q.tol), T(1), T(-1));
    return r;
  }

  /** Multiplies two rational functions. */
  rsRationalFunction<T> operator*(const rsRationalFunction<T>& q) const
  {
    rsRationalFunction<T> r;
    ratMul(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, r.num.coeffs, r.den.coeffs, 
      rsBiggest(tol, q.tol));
    return r;
  }

  /** Divides two rational functions. */
  rsRationalFunction<T> operator/(const rsRationalFunction<T>& q) const
  {
    rsRationalFunction<T> r;
    ratDiv(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, r.num.coeffs, r.den.coeffs, 
      rsBiggest(tol, q.tol));
    return r;
    // implement conversion operator in rsPolynomial to get rid of accessing coeffs
  }

  /** Adds another rational function to this one. */
  rsRationalFunction<T>& operator+=(const rsRationalFunction<T>& q)
  { 
    ratAdd(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, num.coeffs, den.coeffs, 
      rsBiggest(tol, q.tol));
    return *this;
  }

  /** Multiplies this rational function by another one. */
  rsRationalFunction<T>& operator*=(const rsRationalFunction<T>& q)
  { 
    ratMul(num.coeffs, den.coeffs, q.num.coeffs, q.den.coeffs, num.coeffs, den.coeffs, 
      rsBiggest(tol, q.tol));
    return *this;
  }


  rsRationalFunction<T> operator*(const T& a) const
  { return rsRationalFunction<T>(num*a, den); } // write a*num




  /** Returns the rational function that results from nesting/composing the given inner rational 
  function with this function as outer function. You can use it like h = f(g) where h,f,g are all 
  rsRationalFunction objects. */
  rsRationalFunction<T> operator()(rsRationalFunction<T> inner) const
  {
    rsRationalFunction<T> r;
    ratNest(inner.num.coeffs, inner.den.coeffs, num.coeffs, den.coeffs, 
      r.num.coeffs, r.den.coeffs, rsBiggest(tol, inner.tol));
    return r;
  }

  /** Evaluates the function at the given input x. */
  T operator()(T x) const
  {
    return num(x) / den(x);
  }

  /** Compares this rational function to another one for equality. */
  bool operator==(const rsRationalFunction<T>& q) const 
  { 
    return num == q.num && den == q.den;
  }




  //===============================================================================================
  /** \name Computations on raw coefficient arrays */


  /** A general routine for performing a partial fraction expansion of a rational function with 
  known poles. You must pass the coefficient arrays of numerator and denominator, the array of 
  poles along with an array of their respective multiplicities (these two arrays are both of length
  "numDistinctPoles". On return, the pfeCoeffs will contain the residues corresponding the the 
  given poles. The array is of length equal to the total number of poles, each counted with its 
  multiplicity, which is the same number as the degree of the denominator. The ordering of the 
  pfeCoeffs corresponds to the ordering of the poles and if a pole p_i has a  multiplicity k, then 
  you will first get the residue for r_i/(x-p_i), then r_i/(x-p_i)^2, etc. If the rational function
  is not strictly proper (i.e. numeratorDegree >= denominatorDegree), the expansion will also 
  feature a polynomial part which is returned in polyCoeffs - which must be of length 
  numeratorDegree+1 which is the *maximum potential* number of coefficients in the polynomial part.
  It's *not* enough if it is just long enough to hold the *actual* number of coeffs. If the actual 
  number of polynomial coeffs is less than the maximum possible number, you'll notice this by 
  getting (numerically close to) zero-valued coeffs for the higher order terms. Depending on 
  whether or not all poles are distinct, it dipsatches between the two functions 
  partialFractionExpansionDistinctPoles and partialFractionExpansionMultiplePoles for the actual
  work. It's the high-level interface function meant to be called from client code.

  Note that function may manipulate the incoming numerator and denominator arrays in place (in 
  order to make the denominator monic and/or divide out the polynomial part from the numerator). 
  So, if you still need them in their original form after the function returns, you should create a
  local copy to pass into the function. */
  template<class R>
  static void partialFractionExpansion(
    std::complex<R>* numerator, int numeratorDegree,
    std::complex<R>* denominator, int denominatorDegree,
    const std::complex<R>* poles, const int* multiplicities, int numDistinctPoles,
    std::complex<R>* pfeCoeffs, std::complex<R>* polyCoeffs = nullptr);
  // -may allocate heap memory (in case of multiple poles)
  // ToDo:
  // -have a higher-level version of the function that doesn't require the poles to be known and
  //  passed in by the caller (the function should find them itself via a root finder)
  // maybe rename to partialFractions
  // maybe needs a separate template parameter R for real numbers


  /** A routine to perform a partial fraction expansion of a strictly proper rational function when
  all poles are distinct. In this common special case, a much more efficient and numerically more 
  precise (supposedly - verify that) algorithm can be used than in the general case where poles may 
  have multiplicities. This function implements the cover-up method, see:
  https://en.wikipedia.org/wiki/Heaviside_cover-up_method  
  called interbally by partialFractionExpansion  */
  template<class R>
  static void partialFractionExpansionDistinctPoles(
    std::complex<R> *num, int numDeg, std::complex<R> *den, int denDeg,
    const std::complex<R> *poles, std::complex<R> *pfeCoeffs);

  /** A routine to perform a partial fraction expansion of a strictly proper rational function when
  some poles may have mutliplicities. The algorithm implemented here solves the linear system
  of equations that results from equating the original rational function to a partial fraction
  expansion with undetermined coefficients, multiplying both sides by the denominator and equating
  coefficients of the polynomials on both sides. called interbally by partialFractionExpansion */
  template<class R>
  static void partialFractionExpansionMultiplePoles(
    const std::complex<R>* num, int numDeg, const std::complex<R>* den, int denDeg,
    const std::complex<R>* poles, const int* multiplicities, int numDistinctPoles,
    std::complex<R>* pfeCoeffs);
  // allocates heap memory


  // convenience functions to work on std::vector (not recommended for production code due to 
  // extra memory allocations - mostly for making tests and experiments more convenient):

  template<class R>
  static std::vector<std::complex<R>> partialFractions(
    const std::vector<std::complex<R>>& numerator,
    const std::vector<std::complex<R>>& denominator,
    const std::vector<std::complex<R>>& poles);

  template<class R>
  static std::vector<std::complex<R>> partialFractions(
    const std::vector<std::complex<R>>& numerator,
    const std::vector<std::complex<R>>& denominator,
    const std::vector<std::complex<R>>& poles,
    const std::vector<int>& multiplicities);
  // maybe have another parameter later to switch between algorithms




protected:

  rsPolynomial<T> num, den;  // numerator and denominator polynomials
  T tol = T(0);              // tolerance for reduction in the arithmetic operators

};


//}

/*
todo:
-implement a conversion constructor that can take a polynomial and make a rationla function from it

*/