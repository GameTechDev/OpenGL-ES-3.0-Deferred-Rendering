/*! @file game.c
 *  @copyright Copyright (c) 2013 Kyle Weicht. All rights reserved.
 */
#include "game.h"
#include <stdlib.h>
#include "system.h"
#include "timer.h"
#include "graphics.h"
#include "vec_math.h"
#include "scene.h"

/* Defines
 */

/* Types
 */
struct Game
{
    /* Engine objects */
    Timer*      timer;
    Graphics*   graphics;

    /* Game objects */
    Transform   camera;
    Scene*      scene;

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
static void _control_camera(Game* G, float delta_time)
{
    if(G->num_points == 1) {
        Vec2 curr = G->points[0].pos;
        Vec2 delta = vec2_sub(curr, G->prev_single);

        /* L-R rotation */
        Quaternion q = quat_from_axis_anglef(0, 1, 0, delta_time*delta.x*0.2f);
        G->camera.orientation = quat_multiply(G->camera.orientation, q);

        /* U-D rotation */
        q = quat_from_axis_anglef(1, 0, 0, delta_time*delta.y*0.2f);
        G->camera.orientation = quat_multiply(q, G->camera.orientation);

        G->prev_single = curr;
    } else if(G->num_points == 2) {
        float camera_speed = 0.1f;
        Vec3 look = quat_get_z_axis(G->camera.orientation);
        Vec3 right = quat_get_x_axis(G->camera.orientation);
        Vec2 avg = vec2_add(G->points[0].pos, G->points[1].pos);
        Vec2 delta;

        avg = vec2_mul_scalar(avg, 0.5f);
        delta = vec2_sub(avg, G->prev_double);

        look = vec3_mul_scalar(look, -delta.y*camera_speed);
        right = vec3_mul_scalar(right, delta.x*camera_speed);


        G->camera.position = vec3_add(G->camera.position, look);
        G->camera.position = vec3_add(G->camera.position, right);

        G->prev_double = avg;
    }
}

/* External functions
 */
Game* create_game(void)
{
    Game* G = (Game*)calloc(1, sizeof(Game));
    G->timer = create_timer();
    G->graphics = create_graphics();

    /* Set up camera */
    G->camera = transform_zero;
    G->camera.orientation = quat_from_euler(0, -0.75f * kPi, 0);
    G->camera.position.x = 4.0f;
    G->camera.position.y = 2;
    G->camera.position.z = 7.5f;

    /* Load scene */
    {
        reset_timer(G->timer);
        create_scene("lightHouse.obj");
        system_log("Loading time: %f\n", get_delta_time(G->timer));
    }

    reset_timer(G->timer);
    return G;
}
void destroy_game(Game* G)
{
    destroy_timer(G->timer);
    destroy_graphics(G->graphics);
    free(G);
}
void resize_game(Game* G, int width, int height)
{
    resize_graphics(G->graphics, width, height);
}
void update_game(Game* G)
{
    float delta_time = (float)get_delta_time(G->timer);

    _control_camera(G, delta_time);

    /* Calculate FPS */
    G->fps_time += delta_time;
    G->fps_count++;
    
    if(G->fps_time >= 1.0f) {
        system_log("FPS: %f\n", G->fps_count/G->fps_time);
        G->fps_time -= 1.0f;
        G->fps_count = 0;
    }
}
void render_game(Game* G)
{
    render_graphics(G->graphics);
}
void add_touch_points(Game* G, int num_touch_points, TouchPoint* points)
{
    int ii;
    for(ii=0;ii<num_touch_points;++ii) {
        G->points[G->num_points++] = points[ii];
    }

    if(G->num_points == 1) {
        G->prev_single = G->points[0].pos;
    } else if(G->num_points == 2) {
        Vec2 avg = vec2_add(G->points[0].pos, G->points[1].pos);
        avg = vec2_mul_scalar(avg, 0.5f);
        G->prev_double = avg;
    }
}
void update_touch_points(Game* G, int num_touch_points, TouchPoint* points)
{
    int ii, jj;
    for(ii=0;ii<G->num_points;++ii) {
        for(jj=0;jj<num_touch_points;++jj) {
            if(G->points[ii].index == points[jj].index) {
                G->points[ii] = points[jj];
                break;
            }
        }
    }
}
void remove_touch_points(Game* G, int num_touch_points, TouchPoint* points)
{
    int orig_num_points = G->num_points;
    int ii, jj;
    for(ii=0;ii<orig_num_points;++ii) {
        for(jj=0;jj<num_touch_points;++jj) {
            if(G->points[ii].index == points[jj].index) {
                /* This is the removed touch, swap it with the end of the list */
                G->points[ii] = G->points[--G->num_points];
                break;
            }
        }
    }

    if(G->num_points == 1) {
        G->prev_single = G->points[0].pos;
    } else if(G->num_points == 2) {
        Vec2 avg = vec2_add(G->points[0].pos, G->points[1].pos);
        avg = vec2_mul_scalar(avg, 0.5f);
        G->prev_double = avg;
    }
}

