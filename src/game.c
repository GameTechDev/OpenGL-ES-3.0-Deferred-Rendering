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

#include "utility.h"

/* Defines
 */
#define NUM_LIGHTS 4

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
    Light       sun_light;
    Light       lights[NUM_LIGHTS];

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
static float _rand_float()
{
    return rand()/(float)RAND_MAX;
}
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
    int ii;
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
    reset_timer(G->timer);
    G->scene = create_scene("lightHouse.obj");
    G->sun_light.position = vec3_create(-4.0f, 5.0f, 2.0f);
    G->sun_light.color = vec3_create(1, 1, 1);
    G->sun_light.size = 10.0f;

    G->lights[0].color = vec3_create(1, 0, 0);
    G->lights[1].color = vec3_create(1, 1, 0);
    G->lights[2].color = vec3_create(0, 1, 0);
    G->lights[3].color = vec3_create(1, 0, 1);
    G->lights[4].color = vec3_create(0, 0, 1);
    G->lights[5].color = vec3_create(0, 1, 1);

    for(ii=0;ii<NUM_LIGHTS;++ii) {
        G->lights[ii].color = vec3_create(_rand_float(), _rand_float(), _rand_float());
        G->lights[ii].color = vec3_normalize(G->lights[ii].color);
    }

    for(ii=0;ii<NUM_LIGHTS;++ii) {
        float x = (20.0f/NUM_LIGHTS) * ii - 8.0f;
        G->lights[ii].color = vec3_create(_rand_float(), _rand_float(), _rand_float());
        G->lights[ii].color = vec3_normalize(G->lights[ii].color);
        G->lights[ii].position = vec3_create(x, _rand_float()*3, 0.0f);
    }

    get_model(G->scene, 3)->material->specular_color = vec3_create(0.5f, 0.5f, 0.5f);
    get_model(G->scene, 3)->material->specular_coefficient = 1.0f;

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
    set_view_matrix(G->graphics, mat4_inverse(transform_get_matrix(G->camera)));
    add_light(G->graphics, G->sun_light);
    render_scene(G->scene, G->graphics);

    if(1) { /* Lights */
        static float move = 0.0f;
        int ii;
        move += delta_time;
        for(ii=0;ii<NUM_LIGHTS;++ii) {
            G->lights[ii].position.z = sinf(move + ii * 1.0f) * 8.0f - 1.0f;
            G->lights[ii].size = 4.0f;

            add_light(G->graphics, G->lights[ii]);
        }
    }

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

