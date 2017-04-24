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

#ifndef __game_h__
#define __game_h__

#include <stdint.h>
#include "vec_math.h"

typedef struct Game Game;

typedef struct TouchPoint
{
    intptr_t    index;
    Vec2        pos;
} TouchPoint;

Game* create_game(void);
void destroy_game(Game* G);
void resize_game(Game* G, int width, int height);

void update_game(Game* G);
void render_game(Game* G);

void add_touch_points(Game* G, int num_touch_points, TouchPoint* points);
void update_touch_points(Game* G, int num_touch_points, TouchPoint* points);
void remove_touch_points(Game* G, int num_touch_points, TouchPoint* points);

#endif /* include guard */
