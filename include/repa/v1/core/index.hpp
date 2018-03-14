//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <array>
#include <tuple>

#include "repa/v1/core/index_concept.hpp"
#include "repa/v1/meta/concepts.hpp"

namespace repa {

    template<Integral Repr, size_t Rank>
    struct index {
        using value_type = Repr;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using rvalue_reference = value_type&&;

        constexpr
        auto operator[](size_t pos)
            -> reference
        {
            return data[pos];
        }

        constexpr
        auto operator[](size_t pos) const
            -> const_reference
        {
            return data[pos];
        }

        template<size_t N>
        constexpr
        auto get()
            -> reference
        {
            static_assert(N < Rank);
            return std::get<N>(data);
        }

        template<size_t N>
        constexpr
        auto get() const
            -> const_reference
        {
            static_assert(N < Rank);
            return std::get<N>(data);
        }

        std::array<Repr, Rank> data;
    };

    namespace detail {

        template<class Repr, size_t Rank, size_t... I>
        auto Index_print_impl(std::ostream& o,
            repa::index<Repr, Rank> const& index,
            std::index_sequence<I...>)
            -> std::ostream&
        {
            o << "[";
                ((o << get<I>(index) << ", "), ...)
                << get<Rank-1>(index) << "]";
            return o;
        }

    } // namespace detail

    template<class Repr, size_t Rank>
    auto operator<<(std::ostream& o, repa::index<Repr, Rank> const& index)
        -> std::ostream&
    {
        return detail::Index_print_impl(o, index,
            std::make_index_sequence<Rank-1>());
    };

    namespace detail {

        template<size_t... I>
        constexpr
        auto index_equal_impl(auto&& s1, auto&& s2,
            std::integer_sequence<size_t, I...>)
            -> bool
        {
            return ((get<I>(s1) == get<I>(s2)) && ...);
        };

    } // namespace detail

    template<class Repr1, size_t Dim1, class Repr2, size_t Dim2>
    requires
        Dim1 == Dim2 &&
        EqualityComparable2<Repr1, Repr2>
    constexpr
    auto operator==(const index<Repr1, Dim1>& l, const index<Repr2, Dim2>& r)
        -> bool
    {
        return detail::index_equal_impl(l, r,
            std::make_integer_sequence<size_t, Dim1>());
    };

    template<class Repr1, size_t Dim1, class Repr2, size_t Dim2>
    constexpr
    auto operator!=(const index<Repr1, Dim1>& l, const index<Repr2, Dim2>& r)
        -> bool
    {
        return !(l == r);
    }


    // Constructs a repa::index from the given values.
    template<Integral... Values>
    auto make_index(Values... values) {
        using _type = std::common_type_t<Values...>;
        return index<_type, sizeof...(Values)>({_type{values}...});
    }

    namespace detail {

        template<class T, size_t... I>
        auto _make_index_impl(T&& t, std::index_sequence<I...>) {
            return make_index((t.template get<I>())...);
        };

    } // namespace detail

    // Constructs a repa::index from the given index.
    auto make_index(Index&& index) {
        return detail::_make_index_impl(
            std::forward<decltype(index)>(index),
            std::make_index_sequence<rank_v<decltype(index)>>());
    }

} // namespace repa

namespace std {

    template<class Repr, size_t Ex>
    struct tuple_size<repa::index<Repr, Ex>>
        : std::integral_constant<size_t, Ex> {};

    template<size_t N, class Repr, size_t Ex>
    struct tuple_element<N, repa::index<Repr, Ex>> {
        using type = Repr;
    };

} // namespace std
