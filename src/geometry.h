/** @file geometry.h
 *  @brief Geometry descriptions
 *  @copyright  Copyright (c) 2013 Intel. All rights reserved.
 */
#ifndef _geometry_h_
#define _geometry_h_

#include <stdint.h>
#include <stddef.h>
#if defined(__APPLE__)
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
#elif defined(__ANDROID__)
    #include <GLES3/gl3.h>
#else
    #error Need an OpenGL implementation
#endif
#include "vec_math.h"

/** Vertex types
 */
typedef struct PosColorVertex
{
    Vec3    position;
    Vec4    color;
} PosColorVertex;
typedef struct PosTexVertex
{
    Vec3    position;
    Vec2    tex;
} PosTexVertex;
typedef struct PosNormTexVertex
{
    Vec3    position;
    Vec3    norm;
    Vec2    tex;
} PosNormTexVertex;

typedef enum VertexType
{
    kPosNormTexVertex,
    kPosColorVertex,
    kPosTexVertex,

    kNUM_VERTEX_TYPES
} VertexType;


typedef enum AttributeSlot
{
    kPositionSlot   = 0,
    kNormalSlot,
    kTexCoordSlot,
    kColorSlot,

    kEmptySlot = -1
} AttributeSlot;

static const char* kAttributeSlotNames[] =
{
    "a_Position", /* kPositionSlot */
    "a_Normal",   /* kNormalSlot */
    "a_TexCoord", /* kTexCoordSlot */
    "a_Color",    /* kColorSlot */
};

typedef struct Mesh
{
    GLuint      vertex_buffer;
    GLuint      index_buffer;
    int         index_count;
    int         vertex_size;
    GLenum      index_format;
    VertexType  type;
} Mesh;

/** Vertex Descriptions 
 */
typedef struct VertexDescription
{
    AttributeSlot   slot;
    int             count;
} VertexDescription;

static const VertexDescription kVertexDescriptions[kNUM_VERTEX_TYPES][16] =
{
    { /* kPosNormTexVertex */
        { kPositionSlot,  3, },
        { kNormalSlot,    3, },
        { kTexCoordSlot,  2, },
        { kEmptySlot, 0 }
    },
    { /* kPosColorVertex */
        { kPositionSlot,  3, },
        { kColorSlot,     4, },
        { kEmptySlot, 0 }
    },
    { /* kPosTexVertex */
        { kPositionSlot,  3, },
        { kTexCoordSlot,  2, },
        { kEmptySlot, 0 }
    }
};

/** Functions
 */
Mesh gl_create_mesh(const void* vertex_data, size_t vertex_data_size,
                    const void* index_data, size_t index_data_size,
                    int index_count, int vertex_size, VertexType type);
Mesh gl_load_mesh(const char* filename);


/* triangle vertices
 *
 *      0
 *     / \
 *    /   \
 *   /     \
 *  1-------2
 */
static const PosNormTexVertex kTriangleVertices[] =
{
    /*  position                 normal               texcoords */
    { {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },  /* 0 */
    { { -1.0f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  /* 1 */
    { {  1.0f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },  /* 2 */
};
static const uint16_t kTriangleIndices[] =
{
    0, 1, 2
};

/* quad vertices
 *
 *  1-------0
 *  |       |
 *  |       |
 *  |       |
 *  2-------3
 *
 */
static const PosNormTexVertex kQuadVertices[] =
{
    { {  1.0f,  1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    { { -1.0f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
};
static const uint16_t kQuadIndices[] =
{
    0, 1, 2,
    0, 2, 3,
};

/* cube vertices
 *
 *               9---------8
 *              /         /
 *             /         /
 *            10-------11
 *
 *    13         20-------21         4       front: {  0,  1,  2 }, {  0,  2,  3 }
 *    /|         |         |        /|       right: {  4,  5,  6 }, {  4,  6,  7 }
 *   / |         |         |       / |         top: {  8,  9, 10 }, {  8, 10, 11 }
 *  12 |      1---------0  |      5  |        left: { 12, 13, 14 }, { 12, 14, 15 }
 *  |  |      |  |      |  |      |  |      bottom: { 16, 17, 18 }, { 16, 18, 19 }
 *  | 14      |  23-----|-22      |  7        back: { 20, 21, 22 }, { 20, 22, 23 }
 *  | /       |         |         | /
 *  |/        |         |         |/
 *  15        2---------3         6
 *
 *               18-------19
 *              /         /
 *             /         /
 *            17-------16
 *
 */
static const PosNormTexVertex kCubeVertices[] =
{
    /* FRONT */
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },  /* 0 */
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },  /* 1 */
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  /* 2 */
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },  /* 3 */

    /* RIGHT */
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },  /* 4 */
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },  /* 5 */
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },  /* 6 */
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },  /* 7 */

    /* TOP */
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },  /* 8 */
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },  /* 9 */
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },  /* 10 */
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },  /* 11 */

    /* LEFT */
    { { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },  /* 12 */
    { { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },  /* 13 */
    { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },  /* 14 */
    { { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },  /* 15 */

    /* BOTTOM */
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },  /* 16 */
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },  /* 17 */
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },  /* 18 */
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },  /* 19 */

    /* BACK */
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },  /* 20 */
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },  /* 21 */
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },  /* 22 */
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },  /* 23 */
};

static const uint16_t kCubeIndices[] =
{
     0,  1,  2,   0,  2,  3,  /* front */
     4,  5,  6,   4,  6,  7,  /* right */
     8,  9, 10,   8, 10, 11,  /* top */
    12, 13, 14,  12, 14, 15,  /* left */
    16, 17, 18,  16, 18, 19,  /* bottom */
    20, 21, 22,  20, 22, 23,  /* back */
};


#endif /* include guard */
