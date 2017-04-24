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

#ifndef __mesh_h__
#define __mesh_h__

#include <stdint.h>
#include <stddef.h>
#include "vertex.h"
#include "graphics_types.h"


Mesh* create_mesh(const Vertex* vertex_data, size_t vertex_data_size,
                  const uint32_t* index_data, size_t index_data_size,
                  int index_count);
void draw_mesh(const Mesh* M);
void destroy_mesh(Mesh* M);

#endif /* include guard */
