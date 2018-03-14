//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>

#include "repa/v1/core/index.hpp"
#include "repa/v1/meta/type_traits.hpp"

namespace repa {

    // defines if a Array or ArrayView type is delayed
    template<class T>
    struct is_delayed;
    template<class T>
    struct is_delayed<T&> : is_delayed<T> {};
    template<class T>
    struct is_delayed<T&&> : is_delayed<T> {};
    template<class T>
    struct is_delayed<const T> : is_delayed<T>{};
    template<class T>
    struct is_delayed<volatile T> : is_delayed<T>{};
    template<class T>
    struct is_delayed<const volatile T> : is_delayed<T>{};
    template<class T>
    inline constexpr bool is_delayed_v = is_delayed<T>::value;

    namespace detail {

        template<class T>
        using _index_t =
            index<size_t, rank_v<decltype(std::declval<T>().extent())>>;

        template<class T>
        requires Complete<is_delayed<remove_cvref_t<T>>>
        concept bool _ArrayViewImpl =
            requires {
                { is_delayed_v<T> } -> bool;
            } &&
            requires(const T a, const detail::_index_t<T> p) {
                { a.extent() } -> Index;
                { a[p] } -> Object;
            };

    } // namespace detail

    //
    // concept ArrayView
    //
    template<class T>
    concept bool ArrayView =
        MoveConstructible<remove_cvref_t<T>> &&
        Complete<is_delayed<remove_cvref_t<T>>> &&
        detail::_ArrayViewImpl<T>;

    // If T is an ArrayView type, provides the member constant value equal to
    // the number of dimensions of the array.
    template<ArrayView T>
    struct rank<T> : rank<decltype(std::declval<T>().extent())> {};

    // If T is an ArrayView type, provides the member typedef type equal to
    // the return type of T::at(repa::index<size_t, rank_v<T>>) without cvref
    // qualifiers.
    template<ArrayView T>
    struct element_type<T> {
        using type = remove_cvref_t<decltype(std::declval<T>()[
            std::declval<repa::index<size_t, rank_v<T>>>()])>;
    };

    //
    // concept Array
    //
    template<class T>
    concept bool Array =
        ArrayView<T> &&
        requires(T a, const detail::_index_t<T> p, element_type_t<T> v) {
            { a[p] = v };
        };

    namespace detail {

        // see Index_check_same_types implementation fro reasoning
        template<class T>
        requires ArrayView<T>
        inline constexpr bool safe_is_delayed_v = is_delayed_v<T>;

    } // namespace detail

    //
    // concept DelayedView
    //
    template<class T>
    concept bool DelayedView = ArrayView<T> && detail::safe_is_delayed_v<T>;

    //
    // concept ManifestView
    //
    template<class T>
    concept bool ManifestView = ArrayView<T> && !detail::safe_is_delayed_v<T>;

    //
    // concept DelayedArray
    //
    template<class T>
    concept bool DelayedArray = Array<T> && DelayedView<T>;

    //
    // concept ManifestArray
    //
    template<class T>
    concept bool ManifestArray = Array<T> && ManifestView<T>;

    template<class T>
    struct index_type;
    template<class T>
    using index_type_t = _t<index_type<T>>;

    // If T is an ArrayView type, provides the member typedef type equal to
    // the return type of T::extent
    template<ArrayView _ArrayView>
    struct index_type<_ArrayView> {
        using type = remove_cvref_t<decltype(std::declval<_ArrayView>().extent())>;
    };

} // namespace repa
