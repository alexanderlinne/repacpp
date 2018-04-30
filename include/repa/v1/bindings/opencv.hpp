//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "repa/v1/core/array_concept.hpp"

#include "opencv2/core.hpp"

namespace repa {

    namespace detail {

        template<class T, size_t Rank = 2>
        struct cv_mat
        {
            using value_type = T;
            using reference = value_type&;
            using const_reference = const value_type&;

            // ctor

            cv_mat(cv::Mat mat)
                : _extent(make_index(static_cast<size_t>(mat.rows),
                    static_cast<size_t>(mat.cols))),
                mat(std::move(mat))
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
                return mat.at<T>(cv_size(idx));
            }

            auto operator[](Index const& idx)
                -> reference
            {
                return mat.at<T>(cv_size(idx));
            }

        private:
            static auto cv_size(Index const& idx) {
                return cv::Point(idx.template get<1>(), idx.template get<0>());
            }

        private:
            index<size_t, Rank> _extent;
            cv::Mat mat;
        };

        template<size_t CvMatType>
        struct cv_elem_type {
            using type = void;
        };
        template<size_t CvMatType>
        using cv_elem_type_t = _t<cv_elem_type<CvMatType>>;

        template<> struct cv_elem_type<CV_8UC1> { using type = uchar; };
        template<> struct cv_elem_type<CV_8SC1> { using type = char; };
        template<> struct cv_elem_type<CV_16UC1> { using type = ushort; };
        template<> struct cv_elem_type<CV_16SC1> { using type = short; };
        template<> struct cv_elem_type<CV_32SC1> { using type = int; };
        template<> struct cv_elem_type<CV_32FC1> { using type = float; };
        template<> struct cv_elem_type<CV_64FC1> { using type = double; };

    } // namespace detail

    template<class T, size_t Rank>
    struct is_delayed<detail::cv_mat<T, Rank>> : std::false_type {};

    template<int CvMatType>
    auto id(cv::Mat mat) {
        return detail::cv_mat<detail::cv_elem_type_t<CvMatType>>{mat};
    }

} // namespace repa
