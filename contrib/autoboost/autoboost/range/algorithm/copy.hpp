//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef AUTOBOOST_RANGE_ALGORITHM_COPY_HPP_INCLUDED
#define AUTOBOOST_RANGE_ALGORITHM_COPY_HPP_INCLUDED

#include <autoboost/concept_check.hpp>
#include <autoboost/range/begin.hpp>
#include <autoboost/range/end.hpp>
#include <autoboost/range/concepts.hpp>
#include <autoboost/range/iterator_range.hpp>
#include <algorithm>

namespace autoboost
{
    namespace range
    {

/// \brief template function copy
///
/// range-based version of the copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre OutputIterator is a model of the OutputIteratorConcept
template< class SinglePassRange, class OutputIterator >
inline OutputIterator copy(const SinglePassRange& rng, OutputIterator out)
{
    AUTOBOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::copy(autoboost::begin(rng),autoboost::end(rng),out);
}

    } // namespace range
    using range::copy;
} // namespace autoboost

#endif // include guard
