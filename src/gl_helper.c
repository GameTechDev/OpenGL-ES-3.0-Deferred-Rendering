/*! @file gl_helper.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "gl_helper.h"
#include <stdlib.h>
#include "assert.h"
#include "system.h"
#include "stb_image.h"

/* Defines
 */

/* Types
 */

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */
GLuint create_buffer(GLenum type, const void* data, size_t size)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, size, data, GL_STATIC_DRAW);
    glBindBuffer(type, 0);
    return buffer;
}
GLuint load_shader(const char* filename, GLenum type)
{
    char        buffer[1024*16] = {0};
    const char* source = buffer;
    GLuint      shader = 0;
    GLint       compile_status = 0;
    int         file_size = 0;

    system_log("Attempting to load shader %s\n", filename);
    file_size = (int)load_file_contents(filename, buffer, sizeof(buffer));
    if(file_size == 0)
        system_log("Loading shader %s failed", filename);
    assert(file_size && file_size < (int)sizeof(buffer));

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &file_size);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if(compile_status == GL_FALSE) {
        char message[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(message), 0, message);
        system_log(message);
        return 0;
    }

    return shader;
}
GLuint load_texture(const char* filename)
{
    void* file_data;
    size_t data_size;
    int width, height, components;
    void* tex_data;
    GLenum format = GL_RGB;
    GLuint texture;

    data_size = 1024*1024*4;
    file_data = calloc(1, data_size);
    data_size = load_file_contents(filename, file_data, data_size);
    assert(data_size);

    tex_data = stbi_load_from_memory(file_data, data_size, &width, &height, &components, 0);
    assert(tex_data);

    glGenTextures(1, &texture);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGLError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    switch(components)
    {
    case 4:
        format = GL_RGBA;
        components = GL_RGBA;
        break;
    case 3:
        format = GL_RGB;
        components = GL_RGB;
        break;
    default:
        assert(0);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, tex_data);
    CheckGLError();
    glGenerateMipmap(GL_TEXTURE_2D);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError();

    stbi_image_free(tex_data);
    free(file_data);

    return texture;
}
