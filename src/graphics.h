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

#ifndef __graphics_h__
#define __graphics_h__

#include "scene.h"
#include "graphics_types.h"

#define MAX_LIGHTS 128

typedef enum {
    kForward,
    kLightPrePass,
    kDeferred,
    
    MAX_RENDERERS
} RendererType;

Graphics* create_graphics(void);
void destroy_graphics(Graphics* G);

void resize_graphics(Graphics* G, int width, int height);

void set_view_matrix(Graphics* G, Mat4 view);
void add_render_command(Graphics* G, Model model);
void add_light(Graphics* G, Light light);

void render_graphics(Graphics* G);

RendererType renderer_type(const Graphics* G);
void cycle_renderers(Graphics* G);

void graphics_size(const Graphics* G, int* width, int* height);

void toggle_static_size(Graphics* G);

#endif /* include guard */
