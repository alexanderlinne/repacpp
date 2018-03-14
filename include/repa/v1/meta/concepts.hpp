//
// Copyright Alexander Linne 2017 - present.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt
//     or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <functional>
#include <tuple>
#include <type_traits>

#include "repa/v1/meta/type_traits.hpp"

namespace repa {

    //
    // concept MoveConstructible
    //
    template<class T>
    concept bool MoveConstructible = std::is_move_constructible_v<T>;

    //
    // concept MoveAssignable
    //
    template<class T>
    concept bool MoveAssignable = std::is_move_assignable_v<T>;

    //
    // concept Movable
    //
    template<class T>
    concept bool Movable =
        std::is_object_v<T>
        && MoveConstructible<T>
        && MoveAssignable<T>;

    //
    // concept CopyConstructible
    //
    template<class T>
    concept bool CopyConstructible =
        MoveConstructible<T> &&
        std::is_copy_constructible_v<T>;

    //
    // concept CopyAssignable
    //
    template<class T>
    concept bool CopyAssignable =
        MoveAssignable<T> &&
        std::is_copy_assignable_v<T>;

    //
    // concept Copyable
    //
    template<class T>
    concept bool Copyable =
        Movable<T>
        && CopyConstructible<T>
        && CopyAssignable<T>;

    //
    // concept BaseOf
    //
    template<class Base, class Derived>
    concept bool BaseOf = std::is_base_of_v<Base, Derived>;

    //
    // concept Same
    //
    template<class T, class U>
    concept bool Same = std::is_same_v<T, U>;

    //
    // concept Boolean
    //
    template<class T>
    concept bool Boolean =
        requires(T b1, T b2) {
            { bool(b1) };
            { !b1 } -> bool;
            requires Same<decltype(b1 && b2), bool>;
            requires Same<decltype(b1 || b2), bool>;
        };

    //
    // concept Integral
    //
    template<class T>
    concept bool Integral = std::is_integral_v<T>;

    //
    // concept Convertible
    //
    template<class From, class To>
    concept bool Convertible = std::is_convertible_v<From, To>;

    //
    // concept Addable
    //
    template<class T>
    concept bool Addable =
        requires (T a, T b) {
            a + b;
        };
    template<class T, class U>
    concept bool Addable2 =
        requires (T a, U b) {
            a + b;
            b + a;
        };

    //
    // concept EqualityComparable
    //
    template<class T>
    concept bool EqualityComparable =
        requires(T a, T b) {
            { a == b } -> bool;
        };
    template<class T, class U>
    concept bool EqualityComparable2 =
        requires(T a, U b) {
            { a == b } -> bool;
            { b == a } -> bool;
        };

    //
    // concept LessThanComparable
    //
    template<class T>
    concept bool LessThanComparable =
        requires(T a, T b) {
            { a < b } -> bool;
        };
    template<class T, class U>
    concept bool LessThanComparable2 =
        requires(T a, U b) {
            { a < b } -> bool;
        };

    //
    // concept Incrementable
    //
    template<class T>
    concept bool Incrementable =
        requires(T a) {
            a++;
        };

    //
    // concept Printable
    //
    template<class T>
    concept bool Printable =
        requires(std::ostream& o, const T a) {
            o << a;
        };

    //
    // concept Complete
    //
    template<class T>
    concept bool Complete = is_complete_v<T>;

    //
    // concept Object
    //
    template<class T>
    concept bool Object = std::is_object_v<T>;

    //
    // concept Reference
    //
    template<class T>
    concept bool Reference = std::is_reference_v<T>;

    //
    // concept Invocable
    //
    template<class F, class... Args>
    concept bool Invocable =
        requires(F&& f, Args&&... args) {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        };

    //
    // concept UnaryFunctor
    //
    template<class F, class Arg>
    concept bool UnaryFunction =
        Invocable<F, Arg> &&
        Object<decltype(std::declval<F>()(std::declval<Arg>()))>;

} // namespace repa
