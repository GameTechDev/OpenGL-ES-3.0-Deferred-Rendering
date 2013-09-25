/*! @file graphics.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "graphics.h"
#include <stdlib.h>
#include <string.h>
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
#define MAX_MESHES 32
#define MAX_TEXTURES 64
#define MAX_RENDER_COMMANDS 1024

/* Types
 */

typedef struct Mesh
{
    GLuint      vertex_buffer;
    GLuint      index_buffer;
    int         index_count;
    int         vertex_size;
    VertexType  type;
} Mesh;

typedef struct RenderCommand
{
    Transform   transform;
    MeshID      mesh;
    TextureID   diffuse;
} RenderCommand;

struct Graphics
{
    GLuint  program;
    GLuint  projection_uniform;
    GLuint  view_uniform;
    GLuint  world_uniform;
    GLuint  diffuse_uniform;
    GLuint  lightdir_uniform;

    GLuint  color_renderbuffer;
    GLuint  depth_renderbuffer;
    GLuint  depth_texture;
    GLuint  framebuffer;

    int width;
    int height;

    Mat4    projection_matrix;
    Transform   view_transform;

    GLuint  fullscreen_program;
    GLuint  fullscreen_texture_uniform;

    Mesh    meshes[MAX_MESHES];
    int     num_meshes;

    GLuint  textures[MAX_TEXTURES];
    int     num_textures;

    MeshID  cube_mesh;
    MeshID  quad_mesh;

    RenderCommand   commands[MAX_RENDER_COMMANDS];
    int num_commands;
};


/* Constants
 */

/* Variables
 */

/* Internal functions
 */
static MeshID _new_mesh_id(Graphics* graphics)
{
    assert(graphics->num_meshes < MAX_MESHES);
    return graphics->num_meshes++;
}
static void _setup_framebuffer(Graphics* graphics)
{
    GLenum framebuffer_status;

    /* Color buffer */
    glGenTextures(1, &graphics->color_renderbuffer);
    glBindTexture(GL_TEXTURE_2D, graphics->color_renderbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, graphics->width, graphics->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    CheckGLError();

    /* Depth buffer */
    glGenTextures(1, &graphics->depth_renderbuffer);
    glBindTexture(GL_TEXTURE_2D, graphics->depth_renderbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, graphics->width, graphics->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
    CheckGLError();

    /* Framebuffer */
    glGenFramebuffers(1, &graphics->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphics->color_renderbuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, graphics->depth_renderbuffer, 0);

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (framebuffer_status) {
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


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError();

    system_log("Created framebuffer\n");
}
static GLuint _create_program(const char* vertex_shader_file, const char* fragment_shader_file,
                              const AttributeSlot* attribute_slots, int num_attributes )
{
    GLuint vertex_shader = gl_load_shader(vertex_shader_file, GL_VERTEX_SHADER);
    GLuint fragment_shader = gl_load_shader(fragment_shader_file, GL_FRAGMENT_SHADER);
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
        gl_create_buffer(GL_ARRAY_BUFFER, vertex_data, vertex_data_size),
        gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_data, index_data_size),
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
            kNormalSlot,
            kTexCoordSlot
        };
        graphics->program = _create_program("SimpleVertex.glsl", "SimpleFragment.glsl", slots, 3);

        graphics->projection_uniform = glGetUniformLocation(graphics->program, "Projection");
        graphics->view_uniform = glGetUniformLocation(graphics->program, "View");
        graphics->world_uniform = glGetUniformLocation(graphics->program, "World");
        graphics->diffuse_uniform = glGetUniformLocation(graphics->program, "s_Diffuse");
        graphics->lightdir_uniform = glGetUniformLocation(graphics->program, "LightDir");
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
    glClearDepthf(1.0f);
    system_log("OpenGL version:\t%s\n", glGetString(GL_VERSION));
    system_log("OpenGL renderer:\t%s\n", glGetString(GL_RENDERER));

    /* Perform other initialization */
    _setup_framebuffer(graphics);
    _setup_programs(graphics);

    graphics->projection_matrix = mat4_perspective_fov(kPiDiv2,
                                                       width/(float)height,
                                                       0.1f,
                                                       1000.0f);
    graphics->view_transform = transform_zero;

    graphics->cube_mesh = _new_mesh_id(graphics);
    graphics->quad_mesh = _new_mesh_id(graphics);

    graphics->meshes[graphics->cube_mesh] = _create_mesh(kCubeVertices, sizeof(kCubeVertices),
                                                         kCubeIndices, sizeof(kCubeIndices),
                                                         sizeof(kCubeIndices)/sizeof(kCubeIndices[0]),
                                                         sizeof(kCubeVertices[0]), kPosNormTexVertex);

    graphics->meshes[graphics->quad_mesh]  = _create_mesh(kQuadVertices, sizeof(kQuadVertices),
                                                          kQuadIndices, sizeof(kQuadIndices),
                                                          sizeof(kQuadIndices)/sizeof(kQuadIndices[0]),
                                                          sizeof(kQuadVertices[0]), kPosNormTexVertex);

    CheckGLError();
    system_log("Graphics initialized\n");

    { /* Print extensions */
        char buffer[1024*16] = {0};
        uint32_t ii;
        strcpy(buffer,(const char*)glGetString(GL_EXTENSIONS));
        for(ii=0;ii<strlen(buffer);++ii) {
            if(buffer[ii] == ' ')
                buffer[ii] = '\n';
        }
        system_log("%s\n", buffer);
    }
    return graphics;
}
void render_graphics(Graphics* graphics)
{
    Mat4 view_matrix = mat4_inverse(transform_get_matrix(graphics->view_transform));
    int ii;

    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);

    /* Bind framebuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->framebuffer);
    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CheckGLError();

    glUseProgram(graphics->program);
    glEnableVertexAttribArray(kPositionSlot);
    glEnableVertexAttribArray(kNormalSlot);
    glEnableVertexAttribArray(kTexCoordSlot);
    CheckGLError();
    glUniformMatrix4fv(graphics->projection_uniform, 1, GL_FALSE, (float*)&graphics->projection_matrix);
    glUniformMatrix4fv(graphics->view_uniform, 1, GL_FALSE, (float*)&view_matrix);
    {
        Vec4 light_dir = { 0.0f, -1.0f, 0.0f, 0.0f };
        glUniform4fv(graphics->lightdir_uniform, 1, (float*)&light_dir);
    }

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(graphics->diffuse_uniform, 0);

    /* Loop through render commands */
    for(ii=0;ii<graphics->num_commands;++ii) {
        RenderCommand command = graphics->commands[ii];
        Mat4 model = transform_get_matrix(command.transform);
        glUniformMatrix4fv(graphics->world_uniform, 1, GL_FALSE, (float*)&model);
        glBindTexture(GL_TEXTURE_2D, graphics->textures[command.diffuse]);
        _draw_mesh(&graphics->meshes[command.mesh]);
    }
    graphics->num_commands = 0;

    CheckGLError();

    /* Back to default */
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(graphics->fullscreen_program);
    glEnableVertexAttribArray(kPositionSlot );
    glEnableVertexAttribArray(kTexCoordSlot);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, graphics->color_renderbuffer);
    glUniform1i(graphics->fullscreen_texture_uniform, 0);
    CheckGLError();

    _draw_mesh(&graphics->meshes[graphics->quad_mesh]);

    glBindTexture(GL_TEXTURE_2D, 0);

    CheckGLError();
}
void destroy_graphics(Graphics* graphics)
{
    free(graphics);
}
MeshID cube_mesh(Graphics* graphics)
{
    return graphics->cube_mesh;
}
MeshID quad_mesh(Graphics* graphics)
{
    return graphics->quad_mesh;
}
void add_render_command(Graphics* graphics, MeshID mesh, TextureID diffuse, Transform transform)
{
    int index = graphics->num_commands++;
    assert(index < MAX_RENDER_COMMANDS);
    graphics->commands[index].mesh = mesh;
    graphics->commands[index].transform = transform;
    graphics->commands[index].diffuse = diffuse;
}
TextureID load_texture(Graphics* graphics, const char* filename)
{
    int index = graphics->num_textures++;
    assert(graphics->num_textures <= MAX_TEXTURES);
    graphics->textures[index] = gl_load_texture(filename);
    return index;
}
void set_view_transform(Graphics* graphics, Transform view)
{
    graphics->view_transform = view;
}


