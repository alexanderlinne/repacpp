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

    template<class T, size_t Rank, Index _Index>
    class array {
        using value_type = T;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using rvalue_reference = value_type&&;

    public:
        array(_Index&& extent)
            : _extent(std::forward<_Index>(extent))
        {
            allocate();
        }

        array(array const&) = default;
        array(array&&) = default;

        array& operator=(array const&) = default;
        array& operator=(array&&) = default;

        auto extent() const
            -> Index const&
        {
            return _extent;
        }

        auto operator[](Index const& idx) const
            -> const_reference
        {
            return data[from_index(idx)];
        }

        auto operator[](Index const& idx)
            -> reference
        {
            return data[from_index(idx)];
        }

    private:
        void allocate() {
            data.resize(repa::size(extent()));
        }

        size_t from_index(Index const& idx) const {
            size_t factor = 1,
                result = 0;
            for (size_t i = Rank; i > 0; --i) {
                result += factor * idx[i-1];
                factor *= _extent[i-1];
            }
            return result;
        }

    private:
        remove_cvref_t<_Index> _extent;
        std::vector<T> data;
    };

    template<class T>
    auto make_array(Index&& extent) {
        return array<T, rank_v<decltype(extent)>, decltype(extent)>{
            std::forward<decltype(extent)>(extent)};
    }

    template<class T, size_t Rank, class IdxRepr>
    struct is_delayed<array<T, Rank, IdxRepr>>
        : std::false_type {};

} // namespace repa
