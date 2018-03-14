//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/index.hpp"
#include "repa/v1/meta/concepts.hpp"

namespace repa {

    namespace detail {

        template<size_t... I>
        constexpr
        auto contains_impl(Index const& extent, Index const& index,
            std::index_sequence<I...>)
            -> bool
        {
            return ((get<I>(index) < get<I>(extent)) && ...);
        }

    } // namespace detail

    // Returns true if index would be a valid index for an array with the
    // given extent.
    template<Index _Extent, Index _Index>
    constexpr
    auto contains(_Extent const& extent, _Index const& index)
        -> bool
    requires
        Compatible<_Extent, _Index> &&
        LessThanComparable2<element_type_t<_Extent>, element_type_t<_Index>>
    {
        return detail::contains_impl(extent, index,
            std::make_index_sequence<rank_v<_Extent>>());
    }

    // Increments index within the given extent.
    // Returns false if the resulting index is not within the given extent.
    template<Index _Extent, Index _Index>
    constexpr
    bool next(_Extent const& extent, _Index& index)
    requires
        Compatible<_Extent, _Index>
    {
        auto i = rank_v<_Extent>;
        for (; i > 0; --i) {
            if (index[i-1] + 1 >= extent[i-1]) {
                index[i-1] = 0;
            } else {
                index[i-1]++;
                break;
            }
        }
        return i != 0 && contains(extent, index);
    }

    namespace detail {

        template<size_t... I>
        constexpr
        auto add_dimension_impl(Index const& index,
            element_type_t<decltype(index)> value,
            std::index_sequence<I...>)
            -> Index
        {
            return make_index(get<I>(index)..., value);
        }

    } // namespace detail

    // Increases the rank of index by one and initializes the new dimension
    // with the given value.
    constexpr
    auto add_dimension(Index const& index,
        element_type_t<decltype(index)> value)
        -> Index
    {
        return detail::add_dimension_impl(index, value,
            std::make_index_sequence<rank_v<decltype(index)>>());
    }

} // namespace repa
