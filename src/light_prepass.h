#ifndef __light_prepass_h__
#define __light_prepass_h__

#include "gl_include.h"
#include "graphics.h"
#include "scene.h"
#include "mesh.h"

typedef struct LightPrepassRenderer LightPrepassRenderer;

LightPrepassRenderer* create_light_prepass_renderer(Graphics* G, int major_version, int minor_version);
void destroy_light_prepass_renderer(LightPrepassRenderer* R);
void resize_light_prepass_renderer(LightPrepassRenderer* R, int width, int height);

void render_light_prepass(LightPrepassRenderer* R, GLuint default_framebuffer,
                          Mat4 proj_matrix, Mat4 view_matrix,
                          const Model* models, int num_models,
                          const Light* lights, int num_lights);

#endif /* include guard */
