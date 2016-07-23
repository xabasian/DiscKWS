//*****************************************************************************
/** Implements a simple 4dim array for real values
 @author Shai Shalev 
 */

#include <iostream>
#include <exception>

template <class T>
class fourDimArray {
  
public:
  
  //-----------------------------------------------------------------------------
  /** Constructs a fourDimArray of size (L,M,N,K)
   @param L First dimension
   @param M First dimension
   @param N Second dimension
   @param K Third dimension
   */
  //  inline fourDimArray(uint L, uint M,uint N,uint K) : _M(M), _NNNN(N), _K(K) {
  inline fourDimArray(uint L,uint M,uint N,uint K) {
    _L = L;
    _M = M;
    _NNNN = N;
    _K = K;
    std::cerr << "Info: trying to allocate " 
    << (L*M*N*K*sizeof(T)/1024.0/1024.0) << "..." ;
    _data = new T[L*M*N*K];
    std::cerr << " succeeded." << std::endl;
  };
  
  //*****************************************************************************
  // reference operators
  //*****************************************************************************
  //-----------------------------------------------------------------------------
  /** Reference operator. Returns a refetence to an entry in the array
   @param l The first index to the requested entry
   @param m The second index to the requested entry
   @param n The third index to the requested entry
   @param k The fourth index to the requested entry
   @returns A reference to the requested array entry
   */
  inline T& operator() (uint l, uint m, uint n, uint k) {
    return *(_data + l + (m + (n + k*_NNNN)*_M)*_L);
  };
  
  
  // destructor
  inline ~fourDimArray() {
    delete [] _data;
  };
  
protected:
  T* _data;  // pointer to the data
  // dimensions
  uint _L;
  uint _M;
  uint _NNNN;
  uint _K;
};
