/*! @file timer.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "timer.h"

#if defined(__APPLE__)

#include <mach/mach_time.h>
void init_timer(Timer* timer) {
    mach_timebase_info_data_t frequency = {0,0};
    mach_timebase_info(&frequency);
    timer->frequency = (double)frequency.numer / (double)frequency.denom;
    timer->frequency *= 1e-9;
    reset_timer(timer);
}
void reset_timer(Timer* timer) {
    timer->start_time = timer->prev_time = mach_absolute_time();
}
double delta_time(Timer* timer) {
    uint64_t current_time = mach_absolute_time();
    double delta_time = (double)(current_time - timer->prev_time) * timer->frequency;
    timer->prev_time = current_time;
    return delta_time;
}
double running_time(Timer* timer) {
    uint64_t current_time = mach_absolute_time();
    double running_time = (double)(current_time - timer->start_time) * timer->frequency;
    return running_time;
}

#elif defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void Timer::init(void) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency( &freq );
    frequency = 1.0/(double)freq.QuadPart;
    reset();
}
void Timer::reset(void) {
    QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
    prev_time = start_time;
}
double Timer::delta_time(void) {
    uint64_t current_time;
    double delta_time;
    QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    delta_time = (current_time - prev_time) * frequency;
    prev_time = current_time;
    return delta_time;
}
double Timer::running_time(void) {
    uint64_t current_time;
    double  running_time;
    QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );
    running_time = (current_time - start_time) * frequency;
    return running_time;
}

#elif defined(__linux__)

#include <sys/time.h>
#include <stdio.h>
#include <time.h>
void init_timer(Timer* timer) {
    timer->frequency = 1.0/1000000000.0;
    reset_timer(timer);
}
void reset_timer(Timer* timer) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    timer->prev_time = timer->start_time = time.tv_sec*1000000000 + time.tv_nsec; // Convert the time to nanoseconds
}
double delta_time(Timer* timer) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    uint64_t current_time = time.tv_sec*1000000000 + time.tv_nsec;
    uint64_t delta_time = current_time - timer->prev_time;
    timer->prev_time = current_time;
    return delta_time*timer->frequency;
}
double running_time(Timer* timer) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    uint64_t current_time = time.tv_sec*1000000000 + time.tv_nsec;
    uint64_t delta_time = current_time - timer->start_time;
    return delta_time * timer->frequency;
}

#else
    #error Need a timer
#endif
