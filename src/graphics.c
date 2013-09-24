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
            system_log("%s:%d:  OpenGL Error: %d\n", __FILE__, __LINE__, _glError);\
        }                               \
        assert(_glError == GL_NO_ERROR);\
    } while(__LINE__ == 0)

/* Types
 */
typedef enum AttributeSlot
{
    kPositionSlot   = 0,
    kTexCoordSlot   = 1,
    kColorSlot      = 2
} AttributeSlot;
typedef enum VertexType
{
    kPosColorVertex,
    kPosTexVertex,

    kNUM_VERTEX_TYPES
} VertexType;

typedef struct VertexDescription
{
    AttributeSlot   slot;
    int             count;
} VertexDescription;

typedef struct Mesh
{
    GLuint      vertex_buffer;
    GLuint      index_buffer;
    int         index_count;
    int         vertex_size;
    VertexType  type;
} Mesh;

struct Graphics
{
    GLuint  program;
    GLuint  position_input;
    GLuint  color_input;
    GLuint  projection_uniform;
    GLuint  modelview_uniform;

    Mesh  cube_mesh;

    GLuint  color_renderbuffer;
    GLuint  depth_renderbuffer;
    GLuint  framebuffer;

    int width;
    int height;

    GLuint  fullscreen_vertex_buffer;
    GLuint  fullscreen_index_buffer;
    GLuint  fullscreen_program;
    GLuint  fullscreen_position_input;
    GLuint  fullscreen_texcoord_input;
    GLuint  fullscreen_texture_uniform;
};

typedef struct PosColorVertex
{
    float position[3];
    float color[4];
} PosColorVertex;
typedef struct PosTexVertex
{
    Vec3    pos;
    Vec2    tex;
} PosTexVertex;

/* Constants
 */
static const char* kAttributeSlotNames[] =
{
    "a_Position", /* kPositionSlot */
    "a_TexCoord", /* kTexCoordSlot */
    "a_Color", /* kColorSlot */
};
static const VertexDescription kVertexDescriptions[kNUM_VERTEX_TYPES][16] =
{
    { /* kPosColorVertex */
        { kPositionSlot,  3, },
        { kColorSlot,     4, },
        { 0, 0 }
    },
    { /* kPosTexVertex */
        { kPositionSlot,  3, },
        { kTexCoordSlot,  2, },
        { 0, 0 }
    }
};
static const PosTexVertex kQuadVertices[] =
{
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f }, },
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f }, },
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, },
    { {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, },
};
static const uint16_t kQuadIndices[] =
{
    0, 1, 2,
    0, 2, 3,
};

static const PosColorVertex kVertices[] = {
    { { 1, -1,  1}, {1, 0, 0, 1}},
    { { 1,  1,  1}, {0, 1, 0, 1}},
    { {-1,  1,  1}, {0, 0, 1, 1}},
    { {-1, -1,  1}, {0, 1, 1, 1}},
    { { 1, -1, -1}, {1, 0, 1, 1}},
    { { 1,  1, -1}, {1, 1, 0, 1}},
    { {-1,  1, -1}, {0, 0, 0, 1}},
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
    #if 0
    glGenRenderbuffers(1, &graphics->color_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, graphics->color_renderbuffer);
    glRenderbufferStorage(  GL_RENDERBUFFER,
                            GL_RGB565,
                            graphics->width,
                            graphics->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CheckGLError();
    #else
    glGenTextures(1, &graphics->color_renderbuffer);
    glBindTexture(GL_TEXTURE_2D, graphics->color_renderbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    {
        uint8_t* tex = malloc(graphics->width*graphics->height*4);
        int ii=0;
        for(;ii<graphics->width*graphics->height*4;++ii)
            tex[ii] = 64;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, graphics->width, graphics->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
        free(tex);
    }

    #endif
    system_log("Created color buffer\n");

    /* Depth buffer */
    glGenRenderbuffers(1, &graphics->depth_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, graphics->depth_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT16,
                          graphics->width,
                          graphics->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    CheckGLError();
    system_log("Created depth buffer\n");

    /* Framebuffer */
    glGenFramebuffers(1, &graphics->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphics->color_renderbuffer, 0);
    //    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
    //                              GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
    //                              graphics->color_renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              graphics->depth_renderbuffer);
    {
        GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        switch (framebufferStatus) {
            case GL_FRAMEBUFFER_COMPLETE: break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                system_log("Framebuffer Object %d Error: Attachment Point Unconnected", graphics->framebuffer);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                system_log("Framebuffer Object %d Error: Missing Attachment", graphics->framebuffer);
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                system_log("Framebuffer Object %d Error: Dimensions do not match", graphics->framebuffer);
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                system_log("Framebuffer Object %d Error: Unsupported Framebuffer Configuration", graphics->framebuffer);
                break;
            default:
                system_log("Framebuffer Object %d Error: Unkown Framebuffer Object Failure", graphics->framebuffer);
                break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError();
    system_log("Created framebuffer\n");
}
static GLuint _create_program(const char* vertex_shader_file, const char* fragment_shader_file,
                              const AttributeSlot* attribute_slots, int num_attributes )
{
    GLuint vertex_shader = _load_shader(vertex_shader_file, GL_VERTEX_SHADER);
    GLuint fragment_shader = _load_shader(fragment_shader_file, GL_FRAGMENT_SHADER);
    GLuint program;
    GLint  link_status;
    int ii;

    /* Create program */
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    for(ii=0;ii<num_attributes;++ii) {
        glBindAttribLocation(program, attribute_slots[ii], kAttributeSlotNames[attribute_slots[ii]]);
    }
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE) {
        char message[1024];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        system_log(message);
        assert(link_status != GL_FALSE);
    }
    glDetachShader(program, fragment_shader);
    glDetachShader(program, vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    CheckGLError();

    return program;
}
static Mesh _create_mesh(const void* vertex_data, size_t vertex_data_size,
                         const void* index_data, size_t index_data_size,
                         int index_count, int vertex_size, VertexType type)
{
    Mesh mesh = {
        _create_buffer(GL_ARRAY_BUFFER, vertex_data, vertex_data_size),
        _create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_data, index_data_size),
        index_count,
        vertex_size,
        type
    };
    return mesh;
}
static void _setup_programs(Graphics* graphics)
{
    { /* Create 3D program */
        AttributeSlot slots[] = {
            kPositionSlot,
            kColorSlot
        };
        graphics->program = _create_program("SimpleVertex.glsl", "SimpleFragment.glsl", slots, 2);

        graphics->projection_uniform = glGetUniformLocation(graphics->program, "Projection");
        graphics->modelview_uniform = glGetUniformLocation(graphics->program, "ModelView");
        graphics->position_input = glGetAttribLocation(graphics->program, "a_Position");
        graphics->color_input = glGetAttribLocation(graphics->program, "a_Color");
        system_log("Created program\n");
    }

    { /* Fullscreen time */
        AttributeSlot slots[] = {
            kPositionSlot,
            kTexCoordSlot
        };
        graphics->fullscreen_program = _create_program("fullscreen_vertex.glsl", "fullscreen_fragment.glsl", slots, 2);

        graphics->fullscreen_texture_uniform = glGetUniformLocation(graphics->fullscreen_program, "s_Diffuse");
        graphics->fullscreen_position_input = glGetAttribLocation(graphics->fullscreen_program, "a_Position");
        graphics->fullscreen_texcoord_input = glGetAttribLocation(graphics->fullscreen_program, "a_TexCoord");
        system_log("Created fullscreen program\n");
    }
    CheckGLError();
}
static void _draw_mesh(const Mesh* mesh)
{
    const VertexDescription* desc = kVertexDescriptions[mesh->type];
    intptr_t ptr = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    while(desc->count) {
        glVertexAttribPointer(desc->slot, desc->count, GL_FLOAT, GL_FALSE, mesh->vertex_size, (void*)ptr);
        ptr += sizeof(float)*desc->count;
        desc++;
    }
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_SHORT, NULL);
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
    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
    system_log("OpenGL initialized\n");

    /* Perform other initialization */
    _setup_framebuffer(graphics);
    _setup_programs(graphics);

    graphics->cube_mesh = _create_mesh(kVertices, sizeof(kVertices),
                                       kIndices, sizeof(kIndices),
                                       sizeof(kIndices)/sizeof(kIndices[0]),
                                       sizeof(kVertices[0]), kPosColorVertex);

    graphics->fullscreen_vertex_buffer = _create_buffer(GL_ARRAY_BUFFER, kQuadVertices, sizeof(kQuadVertices));
    graphics->fullscreen_index_buffer = _create_buffer(GL_ELEMENT_ARRAY_BUFFER, kQuadIndices, sizeof(kQuadIndices));

    CheckGLError();
    system_log("Graphics initialized\n");
    return graphics;
}
void render_graphics(Graphics* graphics)
{
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    #if 1
    /* Bind framebuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    CheckGLError();

    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CheckGLError();

    glUseProgram(graphics->program);
    glEnableVertexAttribArray(kPositionSlot);
    glEnableVertexAttribArray(kColorSlot);
    CheckGLError();

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

    _draw_mesh(&graphics->cube_mesh);
    
    //glDrawElements(GL_TRIANGLES, graphics->cube_mesh.index_count, GL_UNSIGNED_SHORT, NULL);

    CheckGLError();
    #endif

    #if 1
    /* Back to default */
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(graphics->fullscreen_program);
    glEnableVertexAttribArray(graphics->fullscreen_position_input);
    glEnableVertexAttribArray(graphics->fullscreen_texcoord_input);

    CheckGLError();
    glBindBuffer(GL_ARRAY_BUFFER, graphics->fullscreen_vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphics->fullscreen_index_buffer);

    CheckGLError();
    glVertexAttribPointer(graphics->fullscreen_position_input, 3, GL_FLOAT, GL_FALSE, sizeof(PosTexVertex), 0);
    glVertexAttribPointer(graphics->fullscreen_texcoord_input, 2, GL_FLOAT, GL_FALSE, sizeof(PosTexVertex), (void*)(sizeof(float)*3));

    CheckGLError();
    glActiveTexture(GL_TEXTURE0);
    CheckGLError();
    glBindTexture(GL_TEXTURE_2D, graphics->color_renderbuffer);
    CheckGLError();
    glUniform1i(graphics->fullscreen_texture_uniform, 0);
    CheckGLError();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    CheckGLError();


    glBindTexture(GL_TEXTURE_2D, 0);
    #endif

    CheckGLError();
}
void destroy_graphics(Graphics* graphics)
{
    free(graphics);
}
