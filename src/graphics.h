/*! @file graphics.h
 *  @brief Graphics interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __graphics_h__
#define __graphics_h__

#include "scene.h"
#include "graphics_types.h"

#define MAX_LIGHTS 64

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

#endif /* include guard */
