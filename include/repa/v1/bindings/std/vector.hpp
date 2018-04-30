//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <vector>

#include "repa/v1/core/array_concept.hpp"

namespace repa {

    namespace detail {

        template<class T, size_t Rank = 2>
        struct std_vector
        {
            using value_type = T;
            using reference = value_type&;
            using const_reference = const value_type&;
            using vector_type = std::vector<T>;

            // ctor

            std_vector(vector_type vec)
                : _extent(make_index(vec.size())),
                vec(std::move(vec))
            {}

            // Matrix concept

            auto extent() const
                -> Index const&
            {
                return _extent;
            }

            auto operator[](Index const& idx) const
                -> const_reference
            {
                return vec[get<0>(idx)];
            }

            auto operator[](Index const& idx)
                -> reference
            {
                return vec[get<0>(idx)];
            }

        private:

        private:
            index<typename vector_type::size_type, 1> _extent;
            vector_type vec;
        };

    } // namespace detail

    template<class T>
    struct is_delayed<detail::std_vector<T>> : std::false_type {};

    template<class T>
    auto id(std::vector<T> vec) {
        return detail::std_vector{std::move(vec)};
    }

} // namespace repa
