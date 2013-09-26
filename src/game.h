/*! @file game.h
 *  @brief Main game code
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __game_h__
#define __game_h__

#include <stdint.h>

typedef struct Game Game;

typedef struct TouchPoint
{
    intptr_t index;
    float x;
    float y;
} TouchPoint;

Game* create_game(int width, int height);
void destroy_game(Game* game);

void update_game(Game* game);
void render_game(Game* game);

void add_touch_points(Game* game, int num_touch_points, TouchPoint* points);
void update_touch_points(Game* game, int num_touch_points, TouchPoint* points);
void remove_touch_points(Game* game, int num_touch_points, TouchPoint* points);

#endif /* include guard */
