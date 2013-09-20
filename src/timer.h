/*! @file timer.h
 *  @brief High-performance, high-resolution timer
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __timer_h__
#define __timer_h__

#include <stdint.h>

typedef struct Timer
{
    uint64_t    start_time;
    uint64_t    prev_time;
    double      frequency;
} Timer;

void init_timer(Timer* timer);
void reset_timer(Timer* timer);
double delta_time(Timer* timer);
double running_time(Timer* timer);

#endif /* include guard */
