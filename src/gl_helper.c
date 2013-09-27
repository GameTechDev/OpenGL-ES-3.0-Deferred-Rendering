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
GLuint gl_create_buffer(GLenum type, const void* data, size_t size)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, size, data, GL_STATIC_DRAW);
    glBindBuffer(type, 0);
    return buffer;
}
GLuint gl_load_shader(const char* filename, GLenum type)
{
    void*   data = NULL;
    size_t  data_size = 0;
    GLuint  shader = 0;
    GLint   compile_status = 0;
    int     result;
    GLint   shader_size = 0;

    result = (int)load_file_data(filename, &data, &data_size);
    if(result != 0) {
        system_log("Loading shader %s failed", filename);
        return 0;
    }
    assert(result == 0);
    shader_size = (GLint)data_size;

    shader = glCreateShader(type);
    CheckGLError();
    glShaderSource(shader, 1, (const char**)&data, &shader_size);
    CheckGLError();
    glCompileShader(shader);
    CheckGLError();
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    CheckGLError();
    if(compile_status == GL_FALSE) {
        char message[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(message), 0, message);
        system_log("Error compiling %s: %s", filename, message);
        assert(compile_status != GL_FALSE);
        return 0;
    }

    free_file_data(data);

    return shader;
}
GLuint gl_load_texture(const char* filename)
{
    void*   file_data = NULL;
    size_t  file_size = 0;
    uint8_t*    texture_data = NULL;
    int width, height, components;
    GLuint      texture;
    GLenum      format;
    int         result;

    result = load_file_data(filename, &file_data, &file_size);
    assert(result == 0);

    texture_data = stbi_load_from_memory(file_data, (int)file_size, &width, &height, &components, 0);
    assert(texture_data);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGLError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    switch( components ) {
        case 1: {
            // Gray
            format = GL_LUMINANCE;
            break;
        }
        case 2: {
            // Gray and Alpha
            format = GL_LUMINANCE_ALPHA;
            break;
        }
        case 3: {
            // RGB
            format = GL_RGB;
            break;
        }
        case 4: {
            // RGBA
            format = GL_RGBA;
            break;
        }
        default: {
            // Unknown format
            assert(0);
            return 0;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    CheckGLError();
    glGenerateMipmap(GL_TEXTURE_2D);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError();

    stbi_image_free(texture_data);
    free_file_data(file_data);

    return texture;
}
