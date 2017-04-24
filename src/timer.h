/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

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
