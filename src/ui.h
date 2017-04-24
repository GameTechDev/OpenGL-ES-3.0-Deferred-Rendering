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
#ifndef __ui_h__
#define __ui_h__

#include "graphics_types.h"

typedef struct UI UI;

UI* create_ui(Graphics* G);
void destroy_ui(UI* U);

void resize_ui(UI* U, int width, int height);

void add_string(UI* U, float x, float y, float scale, const char* string);
void draw_ui(UI* U);

#endif /* include guard */
