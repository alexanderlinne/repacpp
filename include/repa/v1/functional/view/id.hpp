//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"
#include "repa/v1/functional/detail/pipeable.hpp"

namespace repa::view {

    namespace detail {

        template<ArrayView _ArrayView>
        struct id_view {
            id_view(_ArrayView array) : array(std::move(array)) {};

            auto extent() const
                -> decltype(auto)
            {
                return array.extent();
            }

            auto operator[](Index const& idx) const
                -> decltype(auto)
            {
                return array[idx];
            }

            auto operator[](Index const& idx)
                -> decltype(auto)
            requires Array<_ArrayView>
            {
                return array[idx];
            }

        private:
            _ArrayView array;
        };

    } // namespace detail

    auto id()
        -> Pipeable
    {
        return make_pipeable(
            [](ArrayView&& array)
                -> ArrayView
            {
                return detail::id_view{std::forward<decltype(array)>(array)};
            });
    }

} // namespace repa::view

namespace repa {

    template<ArrayView _ArrayView>
    struct is_delayed<view::detail::id_view<_ArrayView>>
        : is_delayed<_ArrayView> {};

} // namespace repa
