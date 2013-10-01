/*! @file gl_include.h
 *  @brief Abstract out platform OpenGL include files
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __gl_include_h__
#define __gl_include_h__

#if defined(__APPLE__)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(__ANDROID__)
    #include <GLES3/gl3.h>
#else
    #error Need an OpenGL implementation
#endif

#include "system.h"

/** @brief OpenGL Error code strings
 */
#define ERROR_CASE(enum) case enum: return #enum
static const char* _glErrorString(GLenum error)
{
    switch(error) {
        ERROR_CASE(GL_NO_ERROR);
        ERROR_CASE(GL_INVALID_ENUM);
        ERROR_CASE(GL_INVALID_VALUE);
        ERROR_CASE(GL_INVALID_OPERATION);
        ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
        ERROR_CASE(GL_OUT_OF_MEMORY);
    }
    return NULL;
}
#undef ERROR_CASE

/** @brief OpenGL Error checking wrapper
 */
#ifndef ASSERT_GL
    #define ASSERT_GL(x)                                    \
        do {                                                \
            GLenum _glError;                                \
            x;                                              \
            _glError = glGetError();                        \
            if(_glError != GL_NO_ERROR) {                   \
                system_log("%s:%d:  %s Error: %s\n",        \
                            __FILE__, __LINE__,             \
                            #x, _glErrorString(_glError));  \
            }                                               \
        } while(__LINE__ == -1)
#endif /* #ifndef ASSERT_GL */

/** @brief Manual OpenGL error checking
 */
#ifndef CheckGLError
    #define CheckGLError()                              \
        do {                                            \
            GLenum _glError = glGetError();             \
            if(_glError != GL_NO_ERROR) {               \
                system_log("%s:%d:  OpenGL Error: %s\n",\
                            __FILE__, __LINE__,         \
                            _glErrorString(_glError));  \
            }                                           \
        } while(__LINE__ == -1)
#endif /* #ifndef CheckGLError */

#endif
