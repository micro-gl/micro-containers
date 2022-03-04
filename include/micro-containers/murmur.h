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

namespace microc {

    template<class machine_word>
    struct iterative_murmur {
    private:
        using mw = machine_word;
        static constexpr char _bits_ = sizeof(mw)<<3;
        static constexpr int r = _bits_==32 ? 24 : 47;
        static constexpr mw m = _bits_==32 ? 0x5bd1e995 : 0xc6a4a7935bd1e995;
        machine_word _state;
        machine_word _len;
#define mmix(h,k) { (k) *= m; (k) ^= (k) >> r; (k) *= m; (h) *= m; (h) ^= (k); }

    public:
        explicit iterative_murmur(machine_word seed) : _state(seed), _len(0) {
            constexpr bool _32_or_64 = sizeof(mw)==4 or sizeof(mw)==8;
            static_assert(_32_or_64, "machine-word must be 32 or 64 bit");
        }

        void next(machine_word payload) {
            ++_len;
            _next<_bits_>(payload);
        }

        machine_word end() {
            return _end<_bits_>();
        }

    private:

        template<char bits>
        inline void _next(machine_word payload) {}
        template<char bits>
        inline machine_word _end() {}

        template<> inline void _next<32>(machine_word payload) {
            mmix(_state, payload)
        }

        template<> inline void _next<64>(machine_word payload) {
            mmix(_state, payload)
        }

        template<> inline machine_word _end<32>() {
            mw len = _len;
            mmix(_state, len)
            _state ^= _state >> 13;
            _state *= m;
            _state ^= _state >> 15;
            return _state;
        }

        template<> inline machine_word _end<64>() {
            mw len = _len;
            mmix(_state, len)
            _state ^= _state >> r;
            _state *= m;
            _state ^= _state >> r;
            return _state;
        }

#undef mmix
    };
}