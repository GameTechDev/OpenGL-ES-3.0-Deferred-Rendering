/*! @file vertex.h
 *  @brief Needed ver manipulation of vertices
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __vertex_h__
#define __vertex_h__

#include "vec_math.h"

typedef struct Vertex
{
    Vec3    position;
    Vec3    normal;
    Vec3    tangent;
    Vec3    bitangent;
    Vec2    texcoord;
} Vertex;

typedef enum AttributeSlot
{
    kPositionSlot   = 0,
    kNormalSlot,
    kTangentSlot,
    kBitangentSlot,
    kTexCoordSlot,

    kEmptySlot = -1
} AttributeSlot;


#endif /* include guard */
