/*! @file graphics.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "graphics.h"
#include <stdlib.h>
#if defined(__APPLE__)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(__ANDROID__)
    #include <GLES3/gl3.h>
#else
    #error Need an OpenGL implementation
#endif
#include "system.h"
#include "assert.h"
#include "vec_math.h"

/* Defines
 */
#define CheckGLError()                  \
    do {                                \
        GLenum _glError = glGetError(); \
        if(_glError != GL_NO_ERROR) {   \
            system_log("OpenGL Error: %d\n", _glError);\
        }                               \
        assert(_glError == GL_NO_ERROR);\
    } while(__LINE__ == 0)

/* Types
 */
struct Graphics
{
    GLuint  program;
    GLuint  position_input;
    GLuint  color_input;
    GLuint  projection_uniform;
    GLuint  modelview_uniform;

    GLuint  vertex_buffer;
    GLuint  index_buffer;

    GLuint  color_renderbuffer;
    GLuint  depth_renderbuffer;
    GLuint  framebuffer;

    int width;
    int height;
};

typedef struct Vertex
{
    float position[3];
    float color[4];
} Vertex;

/* Constants
 */

static const Vertex kVertices[] = {
    { { 1, -1,  1}, {1, 0, 0, 1}},
    { { 1,  1,  1}, {0, 1, 0, 1}},
    { {-1,  1,  1}, {0, 0, 1, 1}},
    { {-1, -1,  1}, {0, 1, 1, 1}},
    { { 1, -1, -1}, {1, 0, 1, 1}},
    { { 1,  1, -1}, {1, 1, 0, 1}},
    { {-1,  1, -1}, {0, 1, 1, 1}},
    { {-1, -1, -1}, {1, 1, 1, 1}}
};

static const uint16_t kIndices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Back
    4, 6, 5,
    4, 7, 6,
    // Left
    2, 7, 3,
    7, 6, 2,
    // Right
    0, 4, 1,
    4, 1, 5,
    // Top
    6, 2, 1,
    1, 6, 5,
    // Bottom
    0, 3, 7,
    0, 7, 4
};

/* Variables
 */

/* Internal functions
 */
static GLuint _load_shader(const char* filename, GLenum type)
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
static GLuint _create_buffer(GLenum type, const void* data, size_t size)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(type, buffer);
    glBufferData(type, size, data, GL_STATIC_DRAW);
    glBindBuffer(type, 0);
    return buffer;
}
static void _setup_framebuffer(Graphics* graphics)
{
    /* Color buffer */
    glGenRenderbuffers(1, &graphics->color_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, graphics->color_renderbuffer);
    glRenderbufferStorage(  GL_RENDERBUFFER,
                            GL_RGB565,
                            graphics->width,
                            graphics->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CheckGLError();
    system_log("Created color buffer\n");

    /* Depth buffer */
    glGenRenderbuffers(1, &graphics->depth_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, graphics->depth_renderbuffer);
    glRenderbufferStorage(  GL_RENDERBUFFER,
                            GL_DEPTH_COMPONENT16,
                            graphics->width,
                            graphics->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CheckGLError();
    system_log("Created depth buffer\n");

    /* Framebuffer */
    glGenFramebuffers(1, &graphics->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, graphics->color_renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                              graphics->color_renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              graphics->depth_renderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGLError();
    system_log("Created framebuffer\n");
}
static void _setup_program(Graphics* graphics)
{
    GLuint vertex_shader = _load_shader("SimpleVertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = _load_shader("SimpleFragment.glsl", GL_FRAGMENT_SHADER);
    GLint  link_status;

    /* Create program */
    graphics->program = glCreateProgram();
    glAttachShader(graphics->program, vertex_shader);
    glAttachShader(graphics->program, fragment_shader);
    glLinkProgram(graphics->program);
    glGetProgramiv(graphics->program, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE) {
        char message[1024];
        glGetProgramInfoLog(graphics->program, sizeof(message), 0, message);
        system_log(message);
        assert(link_status != GL_FALSE);
    }
    glDetachShader(graphics->program, fragment_shader);
    glDetachShader(graphics->program, vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    /* Get program data */
    graphics->projection_uniform = glGetUniformLocation(graphics->program, "Projection");
    graphics->modelview_uniform = glGetUniformLocation(graphics->program, "ModelView");
    graphics->position_input = glGetAttribLocation(graphics->program, "Position");
    graphics->color_input = glGetAttribLocation(graphics->program, "SourceColor");
    system_log("Created program\n");
}

/* External functions
 */
Graphics* create_graphics(int width, int height)
{
    Graphics* graphics = NULL;

    /* Allocate device */
    graphics = (Graphics*)calloc(1, sizeof(*graphics));
    graphics->width = width;
    graphics->height = height;
    system_log("Graphics created. W: %d  H: %d\n", width, height);

    /* Perform GL initialization */
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    system_log("OpenGL initialized\n");

    /* Perform other initialization */
    _setup_framebuffer(graphics);
    _setup_program(graphics);
    graphics->vertex_buffer = _create_buffer(GL_ARRAY_BUFFER, kVertices, sizeof(kVertices));
    graphics->index_buffer = _create_buffer(GL_ELEMENT_ARRAY_BUFFER, kIndices, sizeof(kIndices));

    CheckGLError();
    return graphics;
}
void render(Graphics* graphics)
{
    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(graphics->program);
    glBindBuffer(GL_ARRAY_BUFFER, graphics->vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphics->index_buffer);

    {
        static int count = 0;
        Transform t = {
            quat_from_euler(count/30.0f, count/30.0f*1.01f, 0.0f),
            vec3_create(sinf(count/30.0f), sinf(count/30.0f*1.1f), 7.0f),
            1.0f };
        Mat4 model = transform_get_matrix(t);
        Mat4 view = mat4_identity;
        //Mat4 proj = mat4_perspective(2, 3, 1.0f, 1000.0f);
        Mat4 proj = mat4_perspective_fov(kPiDiv2, graphics->width/(float)graphics->height, 1.0f, 1000.0f);

        Mat4 model_view = mat4_multiply(model, view);

        glUniformMatrix4fv(graphics->modelview_uniform, 1, GL_FALSE, (float*)&model_view);
        glUniformMatrix4fv(graphics->projection_uniform, 1, GL_FALSE, (float*)&proj);

        count++;
    }

    glEnableVertexAttribArray(graphics->position_input);
    glEnableVertexAttribArray(graphics->color_input);
    glVertexAttribPointer(graphics->position_input, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(graphics->color_input, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float)*3));

    glDrawElements(GL_TRIANGLES, sizeof(kIndices)/sizeof(kIndices[0]), GL_UNSIGNED_SHORT, NULL);

    
    CheckGLError();
}
void destroy_graphics(Graphics* graphics)
{
    free(graphics);
}
