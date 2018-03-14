//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <functional>

#include "repa/v1/core/array_concept.hpp"
#include "repa/v1/core/index_util.hpp"
#include "repa/v1/functional/detail/pipeable.hpp"
#include "repa/v1/functional/view/id.hpp"

namespace repa::view {

    namespace detail {

        template<class _Fn, ArrayView _ArrayView>
        struct fold_view
        {
            static_assert(rank_v<_ArrayView> > 1);
            static_assert(Invocable<_Fn,
                element_type_t<_ArrayView>,
                element_type_t<_ArrayView>>);

            fold_view(_Fn&& fn, _ArrayView array, element_type_t<_ArrayView> v)
                : fn(std::forward<_Fn>(fn)),
                array(std::move(array)),
                v(v),
                prev_extent(this->array.extent())
            {
                init();
            };

            auto extent() const
                -> decltype(auto)
            {
                return _extent;
            }

            auto operator[](Index const& idx) const
                -> decltype(auto)
            {
                auto result = v;
                for (element_type_t<index_type> i = 0;
                    i < get<rank_v<index_type>-1>(prev_extent);
                    ++i)
                {
                    result = fn(result, array[add_dimension(idx, i)]);
                }
                return result;
            }

        private:
            using index_type = index_type_t<_ArrayView>;

            void init() {
                _extent = extent_impl(
                    std::make_index_sequence<rank_v<index_type>-1>());
            }

            template<size_t... I>
            auto extent_impl(std::index_sequence<I...>) const {
                return make_index(get<I>(prev_extent)...);
            }

        private:
            _Fn fn;
            _ArrayView array;
            element_type_t<_ArrayView> v;

            decltype(array.extent()) prev_extent;
            index<element_type_t<index_type>, rank_v<index_type> - 1> _extent;
        };

    } // namespace detail

    auto fold(auto&& fn, auto v)
        -> Pipeable
    {
        return make_pipeable(
            [fn = std::forward<decltype(fn)>(fn), v = v]
                (ArrayView&& array)
                -> ArrayView
            {
                static_assert(rank_v<decltype(array)> > 1);
                return detail::fold_view{
                    std::forward<decltype(fn)>(fn),
                    std::forward<decltype(array)>(array) | id(),
                    element_type_t<decltype(array)>{v}
                };
            });
    }

} // namespace repa::view

namespace repa {

    template<class _Fn, ArrayView _ArrayView>
    struct is_delayed<view::detail::fold_view<_Fn, _ArrayView>>
        : std::true_type {};

} // namespace repa
