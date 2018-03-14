//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"

namespace repa::detail {

    template<size_t Dim>
    void ArrayView_print_impl(std::ostream& o, Index const& extent,
        ArrayView const& array, Index& index)
    {
        // extension of for_each_seq_impl implementation

        constexpr auto _rank = rank_v<decltype(array)>;
        if constexpr (Dim == _rank) {
            o << array[static_cast<const decltype(index)>(index)];
            if (get<_rank - 1>(index) != get<_rank - 1>(extent) - 1)
                o << ", ";
        } else {
            o << "[";
            decltype(auto) dimex = get<Dim>(index);
            for (dimex = 0; dimex < get<Dim>(extent); ++dimex) {
                ArrayView_print_impl<Dim + 1>(o, extent, array, index);
            }
            o << "]";

            if constexpr (Dim != 0) {
                if (get<Dim - 1>(index) != get<Dim - 1>(extent) - 1) {
                    o << ",\n";
                }
            }
        }
    }

    std::ostream& ArrayView_print(std::ostream& o, ArrayView const& array) {
        auto index = array.extent();
        repa::detail::ArrayView_print_impl<0>(o, array.extent(), array, index);
        return o;
    }

} // namespace repa::detail

auto operator<<(std::ostream& o, repa::ArrayView const& array)
    -> std::ostream&
requires
    repa::Printable<repa::element_type_t<decltype(array)>>
{
    return repa::detail::ArrayView_print(o, array);
}
