//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <utility>

#include "repa/v1/meta/concepts.hpp"
#include "repa/v1/meta/type_traits.hpp"

namespace repa {

    namespace detail {

        template<class T, size_t N>
        concept bool _check_dimensions_concept =
            requires {
                typename std::tuple_element<N, remove_reference_t<T>>::type;
            } &&
            requires(const remove_reference_t<T> a) {
                { a.template get<N>() };
            };

        template<class T, size_t... I>
        constexpr
        auto _check_dimensions_impl(std::index_sequence<I...>)
            -> bool
        {
            return (_check_dimensions_concept<T, I> && ...);
        };

        template<class T>
        // use require as gcc doesn't seem to short-circuit conjunctions
        // within concepts as described here:
        // http://en.cppreference.com/w/cpp/language/constraints#Conjunctions
        requires Complete<std::tuple_size<remove_reference_t<T>>>
        struct _check_dimensions {
            static constexpr bool value = _check_dimensions_impl<T>(
                std::make_index_sequence<
                    std::tuple_size_v<remove_reference_t<T>>>());
        };

    } // namespace detail

    //
    // concept Tuple
    //
    template<class T>
    concept bool Tuple =
        Complete<std::tuple_size<remove_reference_t<T>>>
        && detail::_check_dimensions<T>::value;

    namespace detail {

        template<size_t N, class T>
        using Index_declare_get_type =
            decltype(std::declval<T>().template get<N>());

        template<class T, size_t... I>
        constexpr
        auto Index_check_same_types_impl(std::index_sequence<I...>)
            -> bool
        {
            return ((std::is_same_v<Index_declare_get_type<I, T>,
                Index_declare_get_type<0, T>>) && ...);
        }

        template<class T>
        // use require as gcc doesn't seem to short-circuit conjunctions
        // within concepts as described here:
        // http://en.cppreference.com/w/cpp/language/constraints#Conjunctions
        requires Tuple<T>
        constexpr
        auto Index_check_same_types()
            -> bool
        {
            return Index_check_same_types_impl<T>(
                std::make_index_sequence<std::tuple_size_v<remove_cvref_t <T>>>());
        }

    } // namespace detail

    //
    // concept Index
    //
    template<class T>
    concept bool Index =
        Regular<remove_cvref_t<T>> &&
        Tuple<T> &&
        requires(const remove_reference_t<T>& a, size_t idx) {
            { a.template get<0>() } -> Integral;
            { a[idx] } -> Integral;
        } &&
        detail::Index_check_same_types<const remove_cvref_t<T>&>() &&
        requires(remove_cvref_t<T>& a, size_t idx,
            remove_cvref_t<decltype(a[idx])> v) {
            { a.template get<0>() = v };
            { a[idx] = v };
        } &&
        detail::Index_check_same_types<remove_cvref_t<T>&>();

    template<class T>
    struct rank;
    template<class T>
    inline constexpr size_t rank_v = rank<T>::value;

    // If T is an Index type, provides the member constant value equal to the
    // number of dimensions of the index.
    template<Index T>
    struct rank<T> : std::tuple_size<remove_cvref_t<T>> {};

    //
    // concept Compatible
    //
    // Evaluates to true if all specified types T have the same rank.
    //
    template<class... T>
    concept bool Compatible = ((rank_v<T> == rank_v<first_t<T>>) == ...);

    template<class T>
    struct element_type;
    template<class T>
    using element_type_t = _t<element_type<T>>;

    // If T is an Index type, provides the member typedef type equal to the type
    // of the elements returned by T::get<N>()
    template<Index T>
    struct element_type<T> {
        using type = remove_cvref_t<decltype(std::declval<T>().template get<0>())>;
    };


    // Returns the Nth element from the Index. N is an integer value in
    // [0, r) where r is the rank of the index type.
    template<size_t N>
    constexpr
    auto get(Index&& index)
        -> decltype(auto)
    {
        return std::forward<decltype(index)>(index).template get<N>();
    }

    namespace detail {

        template<size_t... I>
        constexpr
        auto size_impl(Index const& extent, std::index_sequence<I...>)
            -> size_t
        {
            return (get<I>(extent) * ...);
        }

    } // namespace detail

    // Returns the number of elements an array with the given extent would
    // contain.
    constexpr
    auto size(Index const& extent)
        -> size_t
    {
        return detail::size_impl(extent,
            std::make_index_sequence<rank_v<decltype(extent)>>());
    }

} // namespace repa
