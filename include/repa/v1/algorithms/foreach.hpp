//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <thread>
#include <vector>

#include "repa/v1/algorithms/execution.hpp"
#include "repa/v1/core/index.hpp"
#include "repa/v1/meta/concepts.hpp"

namespace repa {

    namespace detail {

        template<size_t Dim>
        void for_each_seq_impl(Index const& extent, auto&& fn, Index& index)
        {
            if constexpr (Dim == rank_v<decltype(extent)>) {
                fn(static_cast<std::add_const_t<decltype(index)>>(index));
            } else {
                auto& dimex = get<Dim>(index);
                for (dimex = 0; dimex < get<Dim>(extent); ++dimex) {
                    for_each_seq_impl<Dim + 1>(extent, fn, index);
                }
            }
        }

        void for_each_seq(Index const& extent, auto&& fn)
        {
            auto idx = extent;
            for_each_seq_impl<0>(extent, std::forward<decltype(fn)>(fn), idx);
        }

        void exec_indices(auto&& nthreads,auto&& thread_idx,
            Index const& extent, auto&& fn)
        {
            auto _extent = extent;
            get<0>(_extent) = 1;
            auto max = get<0>(extent);
            for (decltype(max) i = thread_idx; i < max; i += nthreads) {
                for_each_seq(_extent, [&](auto&& idx) {
                    auto _idx = idx;
                    get<0>(_idx) = i;
                    fn(_idx);
                });
            }
        }

        template<class Fn>
        void for_each_par(Index const& extent, Fn&& fn)
        {
            auto nthreads = std::thread::hardware_concurrency();
            if (nthreads < 2) {
                for_each(extent, std::forward<Fn>(fn));
                return;
            }

            auto count = static_cast<size_t>(get<0>(extent));
            auto threads = std::vector<std::thread>{};
            threads.resize(nthreads);
            for (size_t i = 0; i < nthreads - 1 && i < count; ++i) {
                threads[i] = std::thread([&](auto thread_idx) {
                    exec_indices(nthreads, thread_idx, extent, fn);
                }, i);
            }

            exec_indices(nthreads, nthreads - 1, extent, fn);

            for (size_t i = 0; i < nthreads - 1 && i < count; ++i) {
                threads[i].join();
            }
        }

    } // namespace detail

    template<class Fn>
    void for_each(Index const& extent, Fn&& fn)
    {
        detail::for_each_seq(extent, std::forward<Fn>(fn));
    }

    template<class Fn>
    void for_each(ExecutionPolicy policy, Index const& extent, Fn&& fn)
    {
        if constexpr (Same<decltype(policy), execution::sequenced_policy>) {
            for_each(extent, std::forward<Fn>(fn));
        } else {
            detail::for_each_par(extent, std::forward<Fn>(fn));
        }
    }

} // namespace repa
