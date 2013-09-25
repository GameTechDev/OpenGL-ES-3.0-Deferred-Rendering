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

    TextureID   color_tex;
    TextureID   grass_tex;

    Transform   camera;
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

    game->color_tex = load_texture(game->graphics, "texture.png");
    game->grass_tex = load_texture(game->graphics, "grass.jpg"); /* http://www.brusheezy.com/textures/20185-seamless-green-grass-textures */

    game->camera = transform_zero;
    game->camera.position.y = 2.0f;

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
    static float view_move = 0.0f;
    static float view_rotate = 0.0f;

    float delta_time = (float)get_delta_time(game->timer);
    Transform t = {
        quat_from_euler(rotate, rotate*1.01f, rotate*1.03f),
        vec3_create(sinf(rotate), sinf(rotate*1.1f) + 1.0f, 7.0f),
        1.0f };
    rotate += delta_time;

    view_move += delta_time*1.1;
    view_rotate += delta_time*0.5f;

    game->camera.position.z = sinf(view_move)*5;
    game->camera.orientation = quat_from_euler(sinf(view_rotate*2.0f)*0.5f, sinf(view_rotate), 0.0f);


    add_render_command(game->graphics, cube_mesh(game->graphics), game->color_tex, t);

    t.orientation = quat_from_euler(-kPiDiv2, 0.0f, 0.0f);
    t.position = vec3_create(0.0f, 0.0f, 0.0f);
    t.scale = 50.0f;

    add_render_command(game->graphics, quad_mesh(game->graphics), game->grass_tex, t);

    set_view_transform(game->graphics, game->camera);
    game->camera = transform_zero;
    game->camera.position.y = 2.0f;
    set_view_transform(game->graphics, game->camera);
}
void render_game(Game* game)
{
    render_graphics(game->graphics);
}
