/*! @file mesh.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "mesh.h"
#include <stdlib.h>
#include "gl_include.h"

/* Defines
 */

/* Types
 */
struct Mesh
{
    GLuint      vertex_buffer;
    GLuint      index_buffer;
    int         index_count;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */
Mesh* create_mesh(const Vertex* vertex_data, size_t vertex_data_size,
                  const uint32_t* index_data, size_t index_data_size,
                  int index_count)
{
    Mesh*   mesh = NULL;
    GLuint  vertex_buffer = 0;
    GLuint  index_buffer = 0;

    /* Create vertex buffer */
    ASSERT_GL(glGenBuffers(1, &vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
    ASSERT_GL(glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    /* Create index buffer */
    ASSERT_GL(glGenBuffers(1, &vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));
    ASSERT_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_STATIC_DRAW));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Create mesh */
    mesh = (Mesh*)calloc(1, sizeof(Mesh));
    mesh->vertex_buffer = vertex_buffer;
    mesh->index_buffer = index_buffer;
    mesh->index_count = index_count;

    return mesh;
}
void draw_mesh(const Mesh* M)
{
    float* ptr = 0;
    ASSERT_GL(glBindBuffer(GL_ARRAY_BUFFER, M->vertex_buffer));
    ASSERT_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, M->index_buffer));
    ASSERT_GL(glVertexAttribPointer(kPositionSlot,    3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=0)));
    ASSERT_GL(glVertexAttribPointer(kNormalSlot,      3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3)));
    ASSERT_GL(glVertexAttribPointer(kTangentSlot,     3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3)));
    ASSERT_GL(glVertexAttribPointer(kBitangentSlot,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3)));
    ASSERT_GL(glVertexAttribPointer(kTexCoordSlot,    2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3)));
    ASSERT_GL(glDrawElements(GL_TRIANGLES, M->index_count, GL_UNSIGNED_INT, NULL));
}
void _destroy_mesh(Mesh* M)
{
    glDeleteBuffers(1,&M->vertex_buffer);
    glDeleteBuffers(1,&M->index_buffer);
    free(M);
}
