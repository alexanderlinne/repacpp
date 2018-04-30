//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>

#include <repa/core>
#include <repa/functional>
#include <repa/bindings/opencv>
#include <repa/bindings/std/ostream>

namespace rv = repa::view;
namespace ra = repa::action;

auto matrix_product(repa::ArrayView&& a, repa::ArrayView&& b) {
    return b | rv::transpose()
             | rv::extend<0>(a.extent()[0])
             | rv::multiply(a | rv::extend<1>(b.extent()[1]))
             | rv::fold([](auto&& a, auto&& b) { return a + b; }, 0);
}

int main(int argc, char *argv[]) {
    auto a = repa::id<CV_32SC1>(cv::Mat(5, 5, CV_32SC1));
    auto b = repa::id<CV_32SC1>(cv::Mat(5, 5, CV_32SC1));

    repa::for_each(repa::execution::par, a.extent(), [&](auto&& p) {
        auto&& [y, x] = p;
        a[p] = x + y;
        b[p] = x + y;
    });

    auto result = matrix_product(a, b) | ra::compute(repa::execution::par);
    std::cout << result << "\n";
}
