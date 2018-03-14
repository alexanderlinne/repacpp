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

        template<class _Fn, ArrayView... _ArrayViews>
        struct zip_view
        {
            static_assert(Compatible<_ArrayViews...>);
            static_assert(Invocable<_Fn, element_type_t<_ArrayViews>...>);

            zip_view(_Fn&& fn, std::tuple<_ArrayViews...> arrays)
                : fn(std::forward<_Fn>(fn)), arrays(std::move(arrays))
            {
                check_extent(std::make_index_sequence<sizeof...(_ArrayViews)>());
            }

            auto extent() const
                -> decltype(auto)
            {
                return std::get<0>(arrays).extent();
            }

            auto operator[](Index const& idx) const
                -> decltype(auto)
            {
                return fn_impl(idx,
                    std::make_index_sequence<sizeof...(_ArrayViews)>());
            }

        private:
            template<size_t... I>
            void check_extent(std::index_sequence<I...>)
            {
                if (((std::get<I>(arrays).extent()
                    != std::get<0>(arrays).extent())
                    || ...))
                    throw std::exception{};
            }

            template<size_t... I>
            decltype(auto) fn_impl(Index const& idx,
                std::index_sequence<I...>) const
            {
                return fn((std::get<I>(arrays)[idx])...);
            }

        private:
            _Fn fn;
            std::tuple<_ArrayViews...> arrays;
        };

    } // namespace detail

    auto zip(auto&& fn, auto&&... arrays)
        -> Pipeable
    {
        return make_pipeable(
            [&, fn = std::forward<decltype(fn)>(fn)]
                (ArrayView&& array)
                -> DelayedView
            {
                return detail::zip_view{
                    std::forward<decltype(fn)>(fn),
                    std::make_tuple(
                        std::forward<decltype(array)>(array) | id(),
                        (std::forward<decltype(arrays)>(arrays) | id())...
                    )
                };
            });
    }

} // namespace repa::view

namespace repa {

    template<class _Fn, ArrayView... _ArrayViews>
    struct is_delayed<view::detail::zip_view<_Fn, _ArrayViews...>>
        : std::true_type {};

} // namespace repa
