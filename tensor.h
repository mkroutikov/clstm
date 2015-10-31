#ifndef ocropus_tensor_
#define ocropus_tensor_

#include <memory>
#include <unordered_map>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

namespace ocropus {

using Eigen::Tensor;
using Eigen::TensorMap;
using Eigen::TensorRef;
using Eigen::DSizes;
using Eigen::Index;
using Eigen::array;
using std::shared_ptr;
using std::unique_ptr;

#define ROWS(A) (A).rows()
#define COLS(A) (A).cols()
//#define MAPFUN(M, F) ((M).unaryExpr(ptr_fun(F)))

#ifdef LSTM_DOUBLE
typedef double Float;
typedef Eigen::MatrixXd Mat;
typedef Eigen::VectorXd Vec;
#else
typedef float Float;
typedef Eigen::MatrixXf Mat;
typedef Eigen::VectorXf Vec;
#endif

typedef Eigen::Map<Mat> MatMap;

using Eigen::Tensor;
using Eigen::TensorMap;

typedef Float Scalar;
typedef Eigen::Tensor<Float, 1> EigenTensor1;
typedef Eigen::Tensor<Float, 2> EigenTensor2;
typedef Eigen::TensorMap<Eigen::Tensor<Float, 1>> TensorMap1;
typedef Eigen::TensorMap<Eigen::Tensor<Float, 2>> TensorMap2;
typedef Eigen::TensorRef<Eigen::Tensor<Float, 1>> TensorRef1;
typedef Eigen::TensorRef<Eigen::Tensor<Float, 2>> TensorRef2;

inline int rows(const TensorMap2 &m) { return m.dimension(0); }
inline int cols(const TensorMap2 &m) { return m.dimension(1); }
inline int rows(const EigenTensor2 &m) { return m.dimension(0); }
inline int cols(const EigenTensor2 &m) { return m.dimension(1); }

// inline Float reduction(const EigenTensor1 &m) { return m(0); }
inline Float reduction(const EigenTensor1 &m) { return m(0); }
inline Float reduction(const TensorMap1 &m) { return m(0); }
inline Float reduction(Float m) { return m; }
inline int argmax(const EigenTensor1 &m) {
  int mi = -1;
  Float mv = m(0);
  for (int i = 0; i < m.dimension(0); i++) {
    if (m(i) < mv) continue;
    mi = i;
    mv = m(i);
  }
  return mi;
}
inline Float sum(const EigenTensor1 &m) { return reduction(m.sum()); }
inline Float sum(const EigenTensor2 &m) { return reduction(m.sum()); }

// helper functions for Eigen::Tensor axes and sizes

inline Eigen::array<Eigen::IndexPair<int>, 1> axes(int i, int j) {
  Eigen::array<Eigen::IndexPair<int>, 1> result = {Eigen::IndexPair<int>(i, j)};
  return result;
}

inline Eigen::array<ptrdiff_t, 1> ar(int i) { return Eigen::array<ptrdiff_t, 1>({i}); }

inline Eigen::array<ptrdiff_t, 2> ar(int i, int j) {
  return Eigen::array<ptrdiff_t, 2>({i, j});
}

inline Eigen::Sizes<1> S(int i) { return Eigen::Sizes<1>({i}); }

inline Eigen::Sizes<2> S(int i, int j) { return Eigen::Sizes<2>({i, j}); }

#if 0
struct Context {
  virtual int gpu() { return -1; }
};
struct ThreadPoolContext : Context {
  unique_ptr<Eigen::ThreadPool> pool;
  unique_ptr<Eigen::ThreadPoolDevice> device;
  ThreadPoolContext(int n=4) {
    pool.reset(new Eigen::ThreadPool(n));
    device.reset(new Eigen::ThreadPoolDevice(pool.get(), n));
  }
};
struct GpuContext : Context {
  virtual int gpu() { return 0; }
  shared_ptr<Eigen::GpuDevice> device;
};

template <class LHS>
struct ContextSetter {
  Context *context;
  LHS lhs;
  ContextSetter(Context *context, LHS lhs) : context(context), lhs(lhs) {}
  template <class RHS>
  void operator=(RHS rhs) {
    if(!context) {
      lhs = rhs;
    } else if(typeid(context)==typeid(ThreadPoolContext)) {
      Eigen::ThreadPoolDevice *device = dynamic_cast<ThreadPoolContext*>(context)->device;
      lhs.device(*device) = rhs;
    } else if(typeid(context)==typeid(GpuContext)) {
      Eigen::GpuDevice *device = dynamic_cast<GpuContext*>(context)->device;
      lhs.device(*device) = rhs;
    } else {
      THROW("unknown context");
    }
  };
};

template <class LHS>
ContextSetter<LHS> operator>>(Context *context, LHS lhs) {
  return ContextSetter<LHS>(context, lhs);
}
#endif

struct tensor2 {
  int dims[2];
  Float *ptr = nullptr;

  tensor2() {}
  tensor2(const tensor2 &other) { 
    *this = other;
  }
  tensor2(TensorRef<Tensor<Float,2>> other) { }
  ~tensor2() { 
    clear();
  }
  void clear() {
    if(!ptr) return;
    free(ptr);
    ptr = nullptr;
    dims[0] = 0;
    dims[1] = 0;
  }
  int dimension(int i) const {
    return dims[i];
  }
  int rows() {
    return dims[0];
  }
  int cols() {
    return dims[1];
  }
  Float &operator()(int i, int j) {
    return (**this)(i,j);
  }
  void resize(int n, int m) { 
    clear();
    ptr = (Float*)malloc(n * m * sizeof(Float));
    dims[0] = n; 
    dims[1] = m;
  }
  int total_size() {
    return dims[0] * dims[1];
  }
  TensorMap<Tensor<Float,2>> operator*() {
    return TensorMap<Tensor<Float,2>>(ptr, dims[0], dims[1]);
  }
  void operator=(const tensor2 &other) {
    resize(other.dimension(0), other.dimension(1));
    memcpy(ptr, other.ptr, total_size() * sizeof(Float));
  }
  template <class RHS>
  void operator=(RHS rhs) {
    **this = rhs;
  }
  Float *data() {
    return ptr;
  }
  void setConstant(int n, int m, Float c) {
    resize(n,m);
    for(int N=n*m, i=0; i<N; i++) ptr[i] = c;
  }
  void setZero(int n, int m) {
    setConstant(n, m, 0);
  }
  void setZero() {
    for(int N=rows()*cols(), i=0; i<N; i++) ptr[i] = 0;
  }
};

}

#endif