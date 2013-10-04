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
#define GetUniformLocation(R, pass, program, uniform) R->pass.uniform = glGetUniformLocation(R->pass.program, #uniform)

/* Types
 */
struct DeferredRenderer
{
    int width;
    int height;

    GLuint  gbuffer_framebuffer;
    GLuint  gbuffer[4];
    GLuint  depth_buffer;

    struct {
        GLuint  program;

        GLuint  u_World;
        GLuint  u_View;
        GLuint  u_Projection;

        GLuint  u_SpecularColor;
        GLuint  u_SpecularPower;
        GLuint  u_SpecularCoefficient;

        GLuint  s_Albedo;
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
    AttributeSlot geometry_slots[] = {
        kPositionSlot,
        kNormalSlot,
        kTangentSlot,
        kBitangentSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    DeferredRenderer* R = (DeferredRenderer*)calloc(1, sizeof(DeferredRenderer));
    int ii;

    /** Create Gbuffer
     */

    /* Create framebuffer */
    ASSERT_GL(glGenFramebuffers(1, &R->gbuffer_framebuffer));

    ASSERT_GL(glGenTextures(4, R->gbuffer));
    for(ii=0;ii<4;++ii) {
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

    ASSERT_GL(GetUniformLocation(R, geometry, program, u_SpecularPower));
    ASSERT_GL(GetUniformLocation(R, geometry, program, u_SpecularCoefficient));
    ASSERT_GL(GetUniformLocation(R, geometry, program, u_SpecularColor));

    ASSERT_GL(GetUniformLocation(R, geometry, program, s_Normal));
    ASSERT_GL(GetUniformLocation(R, geometry, program, s_Albedo));

    ASSERT_GL(glUseProgram(R->geometry.program));

    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));
    ASSERT_GL(glEnableVertexAttribArray(kNormalSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kBitangentSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTexCoordSlot));

    ASSERT_GL(glUniform1i(R->geometry.s_Normal, 1));
    ASSERT_GL(glUniform1i(R->geometry.s_Normal, 1));
    ASSERT_GL(glUseProgram(0));
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
     *  [1] RGB: WS Normal  A: Spec coefficient
     *  [2] RGB: Spec Color A: Spec exponent
     *  [3] R: Depth
     */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[0]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[1]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0));
    
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[2]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->gbuffer[3]));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0));

    /* Depth texture */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, R->depth_buffer));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));

    /* Framebuffer */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, R->gbuffer[0], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, R->gbuffer[1], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, R->gbuffer[2], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, R->gbuffer[3], 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, R->depth_buffer, 0));

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("Framebuffer error: %s\n", _glStatusString(framebuffer_status));
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
        GL_COLOR_ATTACHMENT3,
    };
    //Mat4 inv_proj = mat4_inverse(proj_matrix);
    //float viewport[] = { R->width, R->height };
    int ii;
    GLint framebuffer_status;

    /** Geometry
     */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, R->gbuffer_framebuffer));
    ASSERT_GL(glDrawBuffers(4, buffers));
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

    ASSERT_GL(glUseProgram(R->geometry.program));
    ASSERT_GL(glUniformMatrix4fv(R->geometry.u_Projection, 1, GL_FALSE, (float*)&proj_matrix));
    ASSERT_GL(glUniformMatrix4fv(R->geometry.u_View, 1, GL_FALSE, (float*)&view_matrix));

    for(ii=0;ii<num_models;++ii) {
        Mat4 world_matrix = transform_get_matrix(models[ii].transform);
        /* Material */
        ASSERT_GL(glUniform3fv(R->geometry.u_SpecularColor, 1, (float*)&models[ii].material->specular_color));
        ASSERT_GL(glUniform1f(R->geometry.u_SpecularPower, models[ii].material->specular_power));
        ASSERT_GL(glUniform1f(R->geometry.u_SpecularCoefficient, models[ii].material->specular_coefficient));
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
}
