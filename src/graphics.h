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
typedef int TextureID;

Graphics* create_graphics(int width, int height);
void destroy_graphics(Graphics* graphics);

void render_graphics(Graphics* graphics);
MeshID cube_mesh(Graphics* graphics);
MeshID quad_mesh(Graphics* graphics);

TextureID load_texture(Graphics* graphics, const char* filename);
MeshID load_mesh(Graphics* graphics, const char* filename);

void set_view_transform(Graphics* graphics, Transform view);
void add_render_command(Graphics* graphics, MeshID mesh, TextureID diffuse, Transform transform);

#endif /* include guard */
