/*! @file timer.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "timer.h"

#if defined(__APPLE__)

#include <stdint.h>
#include <stdlib.h>

struct Timer
{
    uint64_t    start_time;
    uint64_t    prev_time;
    double      frequency;
};


#include <mach/mach_time.h>
Timer* create_timer(void)
{
    Timer* timer = (Timer*)calloc(1,sizeof(*timer));
    mach_timebase_info_data_t frequency = {0,0};
    mach_timebase_info(&frequency);
    timer->frequency = (double)frequency.numer / (double)frequency.denom;
    timer->frequency *= 1e-9;
    reset_timer(timer);

    return timer;
}
void destroy_timer(Timer* timer)
{
    free(timer);
}
void reset_timer(Timer* timer)
{
    timer->start_time = timer->prev_time = mach_absolute_time();
}
double get_delta_time(Timer* timer)
{
    uint64_t current_time = mach_absolute_time();
    double delta_time = (double)(current_time - timer->prev_time) * timer->frequency;
    timer->prev_time = current_time;
    return delta_time;
}
double get_running_time(Timer* timer)
{
    uint64_t current_time = mach_absolute_time();
    double running_time = (double)(current_time - timer->start_time) * timer->frequency;
    return running_time;
}

#elif defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void init_timer(Timer* timer) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency( &freq );
    timer->frequency = 1.0/(double)freq.QuadPart;
    reset_timer(timer);
}
void reset_timer(Timer* timer) {
    QueryPerformanceCounter((LARGE_INTEGER*)&timer->start_time);
    timer->prev_time = timer->start_time;
}
double get_delta_time(Timer* timer) {
    uint64_t current_time;
    double delta_time;
    QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    delta_time = (current_time - timer->prev_time) * timer->frequency;
    timer->prev_time = current_time;
    return delta_time;
}
double get_running_time(void) {
    uint64_t current_time;
    double  running_time;
    QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );
    running_time = (current_time - timer->start_time) * timer->frequency;
    return running_time;
}

#elif defined(__linux__)

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>

static struct timespec _time_difference(struct timespec a, struct timespec b)
{
    struct timespec temp;
    temp.tv_sec = a.tv_sec-b.tv_sec;
    temp.tv_nsec = a.tv_nsec-b.tv_nsec;
    return temp;
}

struct Timer
{
    struct timespec start_time;
    struct timespec prev_time;
};

Timer* create_timer(void)
{
    Timer* timer = (Timer*)calloc(1, sizeof(*timer));
    reset_timer(timer);
    return timer;
}
void destroy_timer(Timer* timer)
{
    free(timer);
}
void reset_timer(Timer* timer)
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    timer->prev_time = timer->start_time = time;
}
double get_delta_time(Timer* timer)
{
    struct timespec time;
    struct timespec diff;
    clock_gettime(CLOCK_MONOTONIC, &time);
    diff = _time_difference(time, timer->prev_time);
    timer->prev_time = time;
    return diff.tv_sec + diff.tv_nsec*1.0/1000000000;
}
double get_running_time(Timer* timer)
{
    struct timespec time;
    struct timespec diff;
    clock_gettime(CLOCK_MONOTONIC, &time);
    diff = _time_difference(time, timer->start_time);
    return diff.tv_sec + diff.tv_nsec*1.0/1000000000;
}

#else
    #error Need a timer
#endif
