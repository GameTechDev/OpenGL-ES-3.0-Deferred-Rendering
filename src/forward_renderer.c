/*! @file forward_renderer.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "renderer.h"
#include "gl_helper.h"
#include "geometry.h"

/* Defines
 */

/* Types
 */

/* Constants
 */

/* Variables
 */
static GLuint  program = 0;

static GLuint  projection = 0;
static GLuint  view = 0;
static GLuint  world = 0;

static GLuint  albedo = 0;
static GLuint  normal = 0;

static GLuint  light_positions = 0;
static GLuint  light_colors = 0;
static GLuint  light_sizes = 0;
static GLuint  num_lights = 0;

static GLuint  camera_position = 0;
static GLuint  specular_color = 0;
static GLuint  specular_power = 0;
static GLuint  specular_coefficient = 0;

static GLuint  sun_direction = 0;
static GLuint  sun_color = 0;

/* Internal functions
 */
static void _draw_mesh(const Mesh* mesh)
{
    const VertexDescription* desc = kVertexDescriptions[mesh->type];
    intptr_t ptr = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    glVertexAttribPointer(kPositionSlot,    3, GL_FLOAT, GL_FALSE, 56, (void*)(ptr+=0));
    glVertexAttribPointer(kNormalSlot,      3, GL_FLOAT, GL_FALSE, 56, (void*)(ptr+=3));
    glVertexAttribPointer(kTangentSlot,     3, GL_FLOAT, GL_FALSE, 56, (void*)(ptr+=3));
    glVertexAttribPointer(kBitangentSlot,   3, GL_FLOAT, GL_FALSE, 56, (void*)(ptr+=3));
    glVertexAttribPointer(kTexCoordSlot,    2, GL_FLOAT, GL_FALSE, 56, (void*)(ptr+=3));
    glDrawElements(GL_TRIANGLES, mesh->index_count, mesh->index_format, NULL);
    CheckGLError();
}

/* External functions
 */
#if 0
void forward_renderer(Mat4 proj_matrix, Mat4 view_matrix,
                      RenderCommand* commands, int num_commands,
                      Light* lights, int num_lights)
{
    Vec3    light_positions[1024] = {0};
    Vec3    light_colors[1024] = {0};
    float   light_sizes[1024] = {0};

    /* Calculate constants */
    Mat4 inv_view_matrix = mat4_inverse(view_matrix);
    Vec3 camera_position = vec3_from_vec4(inv_view_matrix.r3);

    /* Transform to view space */
    for(ii=0;ii < num_lights; ++ii) {
        Vec4 light_position = vec4_from_vec3(lights[ii]);
        light_position = mat4_mul_vector(light_position, view_matrix);
        light_positions[ii] = vec3_from_vec4(light_position);
        light_colors[ii] = lights[ii].color;
        light_sizes[ii] = lights[ii].size;
    }

    /* Set up OpenGL state */
    glUseProgram(program);

    /* Update uniforms */
    glUniform3fv(camera_position, 1, (float*)&graphics->view_transform.position);
    glUniformMatrix4fv(projection, 1, GL_FALSE, (float*)&proj_matrix);
    glUniformMatrix4fv(view, 1, GL_FALSE, (float*)&view_matrix);
    CheckGLError();

    glUniform3fv(light_positions, num_lights, (float*)light_positions);
    glUniform3fv(light_colors, num_lights, (float*)light_colors);
    glUniform1fv(light_sizes, num_lights, (float*)light_sizes);
    glUniform1i(num_lights, num_lights);
    CheckGLError();

        /* Loop through render commands */
    for(ii=0;ii<graphics->num_commands;++ii) {
        RenderCommand command = commands[ii];
        glUniformMatrix4fv(world, 1, GL_FALSE, (float*)&command.world);
        glUniform3fv(specular_color, 1, (float*)&command.material->specular_color);
        glUniform1f(specular_power  , command.material->specular_power);
        glUniform1f(specular_coefficient, command.material->specular_coefficient);
        CheckGLError();

        glActiveTexture(GL_TEXTURE0);
        if(command.material->albedo_tex)
            glBindTexture(GL_TEXTURE_2D, command.material->albedo_tex->texture);
        glActiveTexture(GL_TEXTURE1);
        if(command.material->normal_tex)
            glBindTexture(GL_TEXTURE_2D, command.material->normal_tex->texture);

        _draw_mesh(command.mesh);
    }
}
#endif
