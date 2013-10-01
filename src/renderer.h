/*! @file renderer.h
 *  @brief Description of a basic renderer (forward, deferred, etc.)
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __renderer_h__
#define __renderer_h__

#include "graphics.h"

void forward_renderer(Mat4 proj_matrix, Mat4 view_matrix,
                      RenderCommand* commands, int num_commands,
                      Light* lights, int num_lights);

#endif /* include guard */
