/*! @file scene.cpp
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
extern "C" {
#include "scene.h"
#include "vertex.h"
#include "mesh.h"
#include "utility.h"
#include "system.h"
#include "assert.h"
#include "graphics.h"
}
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

/* Defines
 */
typedef struct Material Material;

/* Types
 */
struct SimpleVertex
{
    Vec3    position;
    Vec3    normal;
    Vec2    texcoord;
};

/** Mesh data
 */
struct MeshData
{
    char        name[128];
    Vertex*     vertices;
    uint32_t*   indices;
    uint32_t    vertex_count;
    uint32_t    index_count;
};
static void _print_mesh_data(const MeshData* M)
{
    printf("\t%s\n", M->name);
    printf("\tIndex count:\t%d\n", M->index_count);
    printf("\tVertex count:\t%d\n", M->vertex_count);
    printf("\tVertices:\t\t%p\n", M->vertices);
    printf("\tIndices:\t\t%p\n", M->indices);
    printf("\n");
}

/** Material data
 */
struct MaterialData
{
    char        name[128];
    char        albedo_tex[128];
    char        normal_tex[128];
    Vec3        specular_color;
    float       specular_power;
    float       specular_coefficient;
};
static void _print_material_data(const MaterialData* M)
{
    printf("\t%s\n", M->name);
    printf("\tAlbedo:\t\t%s\n", M->albedo_tex);
    printf("\tNormal:\t\t%s\n", M->normal_tex);
    printf("\tSpecular :\t%f\n", M->specular_coefficient);
    printf("\tSpecular power:\t%f\n", M->specular_power);
    printf("\tSpecular color:\t%f\n", M->specular_color.x);
    printf("\n");
}

/** Model data
 */
struct ModelData
{
    char    mesh_name[128];
    char    material_name[128];
};
static void _print_model_data(const ModelData* M)
{
    printf("\tMesh:\t%s\n", M->mesh_name);
    printf("\tMaterial:\t%s\n", M->material_name);
    printf("\n");
}

/** Scene data
 */
struct SceneData
{
    MeshData*       meshes;
    MaterialData*   materials;
    ModelData*      models;
    uint32_t        num_meshes;
    uint32_t        num_materials;
    uint32_t        num_models;
};
static void _print_scene_data(const SceneData* scene)
{
    printf("Num meshes:\t%d\n", scene->num_meshes);
    for(int ii=0; ii<scene->num_meshes;++ii) {
        _print_mesh_data(scene->meshes + ii);
    }
    printf("Num Materials:\t%d\n", scene->num_materials);
    for(int ii=0; ii<scene->num_materials;++ii) {
        _print_material_data(scene->materials + ii);
    }
    printf("Num models:\t%d\n", scene->num_models);
    for(int ii=0; ii<scene->num_models;++ii) {
        _print_model_data(scene->models + ii);
    }
}

struct Scene
{
    Mesh**          meshes;
    Material*       materials;
    Model*          models;
    uint32_t        num_meshes;
    uint32_t        num_materials;
    uint32_t        num_models;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */
static void _load_mtl_file(const char* path, const char* filename, SceneData* scene)
{
    std::string path_string(path);
    char* file_data = NULL;
    char* original_data = NULL;
    size_t file_size = 0;
    int matches;

    load_file_data((path_string+filename).c_str(), (void**)&file_data, &file_size);
    original_data = file_data;


    //
    // Count materials first
    //
    int orig_num_materials = scene->num_materials;
    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[64] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "newmtl") == 0) {
            scene->num_materials++;
        }
    }
    file_data = original_data;

    //
    // Allocate materials
    //
    scene->materials = (MaterialData*)realloc(scene->materials, scene->num_materials*sizeof(MaterialData));
    MaterialData* current_material = (scene->materials - 1) + orig_num_materials;

    //
    // Loop through and create materials
    //
    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[64] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "newmtl") == 0) {
            current_material++;
            memset(current_material, 0, sizeof(*current_material));
            matches = sscanf(line, "%s %s\n", line_header, current_material->name);
            current_material->specular_power = 16.0f;
            assert(matches == 2);
        } else if(strcmp(line_header, "map_Kd") == 0) {
            matches = sscanf(line, "%s %s\n", line_header, current_material->albedo_tex);
            assert(matches == 2);
        } else if((strcmp(line_header, "map_bump") == 0 || strcmp(line_header, "map_bump") == 0) && current_material->normal_tex[0] == '\0') {
            matches = sscanf(line, "%s %s\n", line_header, current_material->normal_tex);
            assert(matches == 2);
        } else if(strcmp(line_header, "Ks") == 0) {
            Vec3 spec_color;
            matches = sscanf(line, "%s %f %f %f\n", line_header, &spec_color.x, &spec_color.y, &spec_color.z);
            assert(matches == 4);
            current_material->specular_color = spec_color;
        } else if(strcmp(line_header, "Ns") == 0) {
            matches = sscanf(line, "%s %f\n", line_header, &current_material->specular_coefficient);
            assert(matches == 2);
        }
    }
    free_file_data(original_data);
}

static Vertex* _calculate_tangets(const SimpleVertex* vertices, uint32_t num_vertices,
                                  const uint32_t* indices, int num_indices)
{
    Vertex* new_vertices = (Vertex*)calloc(sizeof(Vertex),num_vertices);
    for(int ii=0;ii<num_vertices;++ii) {
        new_vertices[ii].position = vertices[ii].position;
        new_vertices[ii].normal = vertices[ii].normal;
        new_vertices[ii].texcoord = vertices[ii].texcoord;
    }
    for(int ii=0;ii<num_indices;ii+=3) {
        uint32_t i0 = indices[ii+0];
        uint32_t i1 = indices[ii+1];
        uint32_t i2 = indices[ii+2];

        Vertex& v0 = new_vertices[i0];
        Vertex& v1 = new_vertices[i1];
        Vertex& v2 = new_vertices[i2];

        Vec3 delta_pos1 = vec3_sub(v1.position, v0.position);
        Vec3 delta_pos2 = vec3_sub(v2.position, v0.position);
        Vec2 delta_uv1 = vec2_sub(v1.texcoord, v0.texcoord);
        Vec2 delta_uv2 = vec2_sub(v2.texcoord, v0.texcoord);

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
struct Triangle
{
    int3    vertex[3];
};
/* This is pretty ugly, but functional code. There are probably more efficient
    ways of loading obj's.
 */
static void _load_obj(const char* path, const char* filename, SceneData* scene)
{
    std::string path_string(path);

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;

    std::vector<std::vector<Triangle> >  all_triangles;
    std::vector<std::string>  mesh_material_pairs;
    std::vector<std::string>  mesh_names;

    int textured = 0;


    char* file_data = NULL;
    char* original_data = NULL;
    size_t file_size = 0;
    int matches;

    load_file_data((path_string+filename).c_str(), (void**)&file_data, &file_size);
    original_data = file_data;

    int orig_num_meshes = scene->num_meshes;
    int orig_num_models = scene->num_models;

    //
    // Count everything
    //
    uint32_t num_total_vertices = 0;
    uint32_t num_total_texcoords = 0;
    uint32_t num_total_normals = 0;
    uint32_t num_meshes = 0;
    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[16] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "v") == 0) {
            num_total_vertices++;
        } else if(strcmp(line_header, "vt") == 0) {
            num_total_texcoords++;
        } else if(strcmp(line_header, "vn") == 0) {
            num_total_normals++;
        } else if(strcmp(line_header, "usemtl") == 0) {
            num_meshes++;
            scene->num_meshes++;
            scene->num_models++;
        } else if(strcmp(line_header, "mtllib") == 0 ) {
            char mtl_filename[256];
            matches = sscanf(line, "%s %s\n", line_header, mtl_filename);
            assert(matches == 2);
            _load_mtl_file(path, mtl_filename, scene);
        }
    }
    file_data = original_data;
    positions.reserve(num_total_vertices);
    normals.reserve(num_total_normals);
    texcoords.reserve(num_total_texcoords+1);
    all_triangles.resize(num_meshes);
    
    Vec2 tex = {0.5f, 0.5f};
    texcoords.push_back(tex);

    scene->meshes = (MeshData*)realloc(scene->meshes, sizeof(MeshData)*scene->num_meshes);
    scene->models = (ModelData*)realloc(scene->models, sizeof(ModelData)*scene->num_models);

    //
    // Fill out vertex data
    //
    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        char line_header[16] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "v") == 0) {
            Vec3 v;
            matches = sscanf(line, "%s %f %f %f\n", line_header, &v.x, &v.y, &v.z);
            assert(matches == 4);
            positions.push_back(v);
            textured = 0;
        } else if(strcmp(line_header, "vt") == 0) {
            Vec2 t;
            matches = sscanf(line, "%s %f %f\n", line_header, &t.x, &t.y);
            assert(matches == 3);
            texcoords.push_back(t);
            textured = 1;
        } else if(strcmp(line_header, "vn") == 0) {
            Vec3 n;
            matches = sscanf(line, "%s %f %f %f\n", line_header, &n.x, &n.y, &n.z);
            assert(matches == 4);
            normals.push_back(n);
        }
    }
    file_data = original_data;

    //
    // Load mesh data
    //
    std::vector<Triangle>* mesh_triangles = &all_triangles[0] - 1;
    MeshData* current_mesh = (scene->meshes - 1) + orig_num_meshes;
    ModelData* current_model = (scene->models - 1) + orig_num_models;

    const char* prev_line = NULL;
    while(1) {
        if(file_data == NULL)
            break;
        char line[1024] = {0};
        const char* this_line = file_data;
        file_data = (char*)get_line_from_buffer(line, sizeof(line), file_data);
        const char* next_line = file_data;

        char line_header[16] = {0};
        sscanf(line, "%s", line_header);

        if(strcmp(line_header, "usemtl") == 0) {
            ++mesh_triangles;
            ++current_mesh;
            ++current_model;

            memset(current_mesh, 0, sizeof(*current_mesh));
            memset(current_model, 0, sizeof(*current_model));
            // Get material name
            char material_name[256];
            matches = sscanf(line, "%s %s\n", line_header, material_name);
            assert(matches == 2);
            // Check to see if this is named (a 'g' on the next or prev line)
            if(prev_line && prev_line[0] == 'g') {
                matches = sscanf(prev_line, "%s %s\n", line_header, current_mesh->name);
                assert(matches == 2);
            } else if(next_line && next_line[0] == 'g') {
                matches = sscanf(next_line, "%s %s\n", line_header, current_mesh->name);
                assert(matches == 2);
            } else {
                std::ostringstream s;
                s << "mesh";
                s << current_mesh - scene->meshes;
                strncpy(current_mesh->name, s.str().c_str(), sizeof(current_mesh->name));
            }
            strncpy(current_model->mesh_name, current_mesh->name, sizeof(current_model->mesh_name));
            strncpy(current_model->material_name, material_name, sizeof(current_model->material_name));

        } else if(strcmp(line_header, "f") == 0) {
            int3 triangle[4];
            if(textured) {
                matches = sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].t, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].t, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].t, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].t, &triangle[3].n);
                if(matches != 10 && matches != 13) {
                    printf("Can't load this OBJ\n");
                    free_file_data(original_data);
                    exit(1);
                }
            } else {
                matches = sscanf(line, "%s %d//%d %d//%d %d//%d %d//%d\n",
                                 line_header,
                                 &triangle[0].p, &triangle[0].n,
                                 &triangle[1].p, &triangle[1].n,
                                 &triangle[2].p, &triangle[2].n,
                                 &triangle[3].p, &triangle[3].n);
                if(matches != 7 && matches != 9) {
                    printf("Can't load this OBJ\n");
                    free_file_data(original_data);
                    exit(1);
                }
                triangle[0].t = 0;
                triangle[1].t = 0;
                triangle[2].t = 0;
                if(matches == 9) {
                    triangle[3].t = 0;
                    matches = 13;
                }
            }
            Triangle tri = {
                triangle[0],
                triangle[1],
                triangle[2],
            };
            mesh_triangles->push_back(tri);
            if(matches == 13) {
                Triangle tri2 = {
                    triangle[0],
                    triangle[2],
                    triangle[3],
                };
                mesh_triangles->push_back(tri2);
            }
        }
        prev_line = this_line;
    }

    //
    // Create meshes
    //
    mesh_triangles = &all_triangles[0];
    current_mesh = scene->meshes + orig_num_meshes;
    current_model = scene->models + orig_num_models;

    for(uint32_t kk=0; kk<num_meshes;++kk) {
        std::map<int3, uint32_t> m;
        std::vector<SimpleVertex> v;
        std::vector<uint32_t> i;
        uint32_t num_triangles = (uint32_t)mesh_triangles->size();
        for(uint32_t jj=0;jj<num_triangles;++jj) {
            const Triangle& triangle = (*mesh_triangles)[jj];
            for(uint32_t ii=0;ii<3;++ii) {
                int3 index = triangle.vertex[ii];
                std::map<int3, uint32_t>::iterator iter = m.find(index);
                if(iter != m.end()) {
                    /* Already exists */
                    i.push_back((uint32_t)iter->second);
                } else {
                    /* Add it */
                    int pos_index = index.p-1;
                    int tex_index = index.t;
                    int norm_index = index.n-1;
                    SimpleVertex vertex;
                    vertex.position = positions[pos_index];
                    vertex.texcoord = texcoords[tex_index];
                    vertex.normal = normals[norm_index];
                    /* Flip v-channel */
                    vertex.texcoord.y = 1.0f-vertex.texcoord.y;

                    i.push_back((uint32_t)v.size());
                    m[index] = (uint32_t)v.size();
                    v.push_back(vertex);
                }
            }
        }

        current_mesh->vertex_count = (uint32_t)v.size();
        current_mesh->index_count = (uint32_t)i.size();
        current_mesh->vertices = _calculate_tangets(&v[0], current_mesh->vertex_count,
                                                   &i[0], current_mesh->index_count );
        current_mesh->indices = (uint32_t*)calloc(sizeof(uint32_t), current_mesh->index_count);
        memcpy(current_mesh->indices, &i[0], current_mesh->index_count*sizeof(uint32_t));

        current_mesh++;
        current_model++;
        mesh_triangles++;
    }
    free_file_data(original_data);
}

static void _scene_from_scenedata(const SceneData* data, Scene* scene)
{
    int ii;

    scene->num_meshes = data->num_meshes;
    scene->num_materials = data->num_materials;
    scene->num_models = data->num_models;

    /* Meshes */
    scene->meshes = (Mesh**)calloc(data->num_meshes, sizeof(Mesh*));
    for(ii=0;ii<data->num_meshes;++ii) {
        scene->meshes[ii] = create_mesh(data->meshes[ii].vertices, data->meshes[ii].vertex_count*sizeof(Vertex),
                                        data->meshes[ii].indices, data->meshes[ii].index_count*sizeof(uint32_t),
                                        data->meshes[ii].index_count);
    }

    /* Materials */
    scene->materials = (Material*)calloc(data->num_materials, sizeof(Material));
    for(ii=0;ii<data->num_materials;++ii) {
        scene->materials[ii].albedo = load_texture(data->materials[ii].albedo_tex);
        scene->materials[ii].normal = load_texture(data->materials[ii].normal_tex);
        scene->materials[ii].specular_color = data->materials[ii].specular_color;
        scene->materials[ii].specular_power = data->materials[ii].specular_power;
        scene->materials[ii].specular_coefficient = data->materials[ii].specular_coefficient;
    }

    /* Models */
    scene->models = (Model*)calloc(data->num_models, sizeof(Model));
    for(ii=0;ii<data->num_models;++ii) {
        const char* this_mesh_name = data->models[ii].mesh_name;
        const char* this_material_name = data->models[ii].material_name;
        Material* mat = NULL;
        Mesh* mesh = NULL;
        for(int jj=0; jj<scene->num_materials; ++jj) {
            if(strcmp(this_material_name, data->materials[jj].name) == 0) {
                mat = scene->materials + jj;
                break;
            }
        }
        for(int jj=0; jj<scene->num_meshes; ++jj) {
            if(strcmp(this_mesh_name, data->meshes[jj].name) == 0) {
                mesh = scene->meshes[ii];
                break;
            }
        }

        scene->models[ii].material = mat;
        scene->models[ii].mesh = mesh;
        scene->models[ii].transform = transform_zero;
    }
}

/* External functions
 */
Scene* create_scene(const char* filename)
{
    Scene* scene;

    /* Allocate scene */
    scene = (Scene*)calloc(1, sizeof(Scene));

    /* Parse file */
    const char* extension = get_extension_from_filename(filename);
    if(extension == NULL) {
        free(scene);
        return NULL;
    } else if(strcmp(extension, "obj") == 0) {
        SceneData* data = _load_scene_data(filename);
        _scene_from_scenedata(data, scene);
        _free_scene_data(data);
    } else if(strcmp(extension, "mesh") == 0) {
    } else if(strcmp(extension, "scene") == 0) {
    }

    return scene;
}
void destroy_scene(Scene* S)
{
    for(int ii=0; ii<S->num_meshes; ++ii)
        destroy_mesh(S->meshes[ii]);
    for(int ii=0; ii<S->num_materials; ++ii) {
        destroy_texture(S->materials[ii].normal);
        destroy_texture(S->materials[ii].albedo);
    }
    free(S->meshes);
    free(S->materials);
    free(S->models);
    free(S);
}
void render_scene(Scene* S, Graphics* G)
{
    int ii;
    for(ii=0;ii<S->num_models;++ii) {
        add_render_command(G, S->models[ii]);
    }
}
SceneData* _load_scene_data(const char* filename)
{
    char path[256] = {0};
    char file[256] = {0};
    split_filename(path, sizeof(path), file, sizeof(file), filename);

    SceneData* data = (SceneData*)calloc(1, sizeof(SceneData));
    _load_obj(path, filename, data);
    //_print_scene_data(data);
    return data;
}
void _free_scene_data(SceneData* S)
{
    for(int ii=0;ii<S->num_meshes;++ii) {
        free(S->meshes[ii].indices);
        free(S->meshes[ii].vertices);
    }
    free(S->meshes);
    free(S->materials);
    free(S->models);
    free(S);
}

