/*! @file graphics.h
 *  @brief OpenGL interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include <stdint.h>

typedef struct Graphics Graphics;

unsigned int load_shader(const char* filename, unsigned int type);

Graphics* create_graphics(int width, int height);
void render(Graphics* graphics);
void destroy_graphics(Graphics* graphics);

#endif /* include guard */
