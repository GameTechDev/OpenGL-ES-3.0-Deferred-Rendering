/*! @file assert.h
 *  @copyright Copyright (c) 2013 kcweicht. All rights reserved.
 */
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
