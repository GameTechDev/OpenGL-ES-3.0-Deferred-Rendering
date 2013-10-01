/*! @file scene.cpp
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
extern "C" {
#include "scene.h"
#include "vertex.h"
#include "mesh.h"
#include "utility.h"
}
#include <stdlib.h>
#include <string.h>

/* Defines
 */

/* Types
 */
struct Scene
{
    Mesh**  meshes;
    int     num_meshes;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

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
    } else if(strcmp(extension, "mesh") == 0) {
    } else if(strcmp(extension, "scene") == 0) {
    }

    return scene;
}
void destroy_scene(Scene* S)
{
    free(S);
}
