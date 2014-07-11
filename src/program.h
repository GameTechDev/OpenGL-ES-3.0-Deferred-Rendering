#ifndef __program_h__
#define __program_h__

#include <stdint.h>
#include "vertex.h"

typedef uint32_t Program;

Program create_program(const char* vertex_shader_filename,
                       const char* fragment_shader_filename,
                       const AttributeSlot* slots);
void destroy_program(Program program);

#endif /* include guard */
