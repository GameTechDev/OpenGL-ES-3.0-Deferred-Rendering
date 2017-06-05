////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////
#include "deferred.h"
#include <stdlib.h>
#include "gl_include.h"
#include "mesh.h"
#include "scene.h"
#include "graphics.h"
#include "program.h"

/* Defines
 */
#define GetUniformLocation(R, pass, program, uniform) R->pass.uniform = glGetUniformLocation(R->pass.program, #uniform)
#define GBUFFER_SIZE 2

/* Types
 */
struct DeferredRenderer
{
    int width;
    int height;

    GLuint  cube_vertex_buffer;
    GLuint  cube_index_buffer;

    GLuint  gbuffer_framebuffer;
    GLuint  gbuffer[GBUFFER_SIZE];
    GLuint  depth_buffer;

    struct {
        GLuint  program;

        GLuint  u_World;
        GLuint  u_View;
        GLuint  u_Projection;

        GLuint  s_Albedo;
        GLuint  s_Normal;
    } geometry;

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
    } light;
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
static void _draw_point_light(DeferredRenderer* R)
{
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, R->cube_vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, R->cube_index_buffer));
    ASSERT_GL(glVertexAttribPointer(kPositionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0));
    ASSERT_GL(glDrawElements(GL_TRIANGLES, sizeof(kCubeIndices)/sizeof(kCubeIndices[0]), GL_UNSIGNED_SHORT, NULL));
}

/* External functions
 */
DeferredRenderer* create_deferred_renderer(Graphics* G)
{
    AttributeSlot geometry_slots[] = {
        kPositionSlot,
        kNormalSlot,
        kTangentSlot,
        kBitangentSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    AttributeSlot light_slots[] = {
        kPositionSlot,
        kEmptySlot
    };
    DeferredRenderer* R = (DeferredRenderer*)calloc(1, sizeof(DeferredRenderer));
    int i[] = {0,1,2};
    int ii;

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

    /** Create Gbuffer
     */

    /* Create framebuffer */
    ASSERT_GL(glGenFramebuffers(1, &R->gbuffer_framebuffer));

    ASSERT_GL(glGenTextures(GBUFFER_SIZE, R->gbuffer));
    for(ii=0;ii<GBUFFER_SIZE;++ii) {
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[ii]));
        ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }
    ASSERT_GL(glGenTextures(1, &R->depth_buffer));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->depth_buffer));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));

    /** Geometry pass
     */
    R->geometry.program = create_program("shaders/deferred/geometryvertex.glsl",
                                         "shaders/deferred/geometryfragment.glsl",
                                         geometry_slots);

    ASSERT_GL(GetUniformLocation(R, geometry, program, u_Projection));
    ASSERT_GL(GetUniformLocation(R, geometry, program, u_View));
    ASSERT_GL(GetUniformLocation(R, geometry, program, u_World));

    ASSERT_GL(GetUniformLocation(R, geometry, program, s_Normal));
    ASSERT_GL(GetUniformLocation(R, geometry, program, s_Albedo));

    ASSERT_GL(glUseProgram(R->geometry.program));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));
    ASSERT_GL(glEnableVertexAttribArray(kNormalSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kBitangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTexCoordSlot));

    ASSERT_GL(glUniform1i(R->geometry.s_Albedo, 0));
    ASSERT_GL(glUniform1i(R->geometry.s_Normal, 1));
    ASSERT_GL(glUseProgram(0));

    /** Light pass
     */
    R->light.program = create_program("shaders/deferred/lightvertex.glsl", "shaders/deferred/lightfragment.glsl", light_slots);

    ASSERT_GL(GetUniformLocation(R, light, program, u_Projection));
    ASSERT_GL(GetUniformLocation(R, light, program, u_View));
    ASSERT_GL(GetUniformLocation(R, light, program, u_World));

    ASSERT_GL(GetUniformLocation(R, light, program, u_InvProj));
    ASSERT_GL(GetUniformLocation(R, light, program, u_Viewport));

    ASSERT_GL(GetUniformLocation(R, light, program, s_GBuffer));


    ASSERT_GL(GetUniformLocation(R, light, program, u_LightColor));
    ASSERT_GL(GetUniformLocation(R, light, program, u_LightPosition));
    ASSERT_GL(GetUniformLocation(R, light, program, u_LightSize));

    ASSERT_GL(glUseProgram(R->light.program));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));

    ASSERT_GL(glUniform1iv(R->light.s_GBuffer, GBUFFER_SIZE+1, i));
    ASSERT_GL(glUseProgram(0));

    if(R->geometry.program == 0 ||
       R->light.program == 0) {
        /* Failed to create programs. Return NULL */
        free(R);
        return NULL;
    }
    return R;
}
void destroy_deferred_renderer(DeferredRenderer* R)
{
    destroy_program(R->geometry.program);
    free(R);
}
void resize_deferred_renderer(DeferredRenderer* R, int width, int height)
{
    GLenum framebuffer_status;
    R->width = width;
    R->height = height;

    /** GBuffer format
     *  [0] RGB: Albedo
     *  [1] RG: VS Normal (encoded)
     *  [2] R: Depth
     */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[0]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[1]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0));

    /* Depth texture */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->depth_buffer));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

    /* Framebuffer */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, R->gbuffer[0], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, R->gbuffer[1], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, R->depth_buffer, 0));

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("%s:%d Framebuffer error: %s\n", __FILE__, __LINE__, _glStatusString(framebuffer_status));
        assert(0);
    }

    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));

}

void render_deferred(DeferredRenderer* R, GLuint default_framebuffer,
                     Mat4 proj_matrix, Mat4 view_matrix,
                     const Model* models, int num_models,
                     const Light* lights, int num_lights)
{
    GLenum buffers[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    Mat4 inv_proj = mat4_inverse(proj_matrix);
    float viewport[] = { R->width, R->height };
    int ii;
    GLint framebuffer_status;

    /** Geometry
     */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("%s:%d Framebuffer error: %s\n", __FILE__, __LINE__, _glStatusString(framebuffer_status));
        assert(0);
    }
    ASSERT_GL(glDrawBuffers(GBUFFER_SIZE, buffers));
    ASSERT_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    ASSERT_GL(glDepthMask(GL_TRUE));
    ASSERT_GL(glDepthFunc(GL_LESS));
    ASSERT_GL(glCullFace(GL_BACK));

    ASSERT_GL(glUseProgram(R->geometry.program));
    ASSERT_GL(glUniformMatrix4fv(R->geometry.u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->geometry.u_View, 1, GL_FALSE, (float*)&view_matrix));

    for(ii=0;ii<num_models;++ii) {
        Mat4 world_matrix = transform_get_matrix(models[ii].transform);
        /* Material */
        ASSERT_GL(glActiveTexture(GL_TEXTURE0));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, models[ii].material->albedo));
        ASSERT_GL(glActiveTexture(GL_TEXTURE1));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, models[ii].material->normal));
        /* Mesh */
        ASSERT_GL(glUniformMatrix4fv(R->geometry.u_World, 1, GL_FALSE, (float*)&world_matrix));
        draw_mesh(models[ii].mesh);
    }


    /** Light
     */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer));
    ASSERT_GL(glDrawBuffers(1, buffers));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, R->depth_buffer, 0));
    ASSERT_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT));

    ASSERT_GL(glEnable(GL_BLEND));
    ASSERT_GL(glBlendFunc(GL_ONE, GL_ONE));
    ASSERT_GL(glCullFace(GL_FRONT));
    ASSERT_GL(glDepthMask(GL_FALSE));
    ASSERT_GL(glDepthFunc(GL_GEQUAL));

    ASSERT_GL(glUseProgram(R->light.program));
    ASSERT_GL(glUniformMatrix4fv(R->light.u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->light.u_View, 1, GL_FALSE, (float*)&view_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->light.u_InvProj, 1, GL_FALSE, (float*)&inv_proj));
    ASSERT_GL(glUniform2fv(R->light.u_Viewport, 1, viewport));

    for(ii=0;ii<GBUFFER_SIZE;++ii) {
        ASSERT_GL(glActiveTexture(GL_TEXTURE0+ii));
        ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[ii]));
    }
    ASSERT_GL(glActiveTexture(GL_TEXTURE0+ii));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->depth_buffer));

    for(ii=0;ii<num_lights;++ii) {
        float size = lights[ii].size;
        Mat4 world = mat4_identity;
        Vec4 position = vec4_zero;

        world = mat4_scalef(size,size,size);
        world.r3 = vec4_from_vec3(lights[ii].position,1.0f);

        position = vec4_from_vec3(lights[ii].position, 1.0f);
        position = mat4_mul_vector(position, view_matrix);

        ASSERT_GL(glUniformMatrix4fv(R->light.u_World, 1, GL_FALSE, (float*)&world));
        ASSERT_GL(glUniform3fv(R->light.u_LightPosition, 1, (float*)&position));
        ASSERT_GL(glUniform3fv(R->light.u_LightColor, 1, (float*)&lights[ii].color));
        ASSERT_GL(glUniform1f(R->light.u_LightSize, lights[ii].size));
        _draw_point_light(R);
    }

    ASSERT_GL(glActiveTexture(GL_TEXTURE0));
    ASSERT_GL(glDisable(GL_BLEND));
    ASSERT_GL(glDepthMask(GL_TRUE));
    ASSERT_GL(glDepthFunc(GL_LESS));
    ASSERT_GL(glCullFace(GL_BACK));

}
