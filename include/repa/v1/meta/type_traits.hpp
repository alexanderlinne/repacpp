//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <type_traits>

namespace repa {

    template<class T>
    using _t = typename T::type;

    template<typename T>
    using remove_reference_t = std::remove_reference_t<T>;

    template<typename T>
    using remove_cv_t = std::remove_cv_t<T>;

    template<class T>
    struct remove_cvref {
        using type = remove_cv_t<remove_reference_t<T>>;
    };
    template<class T>
    using remove_cvref_t = _t<remove_cvref<T>>;

    template<size_t N, class... T>
    struct nth_type;
    template<size_t N, class T, class... Tn>
    struct nth_type<N, T, Tn...> : nth_type<N-1, Tn...> {};
    template<class T, class... Tn>
    struct nth_type<0, T, Tn...> {
        using type = T;
    };
    template<size_t N, class... T>
    using nth_type_t = _t<nth_type<N, T...>>;
    template<class... T>
    using first_t = nth_type_t<0, T...>;

    namespace detail {

        template<class T>
        struct is_complete_impl {
            template<class U>
            static
            auto test(U*)
                -> std::integral_constant<bool, sizeof(U) == sizeof(U)>;

            static
            auto test(...)
                -> std::false_type;

            using type = decltype(test(std::declval<T*>()));
        };

    } // namespace detail

    template<class T>
    struct is_complete : detail::is_complete_impl<T>::type {};
    template<class T>
    inline constexpr bool is_complete_v = is_complete<T>::value;

} // namespace repa
