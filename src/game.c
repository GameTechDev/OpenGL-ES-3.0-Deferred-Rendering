/*! @file game.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "game.h"
#include <stdlib.h>
#include "system.h"
#include "timer.h"
#include "graphics.h"

/* Defines
 */

/* Types
 */
struct Game
{
    Timer*      timer;
    Graphics*   graphics;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */

/* External functions
 */
Game* create_game(int width, int height)
{
    Game* game = (Game*)calloc(1, sizeof(*game));
    game->graphics = create_graphics(width, height);
    game->timer = create_timer();

    return game;
}
void destroy_game(Game* game)
{
    destroy_timer(game->timer);
    destroy_graphics(game->graphics);
    destroy_game(game);
}
void update_game(Game* game)
{
    static float time = 0.0f;
    float delta_time = (float)get_delta_time(game->timer);
    time += delta_time;
    if(time > 1.0f) {
        system_log("%f\n", get_running_time(game->timer));
        time -= 1.0f;
    }
}
void render_game(Game* game)
{
    render_graphics(game->graphics);
}
