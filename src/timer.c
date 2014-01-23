/* Copyright (c) <2013>, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
