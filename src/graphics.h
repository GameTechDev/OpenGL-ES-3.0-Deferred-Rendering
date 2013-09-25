/*! @file graphics.h
 *  @brief OpenGL interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include <stdint.h>

typedef struct Graphics Graphics;
typedef int MeshID;

Graphics* create_graphics(int width, int height);
void destroy_graphics(Graphics* graphics);

void render_graphics(Graphics* graphics);
MeshID cube_mesh(Graphics* graphics);

#endif /* include guard */
