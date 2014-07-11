#ifndef __texture_h__
#define __texture_h__

typedef unsigned int Texture;

Texture load_texture(const char* filename);
void destroy_texture(Texture T);

#endif /* include guard */
