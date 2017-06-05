////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

#ifndef __forward_h__
#define __forward_h__
#include "gl_include.h"
#include "graphics.h"
#include "scene.h"
#include "mesh.h"

typedef struct ForwardRenderer ForwardRenderer;

ForwardRenderer* create_forward_renderer(Graphics* G, int major_version, int minor_version);
void destroy_forward_renderer(ForwardRenderer* R);
void resize_forward_renderer(ForwardRenderer* R, int width, int height);

void render_forward(ForwardRenderer* R, GLuint default_framebuffer,
                    Mat4 proj_matrix, Mat4 view_matrix,
                    const Model* models, int num_models,
                    const Light* lights, int num_lights);

#endif /* include guard */
