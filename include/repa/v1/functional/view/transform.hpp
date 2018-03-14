//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"
#include "repa/v1/functional/detail/pipeable.hpp"
#include "repa/v1/functional/view/id.hpp"
#include "repa/v1/meta/concepts.hpp"

namespace repa::view {

    namespace detail {

        template<class _Fn, ArrayView _ArrayView>
        struct transform_view
        {
            static_assert(UnaryFunction<_Fn, element_type_t<_ArrayView>>);

            // ctor

            transform_view(_Fn&& fn, _ArrayView array)
                : fn(std::forward<_Fn>(fn)), array(std::move(array)) {};

            // Matrix concept implementation

            auto extent() const
                -> decltype(auto)
            {
                return array.extent();
            }

            auto operator[](Index const& p) const
                -> decltype(auto)
            {
                return fn(array[p]);
            }

        private:
            _Fn fn;
            _ArrayView array;
        };

    } // namespace detail


    auto transform(auto&& fn)
        -> Pipeable
    {
        return make_pipeable(
            [fn = std::forward<decltype(fn)>(fn)]
                (ArrayView&& array)
                -> DelayedView
            {
                return detail::transform_view{
                    std::forward<decltype(fn)>(fn),
                    std::forward<decltype(array)>(array) | id()
                };
            });
    }

} // namespace repa:view

namespace repa {

    template<class _Fn, ArrayView _ArrayView>
    struct is_delayed<view::detail::transform_view<_Fn, _ArrayView>>
        : std::true_type {};

} // namespace repa
