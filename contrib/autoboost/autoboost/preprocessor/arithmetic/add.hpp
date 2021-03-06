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
# ifndef AUTOBOOST_PREPROCESSOR_ARITHMETIC_ADD_HPP
# define AUTOBOOST_PREPROCESSOR_ARITHMETIC_ADD_HPP
#
# include <autoboost/preprocessor/arithmetic/dec.hpp>
# include <autoboost/preprocessor/arithmetic/inc.hpp>
# include <autoboost/preprocessor/config/config.hpp>
# include <autoboost/preprocessor/control/while.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
#
# /* AUTOBOOST_PP_ADD */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ADD(x, y) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_ADD_P, AUTOBOOST_PP_ADD_O, (x, y)))
# else
#    define AUTOBOOST_PP_ADD(x, y) AUTOBOOST_PP_ADD_I(x, y)
#    define AUTOBOOST_PP_ADD_I(x, y) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE(AUTOBOOST_PP_ADD_P, AUTOBOOST_PP_ADD_O, (x, y)))
# endif
#
# define AUTOBOOST_PP_ADD_P(d, xy) AUTOBOOST_PP_TUPLE_ELEM(2, 1, xy)
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_MWCC()
#    define AUTOBOOST_PP_ADD_O(d, xy) AUTOBOOST_PP_ADD_O_I xy
# else
#    define AUTOBOOST_PP_ADD_O(d, xy) AUTOBOOST_PP_ADD_O_I(AUTOBOOST_PP_TUPLE_ELEM(2, 0, xy), AUTOBOOST_PP_TUPLE_ELEM(2, 1, xy))
# endif
#
# define AUTOBOOST_PP_ADD_O_I(x, y) (AUTOBOOST_PP_INC(x), AUTOBOOST_PP_DEC(y))
#
# /* AUTOBOOST_PP_ADD_D */
#
# if ~AUTOBOOST_PP_CONFIG_FLAGS() & AUTOBOOST_PP_CONFIG_EDG()
#    define AUTOBOOST_PP_ADD_D(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ADD_P, AUTOBOOST_PP_ADD_O, (x, y)))
# else
#    define AUTOBOOST_PP_ADD_D(d, x, y) AUTOBOOST_PP_ADD_D_I(d, x, y)
#    define AUTOBOOST_PP_ADD_D_I(d, x, y) AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PP_WHILE_ ## d(AUTOBOOST_PP_ADD_P, AUTOBOOST_PP_ADD_O, (x, y)))
# endif
#
# endif
