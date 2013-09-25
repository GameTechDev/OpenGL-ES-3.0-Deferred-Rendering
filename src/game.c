/*! @file game.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "game.h"
#include <stdlib.h>
#include "system.h"
#include "timer.h"
#include "graphics.h"
#include "vec_math.h"

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
    static float rotate = 0.0f;
    float delta_time = (float)get_delta_time(game->timer);
    Transform t = {
        quat_from_euler(rotate, rotate*1.01f, rotate*1.03f),
        vec3_create(sinf(rotate), sinf(rotate*1.1f), 7.0f),
        1.0f };
    rotate += delta_time;

    add_render_command(game->graphics, cube_mesh(game->graphics), t);
}
void render_game(Game* game)
{
    render_graphics(game->graphics);
}
