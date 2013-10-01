/*! @file game.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "game.h"
#include <stdlib.h>
#include "system.h"
#include "timer.h"
#include "vec_math.h"

/* Defines
 */

/* Types
 */
struct Game
{
    /* Engine objects */
    Timer*      timer;

    /* Game objects */
    Transform   camera;

    /* Input */
    TouchPoint  points[16];
    int         num_points;
    Vec2        prev_single;
    Vec2        prev_double;

    /* FPS Counting */
    float       fps_time;
    int         fps_count;
};

/* Constants
 */

/* Variables
 */

/* Internal functions
 */
static void _control_camera(Game* game, float delta_time)
{
    if(game->num_points == 1) {
        Vec2 curr = game->points[0].pos;
        Vec2 delta = vec2_sub(curr, game->prev_single);

        /* L-R rotation */
        Quaternion q = quat_from_axis_anglef(0, 1, 0, delta_time*delta.x*0.2f);
        game->camera.orientation = quat_multiply(game->camera.orientation, q);

        /* U-D rotation */
        q = quat_from_axis_anglef(1, 0, 0, delta_time*delta.y*0.2f);
        game->camera.orientation = quat_multiply(q, game->camera.orientation);

        game->prev_single = curr;
    } else if(game->num_points == 2) {
        float camera_speed = 0.1f;
        Vec3 look = quat_get_z_axis(game->camera.orientation);
        Vec3 right = quat_get_x_axis(game->camera.orientation);
        Vec2 avg = vec2_add(game->points[0].pos, game->points[1].pos);
        Vec2 delta;

        avg = vec2_mul_scalar(avg, 0.5f);
        delta = vec2_sub(avg, game->prev_double);

        look = vec3_mul_scalar(look, -delta.y*camera_speed);
        right = vec3_mul_scalar(right, delta.x*camera_speed);


        game->camera.position = vec3_add(game->camera.position, look);
        game->camera.position = vec3_add(game->camera.position, right);

        game->prev_double = avg;
    }
}

/* External functions
 */
Game* create_game(int width, int height)
{
    Game* game = (Game*)calloc(1, sizeof(*game));
    game->timer = create_timer();

    /* Set up camera */
    game->camera = transform_zero;
    game->camera.orientation = quat_from_euler(0, kPi*-0.75f, 0);
    game->camera.position.x = 4.0f;
    game->camera.position.y = 2;
    game->camera.position.z = 7.5f;

    reset_timer(game->timer);
    return game;
}
void destroy_game(Game* game)
{
    destroy_timer(game->timer);
    free(game);
}
void resize_game(Game* game, int width, int height)
{
}
void update_game(Game* game)
{
    float delta_time = (float)get_delta_time(game->timer);

    _control_camera(game, delta_time);

    /* Calculate FPS */
    game->fps_time += delta_time;
    game->fps_count++;
    
    if(game->fps_time >= 1.0f) {
        system_log("FPS: %f\n", game->fps_count/game->fps_time);
        game->fps_time -= 1.0f;
        game->fps_count = 0;
    }
}
void render_game(Game* game)
{
}
void add_touch_points(Game* game, int num_touch_points, TouchPoint* points)
{
    int ii;
    for(ii=0;ii<num_touch_points;++ii) {
        game->points[game->num_points++] = points[ii];
    }

    if(game->num_points == 1) {
        game->prev_single = game->points[0].pos;
    } else if(game->num_points == 2) {
        Vec2 avg = vec2_add(game->points[0].pos, game->points[1].pos);
        avg = vec2_mul_scalar(avg, 0.5f);
        game->prev_double = avg;
    }
}
void update_touch_points(Game* game, int num_touch_points, TouchPoint* points)
{
    int ii, jj;
    for(ii=0;ii<game->num_points;++ii) {
        for(jj=0;jj<num_touch_points;++jj) {
            if(game->points[ii].index == points[jj].index) {
                game->points[ii] = points[jj];
                break;
            }
        }
    }
}
void remove_touch_points(Game* game, int num_touch_points, TouchPoint* points)
{
    int orig_num_points = game->num_points;
    int ii, jj;
    for(ii=0;ii<orig_num_points;++ii) {
        for(jj=0;jj<num_touch_points;++jj) {
            if(game->points[ii].index == points[jj].index) {
                /* This is the removed touch, swap it with the end of the list */
                game->points[ii] = game->points[--game->num_points];
                break;
            }
        }
    }

    if(game->num_points == 1) {
        game->prev_single = game->points[0].pos;
    } else if(game->num_points == 2) {
        Vec2 avg = vec2_add(game->points[0].pos, game->points[1].pos);
        avg = vec2_mul_scalar(avg, 0.5f);
        game->prev_double = avg;
    }
}

