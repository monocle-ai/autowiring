//  (C) Copyright John Maddock 2001.
//  (C) Copyright Jens Maurer 2001.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  config for libstdc++ v3
//  not much to go in here:

#define AUTOBOOST_GNU_STDLIB 1

#ifdef __GLIBCXX__
#define AUTOBOOST_STDLIB "GNU libstdc++ version " AUTOBOOST_STRINGIZE(__GLIBCXX__)
#else
#define AUTOBOOST_STDLIB "GNU libstdc++ version " AUTOBOOST_STRINGIZE(__GLIBCPP__)
#endif

#if !defined(_GLIBCPP_USE_WCHAR_T) && !defined(_GLIBCXX_USE_WCHAR_T)
#  define AUTOBOOST_NO_CWCHAR
#  define AUTOBOOST_NO_CWCTYPE
#  define AUTOBOOST_NO_STD_WSTRING
#  define AUTOBOOST_NO_STD_WSTREAMBUF
#endif

#if defined(__osf__) && !defined(_REENTRANT) \
  && ( defined(_GLIBCXX_HAVE_GTHR_DEFAULT) || defined(_GLIBCPP_HAVE_GTHR_DEFAULT) )
// GCC 3 on Tru64 forces the definition of _REENTRANT when any std lib header
// file is included, therefore for consistency we define it here as well.
#  define _REENTRANT
#endif

#ifdef __GLIBCXX__ // gcc 3.4 and greater:
#  if defined(_GLIBCXX_HAVE_GTHR_DEFAULT) \
        || defined(_GLIBCXX__PTHREADS) \
        || defined(_GLIBCXX_HAS_GTHREADS) \
        || defined(_WIN32) \
        || defined(_AIX)
      //
      // If the std lib has thread support turned on, then turn it on in Boost
      // as well.  We do this because some gcc-3.4 std lib headers define _REENTANT
      // while others do not...
      //
#     define AUTOBOOST_HAS_THREADS
#  else
#     define AUTOBOOST_DISABLE_THREADS
#  endif
#elif defined(__GLIBCPP__) \
        && !defined(_GLIBCPP_HAVE_GTHR_DEFAULT) \
        && !defined(_GLIBCPP__PTHREADS)
   // disable thread support if the std lib was built single threaded:
#  define AUTOBOOST_DISABLE_THREADS
#endif

#if (defined(linux) || defined(__linux) || defined(__linux__)) && defined(__arm__) && defined(_GLIBCPP_HAVE_GTHR_DEFAULT)
// linux on arm apparently doesn't define _REENTRANT
// so just turn on threading support whenever the std lib is thread safe:
#  define AUTOBOOST_HAS_THREADS
#endif

#if !defined(_GLIBCPP_USE_LONG_LONG) \
    && !defined(_GLIBCXX_USE_LONG_LONG)\
    && defined(AUTOBOOST_HAS_LONG_LONG)
// May have been set by compiler/*.hpp, but "long long" without library
// support is useless.
#  undef AUTOBOOST_HAS_LONG_LONG
#endif

// Apple doesn't seem to reliably defined a *unix* macro
#if !defined(CYGWIN) && (  defined(__unix__)  \
                        || defined(__unix)    \
                        || defined(unix)      \
                        || defined(__APPLE__) \
                        || defined(__APPLE)   \
                        || defined(APPLE))
#  include <unistd.h>
#endif

#if defined(__GLIBCXX__) || (defined(__GLIBCPP__) && __GLIBCPP__>=20020514) // GCC >= 3.1.0
#  define AUTOBOOST_STD_EXTENSION_NAMESPACE __gnu_cxx
#  define AUTOBOOST_HAS_SLIST
#  define AUTOBOOST_HAS_HASH
#  define AUTOBOOST_SLIST_HEADER <ext/slist>
# if !defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
#   define AUTOBOOST_HASH_SET_HEADER <ext/hash_set>
#   define AUTOBOOST_HASH_MAP_HEADER <ext/hash_map>
# else
#   define AUTOBOOST_HASH_SET_HEADER <backward/hash_set>
#   define AUTOBOOST_HASH_MAP_HEADER <backward/hash_map>
# endif
#endif

//
// Decide whether we have C++11 support turned on:
//
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103)
#  define AUTOBOOST_LIBSTDCXX11
#endif
//
//  Decide which version of libstdc++ we have, normally
//  stdlibc++ C++0x support is detected via __GNUC__, __GNUC_MINOR__, and possibly
//  __GNUC_PATCHLEVEL__ at the suggestion of Jonathan Wakely, one of the stdlibc++
//  developers. He also commented:
//
//       "I'm not sure how useful __GLIBCXX__ is for your purposes, for instance in
//       GCC 4.2.4 it is set to 20080519 but in GCC 4.3.0 it is set to 20080305.
//       Although 4.3.0 was released earlier than 4.2.4, it has better C++0x support
//       than any release in the 4.2 series."
//
//  Another resource for understanding stdlibc++ features is:
//  http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#manual.intro.status.standard.200x
//
//  However, using the GCC version number fails when the compiler is clang since this
//  only ever claims to emulate GCC-4.2, see https://svn.boost.org/trac/boost/ticket/7473
//  for a long discussion on this issue.  What we can do though is use clang's __has_include
//  to detect the presence of a C++11 header that was introduced with a specific GCC release.
//  We still have to be careful though as many such headers were buggy and/or incomplete when
//  first introduced, so we only check for headers that were fully featured from day 1, and then
//  use that to infer the underlying GCC version:
//
#ifdef __clang__

#if __has_include(<experimental/any>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 50000
#elif __has_include(<shared_mutex>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40900
#elif __has_include(<ext/cmath>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40800
#elif __has_include(<chrono>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40700
#elif __has_include(<typeindex>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40600
#elif __has_include(<future>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40500
#elif  __has_include(<ratio>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40400
#elif __has_include(<array>)
#  define AUTOBOOST_LIBSTDCXX_VERSION 40300
#endif
//
//  GCC 4.8 and 9 add working versions of <atomic> and <regex> respectively.
//  However, we have no test for these as the headers were present but broken
//  in early GCC versions.
//
#endif

#if !defined(AUTOBOOST_LIBSTDCXX_VERSION)
#  define AUTOBOOST_LIBSTDCXX_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

//  C++0x headers in GCC 4.3.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40300) || !defined(AUTOBOOST_LIBSTDCXX11)
#  define AUTOBOOST_NO_CXX11_HDR_ARRAY
#  define AUTOBOOST_NO_CXX11_HDR_TUPLE
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET
#  define AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL
#endif

//  C++0x headers in GCC 4.4.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40400) || !defined(AUTOBOOST_LIBSTDCXX11)
#  define AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define AUTOBOOST_NO_CXX11_HDR_MUTEX
#  define AUTOBOOST_NO_CXX11_HDR_RATIO
#  define AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define AUTOBOOST_NO_CXX11_SMART_PTR
#else
#  define AUTOBOOST_HAS_TR1_COMPLEX_INVERSE_TRIG 
#  define AUTOBOOST_HAS_TR1_COMPLEX_OVERLOADS 
#endif

//  C++0x features in GCC 4.5.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40500) || !defined(AUTOBOOST_LIBSTDCXX11)
#  define AUTOBOOST_NO_CXX11_NUMERIC_LIMITS
#  define AUTOBOOST_NO_CXX11_HDR_FUTURE
#  define AUTOBOOST_NO_CXX11_HDR_RANDOM
#endif

//  C++0x features in GCC 4.6.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40600) || !defined(AUTOBOOST_LIBSTDCXX11)
#  define AUTOBOOST_NO_CXX11_HDR_TYPEINDEX
#  define AUTOBOOST_NO_CXX11_ADDRESSOF
#endif

//  C++0x features in GCC 4.7.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40700) || !defined(AUTOBOOST_LIBSTDCXX11)
// Note that although <chrono> existed prior to 4.7, "steady_clock" is spelled "monotonic_clock"
// so 4.7.0 is the first truely conforming one.
#  define AUTOBOOST_NO_CXX11_HDR_CHRONO
#  define AUTOBOOST_NO_CXX11_ALLOCATOR
#endif
//  C++0x features in GCC 4.8.0 and later
//
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40800) || !defined(AUTOBOOST_LIBSTDCXX11)
// Note that although <atomic> existed prior to gcc 4.8 it was largely unimplemented for many types:
#  define AUTOBOOST_NO_CXX11_HDR_ATOMIC
#  define AUTOBOOST_NO_CXX11_HDR_THREAD
#endif
#if (AUTOBOOST_LIBSTDCXX_VERSION < 40900) || !defined(AUTOBOOST_LIBSTDCXX11)
// Although <regex> is present and compilable against, the actual implementation is not functional
// even for the simplest patterns such as "\d" or "[0-9]". This is the case at least in gcc up to 4.8, inclusively.
#  define AUTOBOOST_NO_CXX11_HDR_REGEX
#endif

#if defined(__clang_major__) && ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ < 7)))
// As of clang-3.6, libstdc++ header <atomic> throws up errors with clang:
#  define AUTOBOOST_NO_CXX11_HDR_ATOMIC
#endif

//  C++0x headers not yet (fully!) implemented
//
#  define AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define AUTOBOOST_NO_CXX11_HDR_CODECVT
#  define AUTOBOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define AUTOBOOST_NO_CXX11_STD_ALIGN

#if (!defined(_GLIBCXX_HAS_GTHREADS) || !defined(_GLIBCXX_USE_C99_STDINT_TR1))
   // Headers not always available:
#  ifndef AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#     define AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  endif
#  ifndef AUTOBOOST_NO_CXX11_HDR_MUTEX
#     define AUTOBOOST_NO_CXX11_HDR_MUTEX
#  endif
#  ifndef AUTOBOOST_NO_CXX11_HDR_THREAD
#     define AUTOBOOST_NO_CXX11_HDR_THREAD
#  endif
#endif

#if (!defined(_GTHREAD_USE_MUTEX_TIMEDLOCK) || (_GTHREAD_USE_MUTEX_TIMEDLOCK == 0)) && !defined(AUTOBOOST_NO_CXX11_HDR_MUTEX)
// Timed mutexes are not always available:
#  define AUTOBOOST_NO_CXX11_HDR_MUTEX
#endif

//  --- end ---