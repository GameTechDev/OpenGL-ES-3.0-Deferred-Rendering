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
    ASSERT_GL(glGenBuffers(1, &index_buffer));
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
void destroy_mesh(Mesh* M)
{
    ASSERT_GL(glDeleteBuffers(1,&M->vertex_buffer));
    ASSERT_GL(glDeleteBuffers(1,&M->index_buffer));
    free(M);
}
