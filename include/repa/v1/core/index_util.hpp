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

        template<size_t... I1, size_t... I2, Index _Index>
        constexpr
        auto add_dimension_impl(_Index const& index,
            element_type_t<_Index> value,
            std::index_sequence<I1...>,
            std::index_sequence<I2...>)
            -> Index
        {
            return make_index(get<I1>(index)..., value, get<I2>(index)...);
        }

        template<size_t... I1, size_t... I2>
        constexpr
        auto remove_dimension_impl(Index const& index,
            std::index_sequence<I1...>,
            std::index_sequence<I2...>)
            -> Index
        {
            return make_index(get<I1>(index)..., get<I2>(index)...);
        }

    } // namespace detail

    template<size_t Dim, Index _Index>
    constexpr
    auto add_dimension(_Index const& index, element_type_t<_Index> value)
        -> Index
    {
        static_assert(Dim <= rank_v<_Index>);
        return detail::add_dimension_impl(index, value,
            std::make_index_sequence<Dim>(),
            make_index_sequence<Dim, rank_v<_Index>>());
    }

    template<size_t Dim, Index _Index>
    constexpr
    auto remove_dimension(_Index const& index)
        -> Index
    {
        static_assert(Dim < rank_v<_Index>);
        return detail::remove_dimension_impl(index,
            std::make_index_sequence<Dim>(),
            make_index_sequence<Dim + 1, rank_v<_Index>>());
    }

    // Increases the rank of index by one and initializes the new dimension
    // with the given value.
    template<Index _Index>
    constexpr
    auto add_dimension(_Index const& index,
        element_type_t<decltype(index)> value)
        -> Index
    {
        return add_dimension<rank_v<_Index>>(index, value);
    }

} // namespace repa
