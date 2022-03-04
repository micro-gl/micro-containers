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

#include "traits.h"

namespace microc {

    template<class T1, class T2>
    struct pair {
        using first_type = T1;
        using second_type = T2;
        T1 first;
        T2 second;

        pair()=default;
        ~pair()=default;
        pair(const T1& x, const T2& y) : first(x), second(y) {}
        template< class U1 = T1, class U2 = T2 >
        constexpr pair(U1&& x, U2&& y) :
                first(microc::traits::forward<U1>(x)),
                second(microc::traits::forward<U2>(y)) {

        };
        pair(const pair& p) : first(p.first), second(p.second) {};
        pair(pair&& p)  noexcept : first(microc::traits::move(p.first)), second(microc::traits::move(p.second)) {};
        pair& operator=(const pair& other) {
            first = other.first;
            second = other.second;
            return *this;
        }
        pair& operator=(pair&& o) noexcept {
            first = microc::traits::move(o.first);
            second = microc::traits::move(o.second);
            return *this;
        };
    };
}