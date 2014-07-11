#ifndef __deferred_h__
#define __deferred_h__

#include "gl_include.h"
#include "graphics.h"
#include "scene.h"
#include "mesh.h"

typedef struct DeferredRenderer DeferredRenderer;

DeferredRenderer* create_deferred_renderer(Graphics* G);
void destroy_deferred_renderer(DeferredRenderer* R);
void resize_deferred_renderer(DeferredRenderer* R, int width, int height);

void render_deferred(DeferredRenderer* R, GLuint default_framebuffer,
                     Mat4 proj_matrix, Mat4 view_matrix,
                     const Model* models, int num_models,
                     const Light* lights, int num_lights);


#endif /* include guard */
