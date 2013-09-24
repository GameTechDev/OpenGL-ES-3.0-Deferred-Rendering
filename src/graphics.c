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
#include "gl_helper.h"
#include "geometry.h"
#include "system.h"
#include "assert.h"
#include "vec_math.h"

/* Defines
 */

/* Types
 */
typedef enum AttributeSlot
{
    kPositionSlot   = 0,
    kNormalSlot,
    kTexCoordSlot,
    kColorSlot
} AttributeSlot;
typedef enum VertexType
{
    kPosNormTexVertex,
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
    GLuint  projection_uniform;
    GLuint  modelview_uniform;
    GLuint  diffuse_uniform;

    Mesh  cube_mesh;
    Mesh  quad_mesh;

    GLuint  texture;

    GLuint  color_renderbuffer;
    GLuint  depth_renderbuffer;
    GLuint  framebuffer;

    int width;
    int height;

    GLuint  fullscreen_program;
    GLuint  fullscreen_texture_uniform;
};


/* Constants
 */
static const char* kAttributeSlotNames[] =
{
    "a_Position", /* kPositionSlot */
    "a_Normal",   /* kNormalSlot */
    "a_TexCoord", /* kTexCoordSlot */
    "a_Color", /* kColorSlot */
};
static const VertexDescription kVertexDescriptions[kNUM_VERTEX_TYPES][16] =
{
    { /* kPosNormTexVertex */
        { kPositionSlot,  3, },
        { kNormalSlot,    3, },
        { kTexCoordSlot,  2, },
        { 0, 0 }
    },
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
    GLuint vertex_shader = load_shader(vertex_shader_file, GL_VERTEX_SHADER);
    GLuint fragment_shader = load_shader(fragment_shader_file, GL_FRAGMENT_SHADER);
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
        create_buffer(GL_ARRAY_BUFFER, vertex_data, vertex_data_size),
        create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_data, index_data_size),
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
        graphics->diffuse_uniform = glGetUniformLocation(graphics->program, "s_Diffuse");
        system_log("Created program\n");
    }

    { /* Fullscreen time */
        AttributeSlot slots[] = {
            kPositionSlot,
            kTexCoordSlot
        };
        graphics->fullscreen_program = _create_program("fullscreen_vertex.glsl", "fullscreen_fragment.glsl", slots, 2);

        graphics->fullscreen_texture_uniform = glGetUniformLocation(graphics->fullscreen_program, "s_Diffuse");
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
    do {
        glVertexAttribPointer(desc->slot, desc->count, GL_FLOAT, GL_FALSE, mesh->vertex_size, (void*)ptr);
        ptr += sizeof(float) * desc->count;
    } while((++desc)->count);
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

    graphics->cube_mesh = _create_mesh(kCubeVertices, sizeof(kCubeVertices),
                                       kCubeIndices, sizeof(kCubeIndices),
                                       sizeof(kCubeIndices)/sizeof(kCubeIndices[0]),
                                       sizeof(kCubeVertices[0]), kPosNormTexVertex);

    graphics->quad_mesh = _create_mesh(kQuadVertices, sizeof(kQuadVertices),
                                       kQuadIndices, sizeof(kQuadIndices),
                                       sizeof(kQuadIndices)/sizeof(kQuadIndices[0]),
                                       sizeof(kQuadVertices[0]), kPosNormTexVertex);

    graphics->texture = load_texture("texture.png");

    CheckGLError();
    system_log("Graphics initialized\n");
    return graphics;
}
void render_graphics(Graphics* graphics)
{
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);


    /* Bind framebuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CheckGLError();

    glUseProgram(graphics->program);
    glEnableVertexAttribArray(kPositionSlot);
    glEnableVertexAttribArray(kNormalSlot);
    glEnableVertexAttribArray(kTexCoordSlot);
    CheckGLError();

    {
        static int count = 0;
        Transform t = {
            quat_from_euler(count/30.0f, count/30.0f*1.01f, 0.0f),
            vec3_create(sinf(count/30.0f), sinf(count/30.0f*1.1f), 7.0f),
            1.0f };
        Mat4 model = transform_get_matrix(t);
        Mat4 view = mat4_identity;
        Mat4 proj = mat4_perspective_fov(kPiDiv2, graphics->width/(float)graphics->height, 1.0f, 1000.0f);

        Mat4 model_view = mat4_multiply(model, view);
        glUniformMatrix4fv(graphics->modelview_uniform, 1, GL_FALSE, (float*)&model_view);
        glUniformMatrix4fv(graphics->projection_uniform, 1, GL_FALSE, (float*)&proj);

        count++;
    }

    _draw_mesh(&graphics->cube_mesh);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics->texture);
    glUniform1i(graphics->diffuse_uniform, 0);

    CheckGLError();

    /* Back to default */
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(graphics->fullscreen_program);
    glEnableVertexAttribArray(kPositionSlot );
    glEnableVertexAttribArray(kTexCoordSlot);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics->color_renderbuffer);
    glUniform1i(graphics->fullscreen_texture_uniform, 0);
    CheckGLError();

    _draw_mesh(&graphics->quad_mesh);

    glBindTexture(GL_TEXTURE_2D, 0);

    CheckGLError();
}
void destroy_graphics(Graphics* graphics)
{
    free(graphics);
}
