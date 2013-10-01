/*! @file scene.h
 *  @brief TODO: Enter description of the file
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __scene_h__
#define __scene_h__

typedef struct Scene Scene;

Scene* create_scene(const char* filename);
void destroy_scene(Scene* S);

#endif /* include guard */
