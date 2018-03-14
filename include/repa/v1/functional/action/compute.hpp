//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/algorithms/foreach.hpp"
#include "repa/v1/core/array.hpp"
#include "repa/v1/functional/detail/pipeable.hpp"

namespace repa::action {

    namespace detail {

        void compute_impl(ExecutionPolicy policy,
            ArrayView&& array, ManifestArray& result)
        {
            for_each(policy, array.extent(), [&](Index const& idx) {
                result[idx] = array[idx];
            });
        }

    } // namespace detail

    auto compute(ExecutionPolicy policy)
        -> Pipeable
    {
        return make_pipeable(
            [=](ArrayView&& array)
                -> ManifestArray
            {
                auto result = make_array<element_type_t<
                    decltype(array)>>(array.extent());
                detail::compute_impl(policy,
                    std::forward<decltype(array)>(array), result);
                return result;
            }
        );
    }

    auto compute()
        -> Pipeable
    {
        return compute(execution::seq);
    }

} // namespace repa::action
