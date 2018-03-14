//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/meta/concepts.hpp"

namespace repa {

    namespace execution {

        struct sequenced_policy {};
        struct parallel_policy {};

        inline constexpr sequenced_policy seq{};
        inline constexpr parallel_policy par{};

    } // namespace execution

    template<class T>
    concept bool ExecutionPolicy =
        Same<T, execution::sequenced_policy> ||
        Same<T, execution::parallel_policy>;

} // namespace repa
