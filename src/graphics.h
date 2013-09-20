/*! @file graphics.h
 *  @brief OpenGL interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include <stdint.h>

typedef struct Graphics Graphics;

Graphics* create_graphics(int width, int height);
void render_graphics(Graphics* graphics);
void destroy_graphics(Graphics* graphics);

#endif /* include guard */
