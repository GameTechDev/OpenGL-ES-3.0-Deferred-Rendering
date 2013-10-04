/*! @file light_prepass.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "light_prepass.h"
#include <stdlib.h>
#include "gl_include.h"
#include "mesh.h"
#include "scene.h"
#include "graphics.h"
#include "program.h"

/* Defines
 */
#define GetUniformLocation(R, pass, program, uniform) R->pass.uniform = glGetUniformLocation(R->pass.program, #uniform)

/* Types
 */
struct LightPrepassRenderer
{
    int width;
    int height;

    GLuint  cube_vertex_buffer;
    GLuint  cube_index_buffer;

    GLuint  gbuffer_framebuffer;
    GLuint  gbuffer_color_texture;
    GLuint  gbuffer_depth_texture;

    /* Pass 1 */
    struct {
        GLuint  program;

        GLuint  u_World;
        GLuint  u_View;
        GLuint  u_Projection;

        GLuint  u_SpecularPower;

        GLuint  s_Normal;
    } pass1;

    /* Pass 2 */
    struct {
        GLuint  program;

        GLuint  u_World;
        GLuint  u_View;
        GLuint  u_Projection;

        GLuint  u_InvProj;
        GLuint  u_Viewport;

        GLuint  u_LightColor;
        GLuint  u_LightPosition;
        GLuint  u_LightSize;

        GLuint  s_GBuffer;
        GLuint  s_Depth;
    } pass2;

    /* Pass 3 */
};

/* Constants
 */
 /* cube vertices
 *
 *               5---------4
 *              /|        /|
 *             / |       / |
 *            1---------0  |
 *            |  |      |  |
 *            |  6------|--7
 *            | /       | /
 *            |/        |/
 *            2---------3
 *
 *   front: { 0, 2, 1 }, { 0, 3, 2 }
 *   right: { 4, 3, 0 }, { 4, 7, 3 }
 *     top: { 4, 1, 5 }, { 4, 0, 1 }
 *    left: { 1, 6, 5 }, { 1, 2, 6 }
 *  bottom: { 3, 6, 2 }, { 3, 7, 6 }
 *    back: { 5, 7, 4 }, { 5, 6, 7 }
 *
 */
static const Vec3 kCubeVertices[] =
{
    {  1.0f,  1.0f, -1.0f }, /* 0 */
    { -1.0f,  1.0f, -1.0f }, /* 1 */
    { -1.0f, -1.0f, -1.0f }, /* 2 */
    {  1.0f, -1.0f, -1.0f }, /* 3 */
    {  1.0f,  1.0f,  1.0f }, /* 4 */
    { -1.0f,  1.0f,  1.0f }, /* 5 */
    { -1.0f, -1.0f,  1.0f }, /* 6 */
    {  1.0f, -1.0f,  1.0f }, /* 7 */
};

static const uint16_t kCubeIndices[] =
{
    0, 2, 1,   0, 3, 2,  /* front */
    4, 3, 0,   4, 7, 3,  /* right */
    4, 1, 5,   4, 0, 1,  /* top */
    1, 6, 5,   1, 2, 6,  /* left */
    3, 6, 2,   3, 7, 6,  /* bottom */
    5, 7, 4,   5, 6, 7,  /* back */
};

/* Variables
 */

/* Internal functions
 */
static void _draw_point_light(LightPrepassRenderer* R)
{
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, R->cube_vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, R->cube_index_buffer));
    ASSERT_GL(glVertexAttribPointer(kPositionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0));
    ASSERT_GL(glDrawElements(GL_TRIANGLES, sizeof(kCubeIndices)/sizeof(kCubeIndices[0]), GL_UNSIGNED_SHORT, NULL));
}

/* External functions
 */
LightPrepassRenderer* create_light_prepass_renderer(Graphics* G)
{
    AttributeSlot pass1_slots[] = {
        kPositionSlot,
        kNormalSlot,
        kTangentSlot,
        kBitangentSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    AttributeSlot pass2_slots[] = {
        kPositionSlot,
        kEmptySlot
    };

    LightPrepassRenderer* R = (LightPrepassRenderer*)calloc(1,sizeof(*R));

    /* Create vertex buffer */
    ASSERT_GL(glGenBuffers(1, &R->cube_vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, R->cube_vertex_buffer));
    ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    /* Create index buffer */
    ASSERT_GL(glGenBuffers(1, &R->cube_index_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, R->cube_index_buffer));
    ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kCubeIndices), kCubeIndices, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Create framebuffer */
    ASSERT_GL(glGenFramebuffers(1, &R->gbuffer_framebuffer));

    /* Color buffer */
    ASSERT_GL(glGenTextures(1, &R->gbuffer_color_texture));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer_color_texture));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    /* Depth buffer */
    ASSERT_GL(glGenTextures(1, &R->gbuffer_depth_texture));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer_depth_texture));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));

    /** Pass 1
     */
    R->pass1.program = create_program("shaders/light_prepass/Pass1Vertex.glsl", "shaders/light_prepass/Pass1Fragment.glsl", pass1_slots);

    ASSERT_GL(GetUniformLocation(R, pass1, program, u_Projection));
    ASSERT_GL(GetUniformLocation(R, pass1, program, u_View));
    ASSERT_GL(GetUniformLocation(R, pass1, program, u_World));

    ASSERT_GL(GetUniformLocation(R, pass1, program, u_SpecularPower));

    ASSERT_GL(GetUniformLocation(R, pass1, program, s_Normal));

    ASSERT_GL(glUseProgram(R->pass1.program));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));
    ASSERT_GL(glEnableVertexAttribArray(kNormalSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kBitangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTexCoordSlot));

    ASSERT_GL(glUniform1i(R->pass1.s_Normal, 0));
    ASSERT_GL(glUseProgram(0));

    /** Pass 2
     */
    R->pass2.program = create_program("shaders/light_prepass/Pass2Vertex.glsl", "shaders/light_prepass/Pass2Fragment.glsl", pass2_slots);

    ASSERT_GL(GetUniformLocation(R, pass2, program, u_Projection));
    ASSERT_GL(GetUniformLocation(R, pass2, program, u_View));
    ASSERT_GL(GetUniformLocation(R, pass2, program, u_World));

    ASSERT_GL(GetUniformLocation(R, pass2, program, u_InvProj));
    ASSERT_GL(GetUniformLocation(R, pass2, program, u_Viewport));

    ASSERT_GL(GetUniformLocation(R, pass2, program, s_GBuffer));
    ASSERT_GL(GetUniformLocation(R, pass2, program, s_Depth));


    ASSERT_GL(GetUniformLocation(R, pass2, program, u_LightColor));
    ASSERT_GL(GetUniformLocation(R, pass2, program, u_LightPosition));
    ASSERT_GL(GetUniformLocation(R, pass2, program, u_LightSize));

    ASSERT_GL(glUseProgram(R->pass2.program));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));

    ASSERT_GL(glUniform1i(R->pass2.s_GBuffer, 0));
    ASSERT_GL(glUniform1i(R->pass2.s_Depth, 1));
    ASSERT_GL(glUseProgram(0));

    /** Pass 3
     */

    return R;
}
void destroy_light_prepass_renderer(LightPrepassRenderer* R)
{
    destroy_program(R->pass1.program);
    free(R);
}
GLuint temp_depth_buffer;
void resize_light_prepass_renderer(LightPrepassRenderer* R, int width, int height)
{
    GLenum framebuffer_status;
    R->width = width;
    R->height = height;
    
    
    ASSERT_GL(glGenTextures(1, &temp_depth_buffer));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, temp_depth_buffer));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));

    /* Color buffer */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer_color_texture));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    /* Depth buffer */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer_depth_texture));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));

    /* Framebuffer */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, R->gbuffer_color_texture, 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, temp_depth_buffer, 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, R->gbuffer_depth_texture, 0));

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("Framebuffer error: %s\n", _glStatusString(framebuffer_status));
        assert(0);
    }

    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));


}

void render_light_prepass(LightPrepassRenderer* R, GLuint default_framebuffer,
                          Mat4 proj_matrix, Mat4 view_matrix,
                          const Model* models, int num_models,
                          const Light* lights, int num_lights)
{
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    Mat4 inv_proj = mat4_inverse(proj_matrix);
    float viewport[] = { R->width, R->height };
    int ii;
    GLint framebuffer_status;

    /** Pass 1
     */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    ASSERT_GL(glDrawBuffers(2, buffers));
    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("Framebuffer error: %s\n", _glStatusString(framebuffer_status));
        assert(0);
    }
    ASSERT_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    ASSERT_GL(glDepthMask(GL_TRUE));
    ASSERT_GL(glDepthFunc(GL_LESS));
    ASSERT_GL(glCullFace(GL_BACK));


    ASSERT_GL(glUseProgram(R->pass1.program));
    ASSERT_GL(glUniformMatrix4fv(R->pass1.u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->pass1.u_View, 1, GL_FALSE, (float*)&view_matrix));

    for(ii=0;ii<num_models;++ii) {
        Mat4 world_matrix = transform_get_matrix(models[ii].transform);
        /* Material */
        ASSERT_GL(glUniform1f(R->pass1.u_SpecularPower, models[ii].material->specular_power));
        ASSERT_GL(glActiveTexture(GL_TEXTURE0));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, models[ii].material->normal));
        /* Mesh */
        ASSERT_GL(glUniformMatrix4fv(R->pass1.u_World, 1, GL_FALSE, (float*)&world_matrix));
        draw_mesh(models[ii].mesh);
    }


    /** Pass 2
     */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer));
    ASSERT_GL(glDrawBuffers(1, buffers));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, R->gbuffer_depth_texture, 0));
    ASSERT_GL(glViewport(0, 0, R->width, R->height));
    ASSERT_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT));

    ASSERT_GL(glEnable(GL_BLEND));
    ASSERT_GL(glBlendFunc(GL_ONE, GL_ONE));
    ASSERT_GL(glCullFace(GL_FRONT));
    ASSERT_GL(glDepthMask(GL_FALSE));
    ASSERT_GL(glDepthFunc(GL_GEQUAL));

    ASSERT_GL(glUseProgram(R->pass2.program));
    ASSERT_GL(glUniformMatrix4fv(R->pass2.u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->pass2.u_View, 1, GL_FALSE, (float*)&view_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->pass2.u_InvProj, 1, GL_FALSE, (float*)&inv_proj));
    ASSERT_GL(glUniform2fv(R->pass2.u_Viewport, 1, viewport));

    for(ii=0;ii<num_lights;++ii) {
        float size = lights[ii].size;
        Mat4 world = mat4_identity;
        Vec4 position = vec4_zero;

        world = mat4_scalef(size,size,size);
        world.r3 = vec4_from_vec3(lights[ii].position,1.0f);

        position = vec4_from_vec3(lights[ii].position, 1.0f);
        position = mat4_mul_vector(position, view_matrix);

        ASSERT_GL(glUniformMatrix4fv(R->pass2.u_World, 1, GL_FALSE, (float*)&world));
        ASSERT_GL(glUniform3fv(R->pass2.u_LightPosition, 1, (float*)&position));
        ASSERT_GL(glUniform3fv(R->pass2.u_LightColor, 1, (float*)&lights[ii].color));
        ASSERT_GL(glUniform1f(R->pass2.u_LightSize, lights[ii].size));
        ASSERT_GL(glActiveTexture(GL_TEXTURE0));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer_color_texture));
        ASSERT_GL(glActiveTexture(GL_TEXTURE1));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, temp_depth_buffer));
        _draw_point_light(R);
    }

    ASSERT_GL(glDisable(GL_BLEND));
    ASSERT_GL(glDepthMask(GL_TRUE));
    ASSERT_GL(glDepthFunc(GL_LESS));
    ASSERT_GL(glCullFace(GL_BACK));

    /** Pass 3
     */

}
