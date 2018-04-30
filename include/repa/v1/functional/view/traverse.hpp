//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"
#include "repa/v1/core/index_util.hpp"
#include "repa/v1/functional/detail/pipeable.hpp"
#include "repa/v1/functional/view/id.hpp"

namespace repa::view {

    namespace detail {

        template<Index _Extent, class _AtFn, ArrayView _ArrayView>
        struct traverse_view
        {
            static_assert(Invocable<_AtFn, _ArrayView, _Extent>);

            traverse_view(_Extent const& extent, _AtFn&& at_fn, _ArrayView array)
                : _extent(extent),
                at_fn(std::forward<_AtFn>(at_fn)),
                array(std::move(array))
            {};

            auto extent() const
                -> decltype(auto)
            {
                return _extent;
            }

            auto operator[](Index const& idx) const
                -> decltype(auto)
            {
                return at_fn(array, idx);
            }

        private:
            _Extent _extent;
            std::decay_t<_AtFn> at_fn;
            _ArrayView array;
        };

    } // namespace detail

    auto traverse(auto&& extent_fn, auto&& at_fn)
        -> Pipeable
    {
        return make_pipeable(
            [extent_fn = std::forward<decltype(extent_fn)>(extent_fn),
                at_fn = std::forward<decltype(at_fn)>(at_fn)]
                (ArrayView&& array)
                -> DelayedView
            {
                return detail::traverse_view{
                    extent_fn(array.extent()),
                    std::forward<decltype(at_fn)>(at_fn),
                    std::forward<decltype(array)>(array) | id()
                };
            });
    }

} // namespace repa::view

namespace repa {

    template<class _ExtentFn, class _AtFn, ArrayView _ArrayView>
    struct is_delayed<view::detail::traverse_view<_ExtentFn, _AtFn, _ArrayView>>
        : std::true_type {};

} // namespace repa
