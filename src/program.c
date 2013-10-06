/*! @file program.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "program.h"
#include "gl_include.h"
#include "system.h"
#include "vertex.h"
#include "assert.h"

/* Defines
 */

/* Types
 */

/* Constants
 */
static const char* kAttributeSlotNames[] =
{
    "a_Position",   /* kPositionSlot */
    "a_Normal",     /* kNormalSlot */
    "a_Tangent",    /* kTangentSlot */
    "a_Bitangent",  /* kBitangentSlot */
    "a_TexCoord",   /* kTexCoordSlot */
};

/* Variables
 */

/* Internal functions
 */
static GLuint _load_shader(const char* filename, GLenum type)
{
    char*   data = NULL;
    size_t  data_size = 0;
    GLuint  shader = 0;
    GLint   compile_status = 0;
    int     result;
    GLint   shader_size = 0;

    result = (int)load_file_data(filename, (void*)&data, &data_size);
    if(result != 0) {
        system_log("Loading shader %s failed", filename);
        return 0;
    }
    assert(result == 0);
    shader_size = (GLint)data_size;

    shader = glCreateShader(type);
    ASSERT_GL(glShaderSource(shader, 1, (const char**)&data, &shader_size));
    ASSERT_GL(glCompileShader(shader));
    ASSERT_GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status));
    if(compile_status == GL_FALSE && 0) {
        char message[1024] = {0};
        ASSERT_GL(glGetShaderInfoLog(shader, sizeof(message), 0, message));
        system_log("Error compiling %s: %s", filename, message);
        assert(compile_status != GL_FALSE);
        free_file_data(data);
        return 0;
    }

    free_file_data(data);

    return shader;
}

/* External functions
 */
Program create_program(const char* vertex_shader_filename,
                       const char* fragment_shader_filename,
                       const AttributeSlot* slots)
{
    GLuint  vertex_shader;
    GLuint  fragment_shader;
    GLuint  program;
    GLint   link_status;

    /* Compile shaders */
    vertex_shader = _load_shader(vertex_shader_filename, GL_VERTEX_SHADER);
    fragment_shader = _load_shader(fragment_shader_filename, GL_FRAGMENT_SHADER);

    /* Create program */
    program = glCreateProgram();
    ASSERT_GL(glAttachShader(program, vertex_shader));
    ASSERT_GL(glAttachShader(program, fragment_shader));
    while(slots && *slots != kEmptySlot) {
        ASSERT_GL(glBindAttribLocation(program, *slots,    kAttributeSlotNames[*slots]));
        ++slots;
    }
    ASSERT_GL(glLinkProgram(program));
    ASSERT_GL(glGetProgramiv(program, GL_LINK_STATUS, &link_status));
    if(link_status == GL_FALSE) {
        char message[1024];
        ASSERT_GL(glGetProgramInfoLog(program, sizeof(message), 0, message));
        system_log("Creating program: %s--%s failed: %s\n", vertex_shader_filename, fragment_shader_filename, message);
        assert(link_status != GL_FALSE);
    }
    ASSERT_GL(glDetachShader(program, fragment_shader));
    ASSERT_GL(glDetachShader(program, vertex_shader));
    ASSERT_GL(glDeleteShader(fragment_shader));
    ASSERT_GL(glDeleteShader(vertex_shader));

    return program;
}

void destroy_program(Program program)
{
    glDeleteProgram(program);
}
