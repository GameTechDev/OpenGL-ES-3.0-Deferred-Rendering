/*! @file gl_helper.h
 *  @brief Various helper OpenGL functions
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __gl_helper_h__
#define __gl_helper_h__

#if defined(__APPLE__)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(__ANDROID__)
    #include <GLES3/gl3.h>
#else
    #error Need an OpenGL implementation
#endif
#include "system.h"

#define CheckGLError()                  \
    do {                                \
        GLenum _glError = glGetError(); \
        if(_glError != GL_NO_ERROR) {   \
            system_log("%s:%d:  OpenGL Error: %d\n", __FILE__, __LINE__, _glError);\
        }                               \
        assert(_glError == GL_NO_ERROR);\
    } while(__LINE__ == 0)


GLuint gl_create_buffer(GLenum type, const void* data, size_t size);
GLuint gl_load_shader(const char* filename, GLenum type);
GLuint gl_load_texture(const char* filename);

#endif /* include guard */
