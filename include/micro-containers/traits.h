/*========================================================================================
 Copyright (2021), Tomer Shalev (tomer.shalev@gmail.com, https://github.com/HendrixString).
 All Rights Reserved.
 License is a custom open source semi-permissive license with the following guidelines:
 1. unless otherwise stated, derivative work and usage of this file is permitted and
    should be credited to the project and the author of this project.
 2. Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
========================================================================================*/
#pragma once

#include "new_micro.h"

template<class Key> struct micro_hash{};
template<> struct micro_hash<unsigned> {
    MICRO_CONTAINERS_SIZE_TYPE operator()(unsigned const s) const noexcept { return s; }
};
template<> struct micro_hash<signed> {
    MICRO_CONTAINERS_SIZE_TYPE operator()(signed const s) const noexcept { return s & ~(1<<((sizeof(s)<<3)-1)) ; }
};

namespace microc {
    using size_t = MICRO_CONTAINERS_SIZE_TYPE;

    namespace traits {

        template< class T > struct remove_reference      {typedef T type;};
        template< class T > struct remove_reference<T&>  {typedef T type;};
        template< class T > struct remove_reference<T&&> {typedef T type;};
        template <class T>
        using remove_reference_t = typename remove_reference<T>::type;

        template< class T > struct remove_const_and_reference      {typedef T type;};
        template< class T > struct remove_const_and_reference<const T>  {typedef T type;};
        template< class T > struct remove_const_and_reference<const T&>  {typedef T type;};
        template< class T > struct remove_const_and_reference<T&&> {typedef T type;};
        template <class T>
        using remove_const_and_reference_t = typename remove_reference<T>::type;

        template< class T > struct remove_const      {typedef T type;};
        template< class T > struct remove_const<const T>  {typedef T type;};
        template< class T > struct remove_const<const T&>  {typedef T & type;};
        template< class T > struct remove_const<T&&> {typedef T&& type;};
        template <class T>
        using remove_const_t = typename remove_reference<T>::type;


        template <class _Tp> inline typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept {
            typedef typename remove_reference<_Tp>::type _Up;
            return static_cast<_Up&&>(__t);
        }
        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type& __t) noexcept {
            return static_cast<_Tp&&>(__t);
        }
        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type&& __t) noexcept {
            return static_cast<_Tp&&>(__t);
        }

        // is same
        template<class T1, class T2> struct is_same { constexpr static bool value = false; };
        template<class T> struct is_same<T, T> { constexpr static bool value = true; };

        // is float point
        template<class number>
        constexpr bool is_float_point() {
            return is_same<float, number>::value ||
                   is_same<double, number>::value ||
                   is_same<long double, number>::value;
        }

        // conditional
        template<bool B, class T, class F>
        struct conditional { typedef T type; };
        template<class T, class F>
        struct conditional<false, T, F> { typedef F type; };

        template<bool, class _Tp = void> struct enable_if {};
        template<class _Tp> struct enable_if<true, _Tp> { typedef _Tp type; };
        template< bool B, class T = void >
        using enable_if_t = typename enable_if<B,T>::type;

        // integral constant
        template <class Tp, Tp _v>
        struct integral_constant
        {
            static constexpr const Tp      value = _v;
            typedef Tp               value_type;
            typedef integral_constant type;
            constexpr operator value_type() const noexcept {return value;}
        };

        typedef integral_constant<bool, true> true_type;
        typedef integral_constant<bool, false> false_type;

        // allocator aware traits with SFINAE
        template <typename T, typename = int>
        struct is_allocator_aware : microc::traits::false_type { };

        template <typename T>
        struct is_allocator_aware <T, typename conditional<false, typename T::allocator_type, int>::type> :
                microc::traits::true_type { };
// another version with decltype
//        template <typename T>
//        struct is_allocator_aware <T, decltype((void) T().get_allocator(), 0)> : microc::traits::true_type { };

        /**
         * standard allocator
         * @tparam T the allocated object type
         */
        template<typename T>
        class std_allocator {
        public:
            using value_type = T;
            using size_t = unsigned long;
        public:
            template<class U>
            explicit std_allocator(const std_allocator<U> & other) noexcept { };
            explicit std_allocator()=default;

            template <class U, class... Args>
            void construct(U* p, Args&&... args) {
                ::new(p, microc_new::blah) U(traits::forward<Args>(args)...);
            }
            T * allocate(size_t n) { return (T *)operator new(n * sizeof(T)); }
            void deallocate(T * p, size_t n=0) { operator delete (p); }

            template<class U> struct rebind {
                typedef std_allocator<U> other;
            };
        };

        template<class T1, class T2>
        bool operator==(const std_allocator<T1>& lhs, const std_allocator<T2>& rhs ) noexcept {
            return true;
        }
    }
}

#include "pair.h"
