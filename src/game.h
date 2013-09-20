/*! @file game.h
 *  @brief Main game code
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#ifndef __game_h__
#define __game_h__

typedef struct Game Game;

Game* create_game(int width, int height);
void destroy_game(Game* game);

void game_update(Game* game);
void game_render(Game* game);

#endif /* include guard */
