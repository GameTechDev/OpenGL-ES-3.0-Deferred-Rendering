/*! @file forward.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "forward.h"
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
struct ForwardRenderer
{
    GLuint  program;

    GLuint  u_World;
    GLuint  u_View;
    GLuint  u_Projection;

    GLuint  s_Albedo;
    GLuint  s_Normal;
};

/* Constants
 */
static const char* kAttributeSlotNames[] =
{
    "a_Position",   /* kPositionSlot */
    "a_Normal",     /* kNormalSlot */
    "a_TexCoord",   /* kTexCoordSlot */
    "a_Tangent",    /* kTangentSlot */
    "a_Bitangent",  /* kBitangentSlot */
};

/* Variables
 */

/* Internal functions
 */

/* External functions
 */

ForwardRenderer* create_forward_renderer(Graphics* G)
{
    ForwardRenderer* R = (ForwardRenderer*)calloc(1,sizeof(*R));
    R->program = create_program("shaders/forward/vertex.glsl", "shaders/forward/fragment.glsl");

    ASSERT_GL(R->u_Projection = glGetUniformLocation(R->program, "u_Projection"));
    ASSERT_GL(R->u_View = glGetUniformLocation(R->program, "u_View"));
    ASSERT_GL(R->u_World = glGetUniformLocation(R->program, "u_World"));

    ASSERT_GL(R->s_Normal = glGetUniformLocation(R->program, "s_Normal"));
    ASSERT_GL(R->s_Albedo = glGetUniformLocation(R->program, "s_Albedo"));

    ASSERT_GL(glUseProgram(R->program));
    ASSERT_GL(glBindAttribLocation(R->program, kPositionSlot,    kAttributeSlotNames[kPositionSlot]));
    ASSERT_GL(glBindAttribLocation(R->program, kNormalSlot,      kAttributeSlotNames[kNormalSlot]));
    ASSERT_GL(glBindAttribLocation(R->program, kTangentSlot,     kAttributeSlotNames[kTangentSlot]));
    ASSERT_GL(glBindAttribLocation(R->program, kBitangentSlot,   kAttributeSlotNames[kBitangentSlot]));
    ASSERT_GL(glBindAttribLocation(R->program, kTexCoordSlot,    kAttributeSlotNames[kTexCoordSlot]));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));
    ASSERT_GL(glEnableVertexAttribArray(kNormalSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kBitangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTexCoordSlot));

    ASSERT_GL(glUniform1i(R->s_Albedo, 0));
    ASSERT_GL(glUniform1i(R->s_Normal, 1));
    ASSERT_GL(glUseProgram(0));

    return R;
}
void destroy_forward_renderer(ForwardRenderer* R)
{
    destroy_program(R->program);
    free(R);
}
void resize_forward_renderer(ForwardRenderer* R)
{
}

void render_forward(ForwardRenderer* R, Mat4 proj_matrix, Mat4 view_matrix,
                    Model* models, int num_models)
{
    int ii;
    ASSERT_GL(glUseProgram(R->program));
    ASSERT_GL(glUniformMatrix4fv(R->u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->u_View, 1, GL_FALSE, (float*)&view_matrix));
    for(ii=0;ii<num_models;++ii) {
        Mat4 world_matrix = transform_get_matrix(models[ii].transform);
        ASSERT_GL(glUniformMatrix4fv(R->u_World, 1, GL_FALSE, (float*)&world_matrix));
        ASSERT_GL(glActiveTexture(GL_TEXTURE0));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, models[ii].material->albedo));
        ASSERT_GL(glActiveTexture(GL_TEXTURE1));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, models[ii].material->normal));
        draw_mesh(models[ii].mesh);
    }
}
