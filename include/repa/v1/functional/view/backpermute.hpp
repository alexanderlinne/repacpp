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

        template<Index _Index, class _IndexFn, ArrayView _ArrayView>
        struct backpermute_view
        {
            static_assert(Invocable<_IndexFn, _Index const&>);

            backpermute_view(_Index const& extent,
                _IndexFn&& index_fn, _ArrayView array)
                : _extent(extent),
                index_fn(std::forward<_IndexFn>(index_fn)),
                array(std::move(array))
            {};

            auto extent() const
                -> Index const&
            {
                return _extent;
            }

            auto operator[](Index const& idx) const
                -> decltype(auto)
            {
                return array[index_fn(idx)];
            }

        private:
            _Index _extent;
            _IndexFn index_fn;
            _ArrayView array;
        };

    } // namespace detail

    template<class _ExtentFn, class _IndexFn>
    auto backpermute(_ExtentFn&& extent_fn, _IndexFn&& index_fn)
        -> Pipeable
    {
        return make_pipeable(
            [&, extent_fn = std::forward<_ExtentFn>(extent_fn),
                index_fn = std::forward<_IndexFn>(index_fn)]
                (ArrayView&& array)
                -> DelayedView
            {
                return detail::backpermute_view{
                    extent_fn(array.extent()),
                    std::forward<decltype(index_fn)>(index_fn),
                    std::forward<decltype(array)>(array)
                };
            });
    }

} // namespace repa::view

namespace repa {

    template<Index _Index, class _ExtentFn, ArrayView _ArrayView>
    struct is_delayed<view::detail::backpermute_view<_Index, _ExtentFn, _ArrayView>>
        : std::true_type {};

} // namespace repa
