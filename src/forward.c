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
#define GetUniformLocation(R, program, uniform) R->uniform = glGetUniformLocation(R->program, #uniform)

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

    GLuint  u_LightPositions;
    GLuint  u_LightColors;
    GLuint  u_LightSizes;
    GLuint  u_NumLights;

    GLuint  u_CameraPosition;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */

ForwardRenderer* create_forward_renderer(Graphics* G)
{
    AttributeSlot slots[] = {
        kPositionSlot,
        kNormalSlot,
        kTangentSlot,
        kBitangentSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    ForwardRenderer* R = (ForwardRenderer*)calloc(1,sizeof(*R));
    R->program = create_program("shaders/forward/vertex.glsl", "shaders/forward/fragment.glsl", slots);

    ASSERT_GL(GetUniformLocation(R, program, u_Projection));
    ASSERT_GL(GetUniformLocation(R, program, u_View));
    ASSERT_GL(GetUniformLocation(R, program, u_World));

    ASSERT_GL(GetUniformLocation(R, program, s_Normal));
    ASSERT_GL(GetUniformLocation(R, program, s_Albedo));


    ASSERT_GL(GetUniformLocation(R, program, u_LightPositions));
    ASSERT_GL(GetUniformLocation(R, program, u_LightColors));
    ASSERT_GL(GetUniformLocation(R, program, u_LightSizes));
    ASSERT_GL(GetUniformLocation(R, program, u_NumLights));
    
    ASSERT_GL(GetUniformLocation(R, program, u_CameraPosition));

    ASSERT_GL(glUseProgram(R->program));

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
                    const Model* models, int num_models,
                    const Light* lights, int num_lights)
{
    Mat4    inv_view = mat4_inverse(view_matrix);
    Vec3    camera_position = vec3_from_vec4(inv_view.r3);
    Vec3    light_positions[MAX_LIGHTS];
    Vec3    light_colors[MAX_LIGHTS];
    float   light_sizes[MAX_LIGHTS];
    int     ii;
    
    for(ii=0;ii<num_lights;++ii) {
        light_positions[ii] = lights[ii].position;
        light_colors[ii] = lights[ii].color;
        light_sizes[ii] = lights[ii].size;
    }

    ASSERT_GL(glUseProgram(R->program));
    ASSERT_GL(glUniformMatrix4fv(R->u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->u_View, 1, GL_FALSE, (float*)&view_matrix));
    ASSERT_GL(glUniform3fv(R->u_LightPositions, num_lights, (float*)light_positions));
    ASSERT_GL(glUniform3fv(R->u_LightColors, num_lights, (float*)light_colors));
    ASSERT_GL(glUniform1fv(R->u_LightSizes, num_lights, (float*)light_sizes));
    ASSERT_GL(glUniform1i(R->u_NumLights, num_lights));
    ASSERT_GL(glUniform3fv(R->u_CameraPosition, 1, (float*)&camera_position));
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
