// Copyright Kevlin Henney, 2000-2005.
// Copyright Alexander Nasonov, 2006-2010.
// Copyright Antony Polukhin, 2011-2014.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// what:  lexical_cast custom keyword cast
// who:   contributed by Kevlin Henney,
//        enhanced with contributions from Terje Slettebo,
//        with additional fixes and suggestions from Gennaro Prota,
//        Beman Dawes, Dave Abrahams, Daryle Walker, Peter Dimov,
//        Alexander Nasonov, Antony Polukhin, Justin Viiret, Michael Hofmann,
//        Cheng Yang, Matthew Bradbury, David W. Birdsall, Pavel Korzh and other Boosters
// when:  November 2000, March 2003, June 2005, June 2006, March 2011 - 2014

#ifndef AUTOBOOST_LEXICAL_CAST_DETAIL_CONVERTER_LEXICAL_HPP
#define AUTOBOOST_LEXICAL_CAST_DETAIL_CONVERTER_LEXICAL_HPP

#include <autoboost/config.hpp>
#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#if defined(AUTOBOOST_NO_STRINGSTREAM) || defined(AUTOBOOST_NO_STD_WSTRING)
#define AUTOBOOST_LCAST_NO_WCHAR_T
#endif

#include <cstddef>
#include <string>
#include <autoboost/limits.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/type_traits/ice.hpp>
#include <autoboost/type_traits/is_integral.hpp>
#include <autoboost/type_traits/is_float.hpp>
#include <autoboost/type_traits/has_left_shift.hpp>
#include <autoboost/type_traits/has_right_shift.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/detail/lcast_precision.hpp>

#include <autoboost/lexical_cast/detail/widest_char.hpp>
#include <autoboost/lexical_cast/detail/is_character.hpp>

#ifndef AUTOBOOST_NO_CXX11_HDR_ARRAY
#include <array>
#endif

#include <autoboost/array.hpp>
#include <autoboost/range/iterator_range_core.hpp>
#include <autoboost/container/container_fwd.hpp>

#include <autoboost/lexical_cast/detail/converter_lexical_streams.hpp>

namespace autoboost {

    namespace detail // normalize_single_byte_char<Char>
    {
        // Converts signed/unsigned char to char
        template < class Char >
        struct normalize_single_byte_char
        {
            typedef Char type;
        };

        template <>
        struct normalize_single_byte_char< signed char >
        {
            typedef char type;
        };

        template <>
        struct normalize_single_byte_char< unsigned char >
        {
            typedef char type;
        };
    }

    namespace detail // deduce_character_type_later<T>
    {
        // Helper type, meaning that stram character for T must be deduced
        // at Stage 2 (See deduce_source_char<T> and deduce_target_char<T>)
        template < class T > struct deduce_character_type_later {};
    }

    namespace detail // stream_char_common<T>
    {
        // Selectors to choose stream character type (common for Source and Target)
        // Returns one of char, wchar_t, char16_t, char32_t or deduce_character_type_later<T> types
        // Executed on Stage 1 (See deduce_source_char<T> and deduce_target_char<T>)
        template < typename Type >
        struct stream_char_common: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Type >::value,
            Type,
            autoboost::detail::deduce_character_type_later< Type >
        > {};

        template < typename Char >
        struct stream_char_common< Char* >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< Char* >
        > {};

        template < typename Char >
        struct stream_char_common< const Char* >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< const Char* >
        > {};

        template < typename Char >
        struct stream_char_common< autoboost::iterator_range< Char* > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< autoboost::iterator_range< Char* > >
        > {};

        template < typename Char >
        struct stream_char_common< autoboost::iterator_range< const Char* > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< autoboost::iterator_range< const Char* > >
        > {};

        template < class Char, class Traits, class Alloc >
        struct stream_char_common< std::basic_string< Char, Traits, Alloc > >
        {
            typedef Char type;
        };

        template < class Char, class Traits, class Alloc >
        struct stream_char_common< autoboost::container::basic_string< Char, Traits, Alloc > >
        {
            typedef Char type;
        };

        template < typename Char, std::size_t N >
        struct stream_char_common< autoboost::array< Char, N > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< autoboost::array< Char, N > >
        > {};

        template < typename Char, std::size_t N >
        struct stream_char_common< autoboost::array< const Char, N > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< autoboost::array< const Char, N > >
        > {};

#ifndef AUTOBOOST_NO_CXX11_HDR_ARRAY
        template < typename Char, std::size_t N >
        struct stream_char_common< std::array<Char, N > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< std::array< Char, N > >
        > {};

        template < typename Char, std::size_t N >
        struct stream_char_common< std::array< const Char, N > >: public autoboost::mpl::if_c<
            autoboost::detail::is_character< Char >::value,
            Char,
            autoboost::detail::deduce_character_type_later< std::array< const Char, N > >
        > {};
#endif

#ifdef AUTOBOOST_HAS_INT128
        template <> struct stream_char_common< autoboost::int128_type >: public autoboost::mpl::identity< char > {};
        template <> struct stream_char_common< autoboost::uint128_type >: public autoboost::mpl::identity< char > {};
#endif

#if !defined(AUTOBOOST_LCAST_NO_WCHAR_T) && defined(AUTOBOOST_NO_INTRINSIC_WCHAR_T)
        template <>
        struct stream_char_common< wchar_t >
        {
            typedef char type;
        };
#endif
    }

    namespace detail // deduce_source_char_impl<T>
    {
        // If type T is `deduce_character_type_later` type, then tries to deduce
        // character type using autoboost::has_left_shift<T> metafunction.
        // Otherwise supplied type T is a character type, that must be normalized
        // using normalize_single_byte_char<Char>.
        // Executed at Stage 2  (See deduce_source_char<T> and deduce_target_char<T>)
        template < class Char >
        struct deduce_source_char_impl
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::normalize_single_byte_char< Char >::type type;
        };

        template < class T >
        struct deduce_source_char_impl< deduce_character_type_later< T > >
        {
            typedef autoboost::has_left_shift< std::basic_ostream< char >, T > result_t;

#if defined(AUTOBOOST_LCAST_NO_WCHAR_T)
            AUTOBOOST_STATIC_ASSERT_MSG((result_t::value),
                "Source type is not std::ostream`able and std::wostream`s are not supported by your STL implementation");
            typedef char type;
#else
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_c<
                result_t::value, char, wchar_t
            >::type type;

            AUTOBOOST_STATIC_ASSERT_MSG((result_t::value || autoboost::has_left_shift< std::basic_ostream< type >, T >::value),
                "Source type is neither std::ostream`able nor std::wostream`able");
#endif
        };
    }

    namespace detail  // deduce_target_char_impl<T>
    {
        // If type T is `deduce_character_type_later` type, then tries to deduce
        // character type using autoboost::has_right_shift<T> metafunction.
        // Otherwise supplied type T is a character type, that must be normalized
        // using normalize_single_byte_char<Char>.
        // Executed at Stage 2  (See deduce_source_char<T> and deduce_target_char<T>)
        template < class Char >
        struct deduce_target_char_impl
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME normalize_single_byte_char< Char >::type type;
        };

        template < class T >
        struct deduce_target_char_impl< deduce_character_type_later<T> >
        {
            typedef autoboost::has_right_shift<std::basic_istream<char>, T > result_t;

#if defined(AUTOBOOST_LCAST_NO_WCHAR_T)
            AUTOBOOST_STATIC_ASSERT_MSG((result_t::value),
                "Target type is not std::istream`able and std::wistream`s are not supported by your STL implementation");
            typedef char type;
#else
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_c<
                result_t::value, char, wchar_t
            >::type type;

            AUTOBOOST_STATIC_ASSERT_MSG((result_t::value || autoboost::has_right_shift<std::basic_istream<wchar_t>, T >::value),
                "Target type is neither std::istream`able nor std::wistream`able");
#endif
        };
    }

    namespace detail  // deduce_target_char<T> and deduce_source_char<T>
    {
        // We deduce stream character types in two stages.
        //
        // Stage 1 is common for Target and Source. At Stage 1 we get
        // non normalized character type (may contain unsigned/signed char)
        // or deduce_character_type_later<T> where T is the original type.
        // Stage 1 is executed by stream_char_common<T>
        //
        // At Stage 2 we normalize character types or try to deduce character
        // type using metafunctions.
        // Stage 2 is executed by deduce_target_char_impl<T> and
        // deduce_source_char_impl<T>
        //
        // deduce_target_char<T> and deduce_source_char<T> functions combine
        // both stages

        template < class T >
        struct deduce_target_char
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME stream_char_common< T >::type stage1_type;
            typedef AUTOBOOST_DEDUCED_TYPENAME deduce_target_char_impl< stage1_type >::type stage2_type;

            typedef stage2_type type;
        };

        template < class T >
        struct deduce_source_char
        {
            typedef AUTOBOOST_DEDUCED_TYPENAME stream_char_common< T >::type stage1_type;
            typedef AUTOBOOST_DEDUCED_TYPENAME deduce_source_char_impl< stage1_type >::type stage2_type;

            typedef stage2_type type;
        };
    }

    namespace detail // extract_char_traits template
    {
        // We are attempting to get char_traits<> from T
        // template parameter. Otherwise we'll be using std::char_traits<Char>
        template < class Char, class T >
        struct extract_char_traits
                : autoboost::false_type
        {
            typedef std::char_traits< Char > trait_t;
        };

        template < class Char, class Traits, class Alloc >
        struct extract_char_traits< Char, std::basic_string< Char, Traits, Alloc > >
            : autoboost::true_type
        {
            typedef Traits trait_t;
        };

        template < class Char, class Traits, class Alloc>
        struct extract_char_traits< Char, autoboost::container::basic_string< Char, Traits, Alloc > >
            : autoboost::true_type
        {
            typedef Traits trait_t;
        };
    }

    namespace detail // array_to_pointer_decay<T>
    {
        template<class T>
        struct array_to_pointer_decay
        {
            typedef T type;
        };

        template<class T, std::size_t N>
        struct array_to_pointer_decay<T[N]>
        {
            typedef const T * type;
        };
    }

    namespace detail // is_this_float_conversion_optimized<Float, Char>
    {
        // this metafunction evaluates to true, if we have optimized comnversion
        // from Float type to Char array.
        // Must be in sync with lexical_stream_limited_src<Char, ...>::shl_real_type(...)
        template <typename Float, typename Char>
        struct is_this_float_conversion_optimized
        {
            typedef autoboost::type_traits::ice_and<
                autoboost::is_float<Float>::value,
#if !defined(AUTOBOOST_LCAST_NO_WCHAR_T) && !defined(AUTOBOOST_NO_SWPRINTF) && !defined(__MINGW32__)
                autoboost::type_traits::ice_or<
                    autoboost::type_traits::ice_eq<sizeof(Char), sizeof(char) >::value,
                    autoboost::is_same<Char, wchar_t>::value
                >::value
#else
                autoboost::type_traits::ice_eq<sizeof(Char), sizeof(char) >::value
#endif
            > result_type;

            AUTOBOOST_STATIC_CONSTANT(bool, value = (result_type::value) );
        };
    }

    namespace detail // lcast_src_length
    {
        // Return max. length of string representation of Source;
        template< class Source,         // Source type of lexical_cast.
                  class Enable = void   // helper type
                >
        struct lcast_src_length
        {
            AUTOBOOST_STATIC_CONSTANT(std::size_t, value = 1);
        };

        // Helper for integral types.
        // Notes on length calculation:
        // Max length for 32bit int with grouping "\1" and thousands_sep ',':
        // "-2,1,4,7,4,8,3,6,4,7"
        //  ^                    - is_signed
        //   ^                   - 1 digit not counted by digits10
        //    ^^^^^^^^^^^^^^^^^^ - digits10 * 2
        //
        // Constant is_specialized is used instead of constant 1
        // to prevent buffer overflow in a rare case when
        // <autoboost/limits.hpp> doesn't add missing specialization for
        // numeric_limits<T> for some integral type T.
        // When is_specialized is false, the whole expression is 0.
        template <class Source>
        struct lcast_src_length<
                    Source, AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if<autoboost::is_integral<Source> >::type
                >
        {
#ifndef AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
            AUTOBOOST_STATIC_CONSTANT(std::size_t, value =
                  std::numeric_limits<Source>::is_signed +
                  std::numeric_limits<Source>::is_specialized + /* == 1 */
                  std::numeric_limits<Source>::digits10 * 2
              );
#else
            AUTOBOOST_STATIC_CONSTANT(std::size_t, value = 156);
            AUTOBOOST_STATIC_ASSERT(sizeof(Source) * CHAR_BIT <= 256);
#endif
        };

#ifndef AUTOBOOST_LCAST_NO_COMPILE_TIME_PRECISION
        // Helper for floating point types.
        // -1.23456789e-123456
        // ^                   sign
        //  ^                  leading digit
        //   ^                 decimal point
        //    ^^^^^^^^         lcast_precision<Source>::value
        //            ^        "e"
        //             ^       exponent sign
        //              ^^^^^^ exponent (assumed 6 or less digits)
        // sign + leading digit + decimal point + "e" + exponent sign == 5
        template<class Source>
        struct lcast_src_length<
                Source, AUTOBOOST_DEDUCED_TYPENAME autoboost::enable_if<autoboost::is_float<Source> >::type
            >
        {
            AUTOBOOST_STATIC_ASSERT(
                    std::numeric_limits<Source>::max_exponent10 <=  999999L &&
                    std::numeric_limits<Source>::min_exponent10 >= -999999L
                );

            AUTOBOOST_STATIC_CONSTANT(std::size_t, value =
                    5 + lcast_precision<Source>::value + 6
                );
        };
#endif // #ifndef AUTOBOOST_LCAST_NO_COMPILE_TIME_PRECISION
    }

    namespace detail // lexical_cast_stream_traits<Source, Target>
    {
        template <class Source, class Target>
        struct lexical_cast_stream_traits {
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::array_to_pointer_decay<Source>::type src;
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::remove_cv<src>::type            no_cv_src;

            typedef autoboost::detail::deduce_source_char<no_cv_src>                           deduce_src_char_metafunc;
            typedef AUTOBOOST_DEDUCED_TYPENAME deduce_src_char_metafunc::type           src_char_t;
            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::deduce_target_char<Target>::type target_char_t;

            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::widest_char<
                target_char_t, src_char_t
            >::type char_type;

#if !defined(AUTOBOOST_NO_CXX11_CHAR16_T) && defined(AUTOBOOST_NO_CXX11_UNICODE_LITERALS)
            AUTOBOOST_STATIC_ASSERT_MSG(( !autoboost::is_same<char16_t, src_char_t>::value
                                        && !autoboost::is_same<char16_t, target_char_t>::value),
                "Your compiler does not have full support for char16_t" );
#endif
#if !defined(AUTOBOOST_NO_CXX11_CHAR32_T) && defined(AUTOBOOST_NO_CXX11_UNICODE_LITERALS)
            AUTOBOOST_STATIC_ASSERT_MSG(( !autoboost::is_same<char32_t, src_char_t>::value
                                        && !autoboost::is_same<char32_t, target_char_t>::value),
                "Your compiler does not have full support for char32_t" );
#endif

            typedef AUTOBOOST_DEDUCED_TYPENAME autoboost::mpl::if_c<
                autoboost::detail::extract_char_traits<char_type, Target>::value,
                AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::extract_char_traits<char_type, Target>,
                AUTOBOOST_DEDUCED_TYPENAME autoboost::detail::extract_char_traits<char_type, no_cv_src>
            >::type::trait_t traits;

            typedef autoboost::type_traits::ice_and<
                autoboost::is_same<char, src_char_t>::value,                                  // source is not a wide character based type
                autoboost::type_traits::ice_ne<sizeof(char), sizeof(target_char_t) >::value,  // target type is based on wide character
                autoboost::type_traits::ice_not<
                    autoboost::detail::is_character<no_cv_src>::value                     // single character widening is optimized
                >::value                                                                  // and does not requires stringbuffer
            >   is_string_widening_required_t;

            typedef autoboost::type_traits::ice_not< autoboost::type_traits::ice_or<
                autoboost::is_integral<no_cv_src>::value,
                autoboost::detail::is_this_float_conversion_optimized<no_cv_src, char_type >::value,
                autoboost::detail::is_character<
                    AUTOBOOST_DEDUCED_TYPENAME deduce_src_char_metafunc::stage1_type          // if we did not get character type at stage1
                >::value                                                                  // then we have no optimization for that type
            >::value >   is_source_input_not_optimized_t;

            // If we have an optimized conversion for
            // Source, we do not need to construct stringbuf.
            AUTOBOOST_STATIC_CONSTANT(bool, requires_stringbuf =
                (autoboost::type_traits::ice_or<
                    is_string_widening_required_t::value, is_source_input_not_optimized_t::value
                >::value)
            );

            typedef autoboost::detail::lcast_src_length<no_cv_src> len_t;
        };
    }

    namespace detail
    {
        template<typename Target, typename Source>
        struct lexical_converter_impl
        {
            typedef lexical_cast_stream_traits<Source, Target>  stream_trait;

            typedef detail::lexical_istream_limited_src<
                AUTOBOOST_DEDUCED_TYPENAME stream_trait::char_type,
                AUTOBOOST_DEDUCED_TYPENAME stream_trait::traits,
                stream_trait::requires_stringbuf,
                stream_trait::len_t::value + 1
            > i_interpreter_type;

            typedef detail::lexical_ostream_limited_src<
                AUTOBOOST_DEDUCED_TYPENAME stream_trait::char_type,
                AUTOBOOST_DEDUCED_TYPENAME stream_trait::traits
            > o_interpreter_type;

            static inline bool try_convert(const Source& arg, Target& result) {
                i_interpreter_type i_interpreter;

                // Disabling ADL, by directly specifying operators.
                if (!(i_interpreter.operator <<(arg)))
                    return false;

                o_interpreter_type out(i_interpreter.cbegin(), i_interpreter.cend());

                // Disabling ADL, by directly specifying operators.
                if(!(out.operator >>(result)))
                    return false;

                return true;
            }
        };
    }

} // namespace autoboost

#undef AUTOBOOST_LCAST_NO_WCHAR_T

#endif // AUTOBOOST_LEXICAL_CAST_DETAIL_CONVERTER_LEXICAL_HPP

