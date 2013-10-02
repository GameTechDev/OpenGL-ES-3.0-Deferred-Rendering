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
#include "assert.h"
#include "system.h"

/** @brief OpenGL Error code strings
 */
#define STATUS_CASE(enum) case enum: return #enum
static const char* _glStatusString(GLenum error)
{
    switch(error) {
        STATUS_CASE(GL_NO_ERROR);
        STATUS_CASE(GL_INVALID_ENUM);
        STATUS_CASE(GL_INVALID_VALUE);
        STATUS_CASE(GL_INVALID_OPERATION);
        STATUS_CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
        STATUS_CASE(GL_OUT_OF_MEMORY);
        STATUS_CASE(GL_FRAMEBUFFER_COMPLETE);
        STATUS_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
        STATUS_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
        STATUS_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
        STATUS_CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
        STATUS_CASE(GL_FRAMEBUFFER_UNSUPPORTED);
    }
    return NULL;
}
#undef STATUS_CASE

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
                            #x, _glStatusString(_glError));  \
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
                            _glStatusString(_glError));  \
            }                                           \
        } while(__LINE__ == -1)
#endif /* #ifndef CheckGLError */

#endif
