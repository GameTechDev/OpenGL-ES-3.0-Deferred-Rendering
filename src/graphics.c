/* Copyright (c) <2013>, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "graphics.h"
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "gl_include.h"
#include "program.h"
#include "vertex.h"

#include "forward.h"
#include "light_prepass.h"
#include "deferred.h"

/* Defines
 */
#define MAX_RENDER_COMMANDS 1024
#define STATIC_WIDTH 1280
#define STATIC_HEIGHT 720

/* Types
 */
struct Graphics
{
    int width;
    int height;
    int real_width;
    int real_height;
    int major_version;
    int minor_version;
    int static_size;

    ForwardRenderer*        forward;
    LightPrepassRenderer*   light_prepass;
    DeferredRenderer*       deferred;

    GLint   default_framebuffer;

    GLuint  fullscreen_program;
    GLuint  fullscreen_quad_vertex_buffer;
    GLuint  fullscreen_quad_index_buffer;
    GLuint  fullscreen_texture;

    GLuint  framebuffer;
    GLuint  color_texture;
    GLuint  depth_texture;

    Mat4    proj_matrix;
    Mat4    view_matrix;

    Model   render_commands[MAX_RENDER_COMMANDS];
    Light   lights[MAX_LIGHTS];
    int     num_render_commands;
    int     num_lights;

    RendererType active_renderer;
};

/* Constants
 */
static const struct {
    float pos[3];
    float tex[2];
} kFullscreenVertices[] = {
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } },
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
};
static const uint32_t kFullscreenIndices[] = {
    0, 2, 1,
    0, 3, 2,
};

/* Variables
 */

/* Internal functions
 */
static void _create_fullscreen_quad(Graphics* G)
{
    AttributeSlot slots[] = {
        kPositionSlot,
        kTexCoordSlot,
        kEmptySlot
    };
    G->fullscreen_program = create_program("fullscreen_vertex.glsl", "fullscreen_fragment.glsl", slots);
    ASSERT_GL(glUseProgram(G->fullscreen_program));
    ASSERT_GL(G->fullscreen_texture = glGetUniformLocation(G->fullscreen_program, "s_Texture"));
    ASSERT_GL(glEnableVertexAttribArray(kPositionSlot));
    ASSERT_GL(glEnableVertexAttribArray(kTexCoordSlot));
    ASSERT_GL(glUseProgram(0));

    /* Create vertex buffer */
    ASSERT_GL(glGenBuffers(1, &G->fullscreen_quad_vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, G->fullscreen_quad_vertex_buffer));
    ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(kFullscreenVertices), kFullscreenVertices, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    /* Create index buffer */
    ASSERT_GL(glGenBuffers(1, &G->fullscreen_quad_index_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G->fullscreen_quad_index_buffer));
    ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kFullscreenIndices), kFullscreenIndices, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
static void _draw_fullscreen_quad(Graphics* G)
{
    float* ptr = 0;
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, G->fullscreen_quad_vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G->fullscreen_quad_index_buffer));
    ASSERT_GL(glVertexAttribPointer(kPositionSlot,    3, GL_FLOAT, GL_FALSE, sizeof(kFullscreenVertices[0]), (void*)(ptr+=0)));
    ASSERT_GL(glVertexAttribPointer(kTexCoordSlot,    2, GL_FLOAT, GL_FALSE, sizeof(kFullscreenVertices[0]), (void*)(ptr+=3)));
    ASSERT_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));
}
static void _create_framebuffer(Graphics* G)
{
    GLenum framebuffer_status;
    /* Color buffer */
    ASSERT_GL(glGenTextures(1, &G->color_texture));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, G->color_texture));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    /* Depth buffer */
    ASSERT_GL(glGenTextures(1, &G->depth_texture));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, G->depth_texture));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    if(G->major_version >= 3)
        ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 4, 4, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));
    else
        ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4, 4, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));

    /* Framebuffer */
    ASSERT_GL(glGenFramebuffers(1, &G->framebuffer));
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, G->framebuffer));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, G->color_texture, 0));
    ASSERT_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, G->depth_texture, 0));

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        system_log("%s:%d Framebuffer error: %s\n", __FILE__, __LINE__, _glStatusString(framebuffer_status));
        assert(0);
    }

    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));
}
static void _resize_framebuffer(Graphics* G)
{
    /* Color buffer */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, G->color_texture));
    ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, G->width, G->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

    /* Depth buffer */
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, G->depth_texture));
    if(G->major_version >= 3)
        ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, G->width, G->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));
    else
        ASSERT_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, G->width, G->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));

    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

/* External functions
 */
Graphics* create_graphics(void)
{
    Graphics* G = NULL;

    /* Allocate graphics */
    G = (Graphics*)calloc(1, sizeof(Graphics));
    G->width = 2;
    G->height = 2;

    /* Set up OpenGL */
    ASSERT_GL(glClearColor(1.0f, 0.0f, 1.0f, 1.0f));
    ASSERT_GL(glClearDepthf(1.0f));
    ASSERT_GL(glEnable(GL_DEPTH_TEST));
    ASSERT_GL(glEnable(GL_CULL_FACE));
    ASSERT_GL(glFrontFace(GL_CW));
    ASSERT_GL(glGetIntegerv(GL_MAJOR_VERSION, &G->major_version));
    ASSERT_GL(glGetIntegerv(GL_MINOR_VERSION, &G->minor_version));
    system_log("OpenGL version:\t%d.%d", G->major_version, G->minor_version);
    system_log("OpenGL version string:\t%s\n", glGetString(GL_VERSION));
    system_log("OpenGL renderer:\t%s\n", glGetString(GL_RENDERER));
    system_log("OpenGL extensions:\n");
    { /* Print extensions */
        char buffer[1024*32] = {0};
        uint32_t ii;
        strlcpy(buffer,(const char*)glGetString(GL_EXTENSIONS), sizeof(buffer));
        for(ii=0;ii<strlen(buffer);++ii) {
            if(buffer[ii] == ' ')
                buffer[ii] = '\n';
        }
        system_log("%s\n", buffer);
    }

    /* Set up self */
    _create_fullscreen_quad(G);
    _create_framebuffer(G);

    /* Set up renderers */
    G->forward = create_forward_renderer(G, G->major_version, G->minor_version);
    G->light_prepass = create_light_prepass_renderer(G, G->major_version, G->minor_version);
    if(G->major_version >= 3)
        G->deferred = create_deferred_renderer(G);

    if(G->deferred)
        G->active_renderer = kDeferred;
    else
        G->active_renderer = kLightPrePass;
    G->static_size = 1;

    return G;
}
void destroy_graphics(Graphics* G)
{
    destroy_deferred_renderer(G->deferred);
    destroy_light_prepass_renderer(G->light_prepass);
    destroy_forward_renderer(G->forward);
    destroy_program(G->fullscreen_program);
    free(G);
}
void resize_graphics(Graphics* G, int width, int height)
{
    if(G->static_size) {
        G->width = STATIC_WIDTH;// width;
        G->height = STATIC_HEIGHT; //height;
    } else {
        G->width = width;
        G->height = height;
    }
    G->real_width = width;
    G->real_height = height;

    G->proj_matrix = mat4_perspective_fov(kPiDiv2, width/(float)height, 1.0f, 100.0f);

    ASSERT_GL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &G->default_framebuffer));

    _resize_framebuffer(G);
    if(G->forward)
        resize_forward_renderer(G->forward, G->width, G->height);
    if(G->light_prepass)
        resize_light_prepass_renderer(G->light_prepass, G->width, G->height);
    if(G->deferred)
        resize_deferred_renderer(G->deferred, G->width, G->height);

    system_log("Graphics resized: %d, %d\n", width, height);
}
void render_graphics(Graphics* G)
{
    GLint device_framebuffer;
    ASSERT_GL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &device_framebuffer));

    ASSERT_GL(glViewport(0, 0, G->width, G->height));
    /* Render scene */
    if(G->major_version >= 3 && G->deferred && G->active_renderer == kDeferred) {
        render_deferred(G->deferred, G->framebuffer,
                        G->proj_matrix, G->view_matrix,
                        G->render_commands, G->num_render_commands,
                        G->lights, G->num_lights);
    } else if(G->active_renderer == kForward) {
        render_forward(G->forward, G->framebuffer,
                       G->proj_matrix, G->view_matrix,
                       G->render_commands, G->num_render_commands,
                       G->lights, G->num_lights);
    } else if(G->active_renderer == kLightPrePass) {
        render_light_prepass(G->light_prepass, G->framebuffer,
                             G->proj_matrix, G->view_matrix,
                             G->render_commands, G->num_render_commands,
                             G->lights, G->num_lights);
    } else {
        assert(!"No Active Renderer");
    }
    G->num_render_commands = 0;
    G->num_lights = 0;

    /* Bind default framebuffer and render to the screen */
    ASSERT_GL(glBindFramebuffer(GL_FRAMEBUFFER, device_framebuffer));
    ASSERT_GL(glViewport(0, 0, G->real_width, G->real_height));
    ASSERT_GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    ASSERT_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    ASSERT_GL(glUseProgram(G->fullscreen_program));
    ASSERT_GL(glActiveTexture(GL_TEXTURE0));
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, G->color_texture));
    _draw_fullscreen_quad(G);
    ASSERT_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void set_view_matrix(Graphics* G, Mat4 view)
{
    G->view_matrix = view;
}
void add_render_command(Graphics* G, Model model)
{
    int index = G->num_render_commands++;
    assert(index <= MAX_RENDER_COMMANDS);
    G->render_commands[index] = model;
}
void add_light(Graphics* G, Light light)
{
    int index = G->num_lights++;
    assert(index <= MAX_LIGHTS);
    G->lights[index] = light;
}
RendererType renderer_type(const Graphics* G)
{
    return G->active_renderer;
}
void cycle_renderers(Graphics* G)
{
    G->active_renderer++;
    if(G->active_renderer == kDeferred && (G->major_version < 3 || G->deferred == NULL))
        G->active_renderer++;

    if(G->active_renderer == MAX_RENDERERS)
        G->active_renderer = 0;
}
void graphics_size(const Graphics* G, int* width, int* height)
{
    *width = G->width;
    *height = G->height;
}
void toggle_static_size(Graphics* G)
{
    G->static_size = !G->static_size;
    resize_graphics(G, G->real_width, G->real_height);
}
