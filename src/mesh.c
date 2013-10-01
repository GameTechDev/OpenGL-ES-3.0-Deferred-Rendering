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
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Create index buffer */
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Create mesh */
    mesh = (Mesh*)calloc(1, sizeof(Mesh));
    mesh->vertex_buffer = vertex_buffer;
    mesh->index_buffer = index_buffer;
    mesh->index_count = index_count;

    return mesh;
}
void draw_mesh(const Mesh* mesh)
{
    intptr_t ptr = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    glVertexAttribPointer(kPositionSlot,    3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=0));
    glVertexAttribPointer(kNormalSlot,      3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3));
    glVertexAttribPointer(kTangentSlot,     3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3));
    glVertexAttribPointer(kBitangentSlot,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3));
    glVertexAttribPointer(kTexCoordSlot,    2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(ptr+=3));
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, NULL);
}
void _destroy_mesh(Mesh* mesh)
{
    glDeleteBuffers(1,&mesh->vertex_buffer);
    glDeleteBuffers(1,&mesh->index_buffer);
    free(mesh);
}
