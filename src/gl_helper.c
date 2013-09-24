/*! @file gl_helper.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "gl_helper.h"
#include "assert.h"
#include "system.h"

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
