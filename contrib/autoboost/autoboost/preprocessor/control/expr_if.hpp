# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef AUTOBOOST_PREPROCESSOR_CONTROL_EXPR_IF_HPP
# define AUTOBOOST_PREPROCESSOR_CONTROL_EXPR_IF_HPP
#
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/expr_iif.hpp>
# include <autoboost/preprocessor/logical/bool.hpp>
#
# /* AUTOBOOST_PP_EXPR_IF */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_EXPR_IF(cond, expr) AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_BOOL(cond), expr)
# else
#    define AUTOBOOST_PP_EXPR_IF(cond, expr) AUTOBOOST_PP_EXPR_IF_I(cond, expr)
#    define AUTOBOOST_PP_EXPR_IF_I(cond, expr) AUTOBOOST_PP_EXPR_IIF(AUTOBOOST_PP_BOOL(cond), expr)
# endif
#
# endif
