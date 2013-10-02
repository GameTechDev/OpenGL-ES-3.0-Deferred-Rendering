/*! @file graphics.h
 *  @brief Graphics interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include "scene.h"

typedef struct Graphics Graphics;

Graphics* create_graphics(void);
void destroy_graphics(Graphics* G);

void resize_graphics(Graphics* G, int width, int height);

void set_view_matrix(Graphics* G, Mat4 view);
void add_render_command(Graphics* G, Model model);
void render_graphics(Graphics* G);

#endif /* include guard */
