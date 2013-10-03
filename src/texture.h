/*! @file texture.h
 *  @brief 2D texture interface
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __texture_h__
#define __texture_h__

typedef unsigned int Texture;

Texture load_texture(const char* filename);
void destroy_texture(Texture T);

#endif /* include guard */
