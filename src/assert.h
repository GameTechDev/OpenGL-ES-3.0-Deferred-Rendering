////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////
#ifndef __assert_h__
#define __assert_h__

/*! @brief Breaks into the debugger
 */
#if defined( _MSC_VER )
    #define debugBreak() __debugbreak()
    #pragma warning(disable:4127) /* Conditional expression is constant (the do-while) */
#elif defined( __GNUC__ )
    #define debugBreak() __builtin_trap()
#else
    #error Unsupported compiler
#endif

/*! @brief Assert
 */
#ifdef DEBUG
    #ifndef assert
        #define assert(condition)   \
            do {                    \
                if(!(condition)) {  \
                    debugBreak();   \
                }                   \
            } while(__LINE__ == -1)
    #endif
#else
    #ifndef assert
        #define assert(condition)
    #endif
#endif

#ifndef ASSERT
    #define ASSERT assert
#endif

#endif /* include guard */
