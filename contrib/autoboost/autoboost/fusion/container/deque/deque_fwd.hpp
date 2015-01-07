/*=============================================================================
    Copyright (c) 2005-2012 Joel de Guzman
    Copyright (c) 2005-2007 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_DEQUE_FORWARD_02092007_0749)
#define FUSION_DEQUE_FORWARD_02092007_0749

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/config.hpp>

#if (defined(AUTOBOOST_NO_CXX11_DECLTYPE)             \
  || defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)   \
  || defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES))   \
  || (defined(__WAVE__) && defined(AUTOBOOST_FUSION_CREATE_PREPROCESSED_FILES))
# if defined(AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE)
#   undef AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE
# endif
#else
# if !defined(AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE)
#   define AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE
# endif
#endif

///////////////////////////////////////////////////////////////////////////////
// With no decltype and variadics, we will use the C++03 version
///////////////////////////////////////////////////////////////////////////////
#if !defined(AUTOBOOST_FUSION_HAS_VARIADIC_DEQUE)
# include <autoboost/fusion/container/deque/detail/cpp03/deque_fwd.hpp>
#else

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
namespace autoboost { namespace fusion
{
    template <typename ...T>
    struct deque;
}}

#endif
#endif