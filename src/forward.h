/*! @file forward.h
 *  @brief forward renderer
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __forward_h__
#define __forward_h__
#include "gl_include.h"
#include "graphics.h"
#include "scene.h"
#include "mesh.h"

typedef struct ForwardRenderer ForwardRenderer;

ForwardRenderer* create_forward_renderer(Graphics* G);
void destroy_forward_renderer(ForwardRenderer* R);
void resize_forward_renderer(ForwardRenderer* R, int width, int height);

void render_forward(ForwardRenderer* R, GLuint default_framebuffer,
                    Mat4 proj_matrix, Mat4 view_matrix,
                    const Model* models, int num_models,
                    const Light* lights, int num_lights);

#endif /* include guard */
