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

#ifndef MICRO_CONTAINERS_SIZE_TYPE
#define MICRO_CONTAINERS_SIZE_TYPE unsigned long
#endif

enum class microc_new { blah };
// This is a placement new override
inline void* operator new (MICRO_CONTAINERS_SIZE_TYPE n, void* ptr, enum microc_new) noexcept {
    return ptr;
}

