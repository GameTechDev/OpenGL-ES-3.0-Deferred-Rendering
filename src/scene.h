#ifndef __scene_h__
#define __scene_h__

#include "texture.h"
#include "vec_math.h"
#include "graphics_types.h"

typedef struct Scene Scene;
typedef struct SceneData SceneData;
typedef struct Material
{
    char    name[64];
    Texture albedo;
    Texture normal;
    Vec3    specular_color;
    float   specular_power;
    float   specular_coefficient;
} Material;
typedef struct Model
{
    char        name[64];
    Transform   transform;
    Mesh*       mesh;
    Material*   material;
} Model;

Scene* create_scene(const char* filename);
void destroy_scene(Scene* S);
void render_scene(Scene* S, Graphics* G);

Model* get_model(Scene* S, int model);

SceneData* _load_scene_data(const char* filename);
void _free_scene_data(SceneData* S);

#endif /* include guard */
