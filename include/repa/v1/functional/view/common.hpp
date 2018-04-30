//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/functional/view/zip.hpp"

namespace repa::view {

    namespace detail {

    } // namespace detail

    template<ArrayView... _ArrayViews>
    auto add(_ArrayViews&&... arrays)
        -> Pipeable
    {
        return zip([](auto&&... v) -> decltype(auto) { return (v + ...); },
            std::forward<_ArrayViews>(arrays)...);
    }

    template<ArrayView... _ArrayViews>
    auto subtract(_ArrayViews&&... arrays)
        -> Pipeable
    {
        return zip([](auto&&... v) -> decltype(auto) { return (v - ...); },
            std::forward<_ArrayViews>(arrays)...);
    }

    template<ArrayView... _ArrayViews>
    auto multiply(_ArrayViews&&... arrays)
        -> Pipeable
    {
        return zip([](auto&&... v) -> decltype(auto) { return (v * ...); },
            std::forward<_ArrayViews>(arrays)...);
    }

    namespace detail {

        auto transpose_swap(Index const& idx)
            -> Index
        {
            auto result = make_index(idx);
            std::swap(get<0>(result), get<1>(result));
            return result;
        }

    } // namespace detail

    auto transpose()
        -> Pipeable
    {
        return backpermute(
            [](auto&& extent) { return detail::transpose_swap(extent); },
            [](auto&& index) { return detail::transpose_swap(index); });
    }

    template<size_t Dim>
    auto extend(size_t size) {
        return traverse(
            [size = size](repa::Index&& extent) {
                return add_dimension<Dim>(extent, size);
            },
            [](repa::ArrayView&& array, repa::Index&& index) {
                return array[remove_dimension<Dim>(index)];
            });
    }

} // namespace repa::view
