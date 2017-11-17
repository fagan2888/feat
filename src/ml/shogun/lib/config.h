#ifndef __SHOGUN_LIB_CONFIG_H__
#define __SHOGUN_LIB_CONFIG_H__

#define MACHINE "x86_64"

#define SFMT_MEXP 19937
#define DSFMT_MEXP 19937

#define HAVE_HDF5 1
#define HAVE_CURL 1
#define HAVE_JSON 1
#define HAVE_XML 1
#define HAVE_DOXYGEN 1
#define HAVE_LAPACK 1
/* #undef HAVE_MVEC */
#define HAVE_PROTOBUF 1

#define HAVE_ARPACK 1
/* #undef HAVE_VIENNACL */
/* #undef VIENNACL_VERSION */
/* #undef HAVE_OPENCV */
/* #undef HAVE_CATLAS */
/* #undef HAVE_ATLAS */
/* #undef HAVE_MKL */
#define HAVE_NLOPT 1
/* #undef USE_LPSOLVE */
#define HAVE_PTHREAD 1
#define HAVE_OPENMP 1
/* #undef USE_CPLEX */
/* #undef HAVE_COLPACK */
/* #undef HAVE_ARPREC */
/* #undef USE_META_INTEGRATION_TESTS */

#define HAVE_POWL 1
#define HAVE_LGAMMAL 1
#define HAVE_SQRTL 1
#define HAVE_LOG2 1
/* #undef USE_LOGCACHE */
/* #undef USE_LOGSUMARRAY */

/* Tells ViennaCL to use OpenCL as computation backend */
#define VIENNACL_WITH_OPENCL 1

/* for linear algebra global backend setups */
#define USE_EIGEN3_GLOBAL 1
/* #undef USE_VIENNACL_GLOBAL */

#define USE_EIGEN3_LINSLV 1
/* #undef USE_VIENNACL_LINSLV */

#define USE_EIGEN3_EIGSLV 1
/* #undef USE_VIENNACL_EIGSLV */

/* Define to 1 if you have the declaration of `isfinite', and to 0 if you
   don't. */
/* #undef HAVE_DECL_ISFINITE */

/* Define to 1 if you have the declaration of `isinf', and to 0 if you don't. */
#define HAVE_DECL_ISINF 1

/* Define to 1 if you have the declaration of `isnan', and to 0 if you don't. */
#define HAVE_DECL_ISNAN 1

/* Define to 1 if you have the `finite' function. */
/* #undef HAVE_FINITE */

/* Define to 1 if you have the `fpclass' function. */
#define HAVE_FPCLASS 1

/* Define to 1 if you have the `isfinite' function. */
/* #undef HAVE_ISFINITE */

/* Define to 1 if you have the `isinf' function. */
#define HAVE_ISINF 1

/* Define to 1 if you have the `isnan' function. */
#define HAVE_ISNAN 1

/* Define to 1 if you have the `std::isfinite' function. */
#define HAVE_STD_ISFINITE 1

/* Define to 1 if you have the `std::isinf' function. */
#define HAVE_STD_ISINF 1

/* Define to 1 if you have the `std::isnan' function. */
#define HAVE_STD_ISNAN 1

#define HAVE_DECL_SIGNGAM 1

#define HAVE_FDOPEN 1

#define USE_SPINLOCKS 1
#define USE_SHORTREAL_KERNELCACHE 1
#define USE_BIGSTATES 1

/* #undef USE_HMMDEBUG */
#define USE_HMMCACHE 1
/* #undef USE_HMMPARALLEL */
/* #undef USE_HMMPARALLEL_STRUCTURES */

/* #undef USE_PATHDEBUG */

/* #undef USE_SVMLIGHT */
/* #undef USE_MOSEK */
#define USE_GPL_SHOGUN 1

#define USE_GLPK 1
#define USE_LZO 1
#define USE_GZIP 1
#define USE_BZIP2 1
#define USE_LZMA 1
#define USE_SNAPPY 1

#define HAVE_SSE2 1
#define HAVE_BUILTIN_VECTOR 1

/* #undef DARWIN */
/* #undef FREEBSD */
#define LINUX 1

/* #undef USE_SWIG_DIRECTORS */
/* #undef TRACE_MEMORY_ALLOCS */
/* #undef USE_JEMALLOC */

/* #undef HAVE_CXX0X */
#define HAVE_CXX11 1
#define HAVE_CXX11_ATOMIC 1
#define HAVE_STD_UNORDERED_MAP 1

/* does the compiler support abi::__cxa_demangle */
#define HAVE_CXA_DEMANGLE 1

/* random related defines */
/* #undef HAVE_ARC4RANDOM */
#define DEV_RANDOM "/dev/urandom"

#endif /* __SHOGUN_LIB_CONFIG_H__ */
