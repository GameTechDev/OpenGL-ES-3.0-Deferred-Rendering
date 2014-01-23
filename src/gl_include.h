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
    #ifndef NDEBUG
        #define ASSERT_GL(x)                                    \
            do {                                                \
                GLenum _glError;                                \
                x;                                              \
                _glError = glGetError();                        \
                if(_glError != GL_NO_ERROR) {                   \
                    system_log("%s:%d:  %s Error: %s\n",        \
                                __FILE__, __LINE__,             \
                                #x, _glStatusString(_glError)); \
                }                                               \
            } while(__LINE__ == -1)
    #else
        #define ASSERT_GL(x)
    #endif /* NDEBUG */
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
