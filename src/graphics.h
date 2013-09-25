/*! @file graphics.h
 *  @brief OpenGL interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include <stdint.h>
#include "vec_math.h"

typedef struct Graphics Graphics;
typedef int MeshID;

Graphics* create_graphics(int width, int height);
void destroy_graphics(Graphics* graphics);

void render_graphics(Graphics* graphics);
MeshID cube_mesh(Graphics* graphics);
MeshID quad_mesh(Graphics* graphics);
void add_render_command(Graphics* graphics, MeshID mesh, Transform transform);

#endif /* include guard */
