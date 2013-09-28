/** @file geometry.cpp
 *  @copyright  Copyright (c) 2013 Intel. All rights reserved.
 */
extern "C" {
#include "geometry.h"
#include "system.h"
#include "gl_helper.h"
#include "assert.h"
#include "vec_math.h"
#include "graphics.h"
}
#include <vector>
#include <string>
#include <stdlib.h>
#include <map>

/* Constants
 */

/* Variables
 */

/* Internal functions
 */
PosNormTanBitanTexVertex* calculate_tangets(const PosNormTexVertex* vertices, int num_vertices, const void* indices, size_t index_size, int num_indices)
{
    PosNormTanBitanTexVertex* new_vertices = new PosNormTanBitanTexVertex[num_vertices];
    for(int ii=0;ii<num_vertices;++ii) {
        new_vertices[ii].position = vertices[ii].position;
        new_vertices[ii].normal = vertices[ii].normal;
        new_vertices[ii].tex = vertices[ii].tex;
    }
    for(int ii=0;ii<num_indices;ii+=3) {
        uint32_t i0,i1,i2;
        if(index_size == 2) {
            i0 = ((uint16_t*)indices)[ii+0];
            i1 = ((uint16_t*)indices)[ii+1];
            i2 = ((uint16_t*)indices)[ii+2];
        } else {
            i0 = ((uint32_t*)indices)[ii+0];
            i1 = ((uint32_t*)indices)[ii+1];
            i2 = ((uint32_t*)indices)[ii+2];
        }

        PosNormTanBitanTexVertex& v0 = new_vertices[i0];
        PosNormTanBitanTexVertex& v1 = new_vertices[i1];
        PosNormTanBitanTexVertex& v2 = new_vertices[i2];

        Vec3 delta_pos1 = vec3_sub(v1.position, v0.position);
        Vec3 delta_pos2 = vec3_sub(v2.position, v0.position);
        Vec2 delta_uv1 = vec2_sub(v1.tex, v0.tex);
        Vec2 delta_uv2 = vec2_sub(v2.tex, v0.tex);

        float r = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
        Vec3 a = vec3_mul_scalar(delta_pos1, delta_uv2.y);
        Vec3 b = vec3_mul_scalar(delta_pos2, delta_uv1.y);
        Vec3 tangent = vec3_sub(a,b);
        tangent = vec3_mul_scalar(tangent, r);

        a = vec3_mul_scalar(delta_pos2, delta_uv1.x);
        b = vec3_mul_scalar(delta_pos1, delta_uv2.x);
        Vec3 bitangent = vec3_sub(a,b);
        bitangent = vec3_mul_scalar(bitangent, r);

        v0.bitangent = bitangent;
        v1.bitangent = bitangent;
        v2.bitangent = bitangent;

        v0.tangent = tangent;
        v1.tangent = tangent;
        v2.tangent = tangent;
    }
    return new_vertices;
}

static const char* get_line_from_buffer(char* line, size_t line_size, const char* buffer)
{
    const char* line_end = line + line_size;

    if(buffer == NULL || line == NULL || line_size == 0)
        return NULL;

    while(line != line_end) {
        char current_char = *buffer;
        switch(current_char) {
            case '\0':
                *line = '\0';
                return NULL;
            case '\n':
                *line = '\0';
                return buffer+1;
            case '\r':
                *line = '\0';
                buffer++;
                if(*buffer == '\n') {
                    return ++buffer;
                }
                return buffer;
            default:
                *line = current_char;
                ++line;
                ++buffer;
                break;
        }
    }

    return buffer;
}
static void _load_mtl_file(Graphics* graphics, const char* filename, std::map<std::string, Material>& materials)
{
    char* file_data = NULL;
    char* original_data = NULL;
    size_t file_size = 0;

    load_file_data(filename, (void**)&file_data, &file_size);
    original_data = file_data;

    std::string current_name;

    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[64] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "newmtl") == 0) {
            Material blank_material = {0};
            blank_material.specular_power = 16.0f;
            char name[128] = {0};
            assert(sscanf(line, "%s %s\n", line_header, name) == 2);
            current_name = name;
            materials[name] = blank_material;
            strncpy(materials[name].name, name, sizeof(materials[name].name));
        } else if(strcmp(line_header, "map_Kd") == 0) {
            char texture[128] = {0};
            assert(sscanf(line, "%s %s\n", line_header, texture) == 2);
            materials[current_name].albedo_tex = load_texture(graphics, texture);
        } else if((strcmp(line_header, "map_bump") == 0 || strcmp(line_header, "map_bump") == 0) && materials[current_name].normal_tex == NULL) {
            char texture[128] = {0};
            assert(sscanf(line, "%s %s\n", line_header, texture) == 2);
            materials[current_name].normal_tex = load_texture(graphics, texture);
        } else if(strcmp(line_header, "Ks") == 0) {
            Vec3 spec_color;
            assert(sscanf(line, "%s %f %f %f\n", line_header, &spec_color.x, &spec_color.y, &spec_color.z) == 4);
            materials[current_name].specular_color = spec_color;
        } else if(strcmp(line_header, "Ns") == 0) {
            Vec3 spec_color;
            assert(sscanf(line, "%s %f\n", line_header, &materials[current_name].specular_coefficient) == 2);
        }
    }
}

/* External functions
 */
Mesh* gl_create_mesh(const void* vertex_data, size_t vertex_data_size,
                     const void* index_data, size_t index_data_size,
                     int index_count, int vertex_size, VertexType type)
{
    Mesh* mesh = (Mesh*)calloc(1, sizeof(*mesh));
    mesh->vertex_buffer   = gl_create_buffer(GL_ARRAY_BUFFER, vertex_data, vertex_data_size);
    mesh->index_buffer    = gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_data, index_data_size);
    mesh->index_count     = index_count;
    mesh->vertex_size     = vertex_size;
    mesh->index_format    = (index_data_size/index_count == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    mesh->type            = type;
    return mesh;
}
struct int3 {
    int p;
    int t;
    int n;

    bool operator==(const int3 rh) const
    {
        return p == rh.p && t == rh.t && n == rh.n;
    }
    bool operator<(const int3 rh) const
    {
        if(p < rh.p) return true;
        if(p > rh.p) return false;
        
        if(t < rh.t) return true;
        if(t > rh.t) return false;
        
        if(n < rh.n) return true;
        if(n > rh.n) return false;

        return false;
    }
};
/* This is pretty ugly, but functional code. There are probably more efficient
    ways of loading obj's.
 */
Mesh* gl_load_mesh(Graphics* graphics, const char* filename)
{
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;

    std::vector<int3>   indices;

    int textured = 0;

    Vec2 tex = {0.5f, 0.5f};
    texcoords.push_back(tex);

    char* file_data = NULL;
    char* original_data = NULL;
    size_t file_size = 0;

    load_file_data(filename, (void**)&file_data, &file_size);
    original_data = file_data;

    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[64] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "v") == 0) {
            Vec3 v;
            assert(sscanf(line, "%s %f %f %f\n", line_header, &v.x, &v.y, &v.z) == 4);
            positions.push_back(v);
            textured = 0;
        } else if(strcmp(line_header, "vt") == 0) {
            Vec2 t;
            assert(sscanf(line, "%s %f %f\n", line_header, &t.x, &t.y) == 3);
            texcoords.push_back(t);
            textured = 1;
        } else if(strcmp(line_header, "vn") == 0) {
            Vec3 n;
            assert(sscanf(line, "%s %f %f %f\n", line_header, &n.x, &n.y, &n.z) == 4);
            normals.push_back(n);
        } else if(strcmp(line_header, "f") == 0) {
            int3 triangle[4];
            int matches;
            if(textured) {
                matches = sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].t, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].t, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].t, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].t, &triangle[3].n);
                if(matches != 10 && matches != 13) {
                    system_log("Can't load this OBJ\n");
                    free_file_data(original_data);
                    return NULL;
                }
            } else {
                matches = sscanf(line, "%s %d//%d %d//%d %d//%d %d//%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].n);
                if(matches != 7 && matches != 9) {
                    system_log("Can't load this OBJ\n");
                    free_file_data(original_data);
                    return NULL;
                }
                triangle[0].t = 0;
                triangle[1].t = 0;
                triangle[2].t = 0;
                if(matches == 9) {
                    triangle[3].t = 0;
                    matches = 13;
                }
            }
            indices.push_back(triangle[0]);
            indices.push_back(triangle[1]);
            indices.push_back(triangle[2]);
            if(matches == 13) {
                indices.push_back(triangle[0]);
                indices.push_back(triangle[2]);
                indices.push_back(triangle[3]);
            }
        }
    }
    free_file_data(original_data);

    PosNormTexVertex* vertices = new PosNormTexVertex[indices.size()];
    int num_indices = (int)indices.size();
    for(int ii=0; ii<num_indices; ++ii) {
        int pos_index = indices[ii].p-1;
        int tex_index = indices[ii].t;
        int norm_index = indices[ii].n-1;
        PosNormTexVertex& vertex = vertices[ii];
        vertex.position = positions[pos_index];
        vertex.tex = texcoords[tex_index];
        vertex.normal = normals[norm_index];
        /* Flip v-channel */
        vertex.tex.y = 1.0f-vertex.tex.y;
    }
    uint32_t* new_indices = new uint32_t[num_indices];
    for(int ii=0;ii<num_indices;++ii)
        new_indices[ii] = ii;

    int vertex_count = num_indices;
    int index_count = vertex_count;

    PosNormTanBitanTexVertex* new_vertices = calculate_tangets(vertices, vertex_count, new_indices, sizeof(uint32_t), index_count);

    Mesh* mesh = gl_create_mesh(new_vertices, vertex_count*sizeof(PosNormTanBitanTexVertex),
                                new_indices, index_count*sizeof(uint32_t),
                                index_count, sizeof(PosNormTanBitanTexVertex),
                                kPosNormTanBitanTexVertex);

    delete [] new_vertices;
    delete [] vertices;
    delete [] new_indices;


    return mesh;
    (void)sizeof(graphics);
}
void gl_load_obj(Graphics* graphics, const char* filename,
                 Mesh*** meshes, int* num_meshes,
                 Material** materials, int* num_materials)
{
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;

    std::vector<std::vector<int3> >  all_indices;
    std::vector<std::string>  mesh_material_pairs;
    int current_indices = -1;

    std::map<std::string, Material> all_materials;

    int textured = 0;

    Vec2 tex = {0.5f, 0.5f};
    texcoords.push_back(tex);

    char* file_data = NULL;
    char* original_data = NULL;
    size_t file_size = 0;

    load_file_data(filename, (void**)&file_data, &file_size);
    original_data = file_data;

    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[16] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "v") == 0) {
            Vec3 v;
            assert(sscanf(line, "%s %f %f %f\n", line_header, &v.x, &v.y, &v.z) == 4);
            positions.push_back(v);
            textured = 0;
        } else if(strcmp(line_header, "vt") == 0) {
            Vec2 t;
            assert(sscanf(line, "%s %f %f\n", line_header, &t.x, &t.y) == 3);
            texcoords.push_back(t);
            textured = 1;
        } else if(strcmp(line_header, "vn") == 0) {
            Vec3 n;
            assert(sscanf(line, "%s %f %f %f\n", line_header, &n.x, &n.y, &n.z) == 4);
            normals.push_back(n);
        } else if(strcmp(line_header, "usemtl") == 0) {
            char name[256];
            assert(sscanf(line, "%s %s\n", line_header, name) == 2);
            all_indices.push_back(std::vector<int3>());
            mesh_material_pairs.push_back(name);
            current_indices++;
        } else if(strcmp(line_header, "mtllib") == 0) {
            char mtl_filename[256];
            assert(sscanf(line, "%s %s\n", line_header, mtl_filename) == 2);
            _load_mtl_file(graphics, mtl_filename, all_materials);
        } else if(strcmp(line_header, "f") == 0) {
            int3 triangle[4];
            int matches;
            if(textured) {
                matches = sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].t, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].t, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].t, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].t, &triangle[3].n);
                if(matches != 10 && matches != 13) {
                    system_log("Can't load this OBJ\n");
                    free_file_data(original_data);
                }
            } else {
                matches = sscanf(line, "%s %d//%d %d//%d %d//%d %d//%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].n);
                if(matches != 7 && matches != 9) {
                    system_log("Can't load this OBJ\n");
                    free_file_data(original_data);
                }
                triangle[0].t = 0;
                triangle[1].t = 0;
                triangle[2].t = 0;
                if(matches == 9) {
                    triangle[3].t = 0;
                    matches = 13;
                }
            }
            std::vector<int3>& indices = all_indices[current_indices];

            indices.push_back(triangle[0]);
            indices.push_back(triangle[1]);
            indices.push_back(triangle[2]);
            if(matches == 13) {
                indices.push_back(triangle[0]);
                indices.push_back(triangle[2]);
                indices.push_back(triangle[3]);
            }
        }
    }
    free_file_data(original_data);

    std::vector<Mesh*> all_meshes;
    for(int jj=0; jj<(int)all_indices.size();++jj) {
        std::vector<int3>& indices = all_indices[jj];

        std::map<int3, int> m;
        std::vector<PosNormTexVertex> v;
        std::vector<uint32_t> i;
        int num_indices = (int)indices.size();
        for(int ii=0;ii<num_indices;++ii) {
            int3 index = indices[ii];
            std::map<int3, int>::iterator iter = m.find(index);
            if(iter != m.end()) {
                /* Already exists */
                i.push_back(iter->second);
            } else {
                /* Add it */
                int pos_index = indices[ii].p-1;
                int tex_index = indices[ii].t;
                int norm_index = indices[ii].n-1;
                PosNormTexVertex vertex;
                vertex.position = positions[pos_index];
                vertex.tex = texcoords[tex_index];
                vertex.normal = normals[norm_index];
                /* Flip v-channel */
                vertex.tex.y = 1.0f-vertex.tex.y;

                i.push_back(v.size());
                m[index] = v.size();
                v.push_back(vertex);
            }
        }
        int vertex_count = (int)v.size();
        int index_count = i.size();
        PosNormTanBitanTexVertex* new_vertices = calculate_tangets(v.data(), vertex_count, i.data(), sizeof(uint32_t), index_count);

        Mesh* mesh = gl_create_mesh(new_vertices, vertex_count*sizeof(PosNormTanBitanTexVertex),
                                    i.data(), index_count*sizeof(uint32_t),
                                    index_count, sizeof(PosNormTanBitanTexVertex),
                                    kPosNormTanBitanTexVertex);

        delete [] new_vertices;

        all_meshes.push_back(mesh);
    }
    *meshes = (Mesh**)calloc(all_meshes.size(),sizeof(Mesh*));
    for(int ii=0;ii<(int)all_meshes.size(); ++ii) {
        (*meshes)[ii] = all_meshes[ii];
    }
    *num_meshes = (int)all_meshes.size();

    *materials = (Material*)calloc(all_materials.size(),sizeof(Material));
    for(int ii=0;ii<(int)mesh_material_pairs.size();++ii) {
        (*materials)[ii] = all_materials[mesh_material_pairs[ii]];
    }
    *num_materials = (int)all_materials.size();
    
    (void)sizeof(graphics);
}
