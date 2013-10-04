/*! @file deferred.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "deferred.h"
#include <stdlib.h>
#include "gl_include.h"
#include "mesh.h"
#include "scene.h"
#include "graphics.h"
#include "program.h"


/* Defines
 */

/* Types
 */
struct DeferredRenderer
{
    int width;
    int height;

    struct {
        GLuint  program;

        GLuint  u_World;
        GLuint  u_View;
        GLuint  u_Projection;

        GLuint  u_SpecularPower;

        GLuint  s_Normal;
    } geometry;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */

DeferredRenderer* create_deferred_renderer(Graphics* G)
{
    DeferredRenderer* R = (DeferredRenderer*)calloc(1, sizeof(DeferredRenderer));

    return R;
}
void destroy_deferred_renderer(DeferredRenderer* R)
{
}
void resize_deferred_renderer(DeferredRenderer* R, int width, int height)
{
}

void render_deferred(DeferredRenderer* R, GLuint default_framebuffer,
                     Mat4 proj_matrix, Mat4 view_matrix,
                     const Model* models, int num_models,
                     const Light* lights, int num_lights)
{
}
