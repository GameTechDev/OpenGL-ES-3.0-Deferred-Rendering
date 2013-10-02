/*! @file light_prepass.h
 *  @brief TODO: Enter description of the file
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __light_prepass_h__
#define __light_prepass_h__


#include "graphics.h"
#include "scene.h"
#include "mesh.h"

typedef struct LightPrepassRenderer LightPrepassRenderer;

LightPrepassRenderer* create_light_prepass_renderer(Graphics* G);
void destroy_light_prepass_renderer(LightPrepassRenderer* R);
void resize_light_prepass_renderer(LightPrepassRenderer* R, int width, int height);

void render_light_prepass(LightPrepassRenderer* R, Mat4 proj_matrix, Mat4 view_matrix,
                    const Model* models, int num_models,
                    const Light* lights, int num_lights);

#endif /* include guard */
