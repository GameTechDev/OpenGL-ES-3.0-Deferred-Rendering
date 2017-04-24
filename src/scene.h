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

#ifndef __scene_h__
#define __scene_h__

#include "texture.h"
#include "vec_math.h"
#include "graphics_types.h"

typedef struct Scene Scene;
typedef struct SceneData SceneData;
typedef struct Material
{
    char    name[64];
    Texture albedo;
    Texture normal;
    Vec3    specular_color;
    float   specular_power;
    float   specular_coefficient;
} Material;
typedef struct Model
{
    char        name[64];
    Transform   transform;
    Mesh*       mesh;
    Material*   material;
} Model;

Scene* create_scene(const char* filename);
void destroy_scene(Scene* S);
void render_scene(Scene* S, Graphics* G);

Model* get_model(Scene* S, int model);

SceneData* _load_scene_data(const char* filename);
void _free_scene_data(SceneData* S);

#endif /* include guard */
