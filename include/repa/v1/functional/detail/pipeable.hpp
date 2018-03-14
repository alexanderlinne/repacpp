//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"
#include "repa/v1/meta/concepts.hpp"

namespace repa {

    namespace detail {

        struct pipeable_base {};

    } // namespace detail

    template<class T>
    concept bool Pipeable = BaseOf<detail::pipeable_base, remove_cvref_t<T>>;

    namespace detail {

        struct access_pipeable {
            template<Pipeable _Pipeable>
            struct impl : _Pipeable {
                using _Pipeable::pipe;
            };

            template<Pipeable _Pipeable>
            struct impl<_Pipeable&> : impl<_Pipeable> {};
        };

        struct pipeable : pipeable_base {
        private:
            friend access_pipeable;

            template<repa::ArrayView _ArrayView, Pipeable _Pipeable>
            static auto pipe(_ArrayView&& array, _Pipeable pipeable)
            requires UnaryFunction<_Pipeable, _ArrayView>
            {
                return pipeable(std::forward<decltype(array)>(array));
            }
        };

        template<class _Bindable>
        struct pipeable_binder : _Bindable, pipeable {
            pipeable_binder(_Bindable&& bind) : _Bindable(std::move(bind)) {}
        };

    } // namespace detail

    template<class Fn>
    auto make_pipeable(Fn&& fn)
        -> repa::Pipeable
    {
        return detail::pipeable_binder<Fn>{std::move(fn)};
    }

} // namespace repa

template<repa::ArrayView _ArrayView, repa::Pipeable _Pipeable>
auto operator|(_ArrayView&& array, _Pipeable pipeable)
    -> repa::ArrayView
requires repa::UnaryFunction<_Pipeable, _ArrayView>
{
    return repa::detail::access_pipeable::impl<_Pipeable>::pipe(
        std::forward<_ArrayView>(array), pipeable);
}

auto operator|(repa::Pipeable l, repa::Pipeable r)
    -> repa::Pipeable
{
    return repa::make_pipeable([=](repa::ArrayView&& array) {
        std::forward<decltype(array)>(array) | l | r;
    });
}
