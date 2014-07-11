#ifndef __graphics_types_h__
#define __graphics_types_h__

#include "vec_math.h"

typedef struct Graphics Graphics;
typedef struct Light
{
    Vec3    position;
    Vec3    color;
    float   size;
} Light;

typedef struct Mesh Mesh;

#endif /* include guard */
