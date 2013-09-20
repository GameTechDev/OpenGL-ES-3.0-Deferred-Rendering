/*! @file timer.h
 *  @brief High-performance, high-resolution timer
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __timer_h__
#define __timer_h__

typedef struct Timer Timer;

Timer* create_timer(void);
void destroy_timer(Timer* timer);

void reset_timer(Timer* timer);
/** @return The time since the last `get_delta_time` call, in seconds. */
double get_delta_time(Timer* timer);
/** @return The time since the last `reset_timer` call, in seconds. */
double get_running_time(Timer* timer);

#endif /* include guard */
