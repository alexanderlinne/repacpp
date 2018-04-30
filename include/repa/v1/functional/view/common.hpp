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
        return make_pipeable(
            [](ArrayView&& array)
                -> DelayedView
            {
                static_assert(rank_v<decltype(array)> >= 2);
                return std::forward<decltype(array)>(array)
                    | backpermute(detail::transpose_swap(array.extent()),
                        [](auto&& idx) {
                            return detail::transpose_swap(idx);
                        });
            });
    }

} // namespace repa::view
