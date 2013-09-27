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

    Mesh*      house_mesh;
    Mesh*       terrain_mesh;
    Transform   camera;

    TouchPoint  points[16];
    int         num_points;

    Vec2        prev_single;
    Vec2        prev_double;

    Material    house_material;
    Material    grass_material;
    Material    color_material;
    Material    terrain_material;
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
        Vec3 up = quat_get_y_axis(game->camera.orientation);
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
static void _print_touches(Game* game)
{
    int ii;
    system_log("Num points: %d\n", game->num_points);
    for(ii=0;ii<game->num_points;++ii) {
        system_log("\t%d: (%d, %d)\n", game->points[ii].index, (int)game->points[ii].pos.x, (int)game->points[ii].pos.y);
    }
}

/* External functions
 */
Game* create_game(int width, int height)
{
    Game* game = (Game*)calloc(1, sizeof(*game));
    game->graphics = create_graphics(width, height);
    game->timer = create_timer();

    game->camera = transform_zero;
    game->camera.position.y = 20;
    game->camera.position.z = -40.0f;

    game->house_mesh = create_mesh(game->graphics, "house_obj.obj");
    game->terrain_mesh = create_mesh(game->graphics, "lightHouse.obj");

    /** Create house material
     */
    game->house_material.albedo_tex = load_texture(game->graphics, "house_diffuse.png");
    game->house_material.normal_tex = load_texture(game->graphics, "house_normal.png");
    game->house_material.specular_color = vec3_create(1.0f, 1.0f, 1.0f);
    game->house_material.specular_power = 1.5f;
    game->house_material.specular_coefficient = 0.06f;

    /** Grass material
     */
    game->grass_material.albedo_tex = load_texture(game->graphics, "grass.jpg");
    game->grass_material.normal_tex = NULL;
    game->grass_material.specular_color = vec3_create(0.0f, 0.0f, 0.0f);
    game->grass_material.specular_power = 0.0f;
    game->grass_material.specular_coefficient = 0.0f;

    /** Color material
     */
    game->color_material.albedo_tex = load_texture(game->graphics, "texture.png");
    game->color_material.normal_tex = NULL;
    game->color_material.specular_color = vec3_create(1.0f, 1.0f, 1.0f);
    game->color_material.specular_power = 32.0f;
    game->color_material.specular_coefficient = 1.0f;


    /** terrain material
     */
    game->terrain_material.albedo_tex = load_texture(game->graphics, "land_diffuse.png");
    game->terrain_material.normal_tex = load_texture(game->graphics, "land_normal.png");
    game->terrain_material.specular_color = vec3_create(0.0f, 0.0f, 0.0f);
    game->terrain_material.specular_power = 0.0f;
    game->terrain_material.specular_coefficient = 0.0f;


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
    Transform t = transform_zero;
    Light light = {
        { 0, -1, 0 },
        { 1, 1, 1 },
    };
    float delta_time = (float)get_delta_time(game->timer);

    _control_camera(game, delta_time);

    t.position = vec3_create(10.0f, 3.0f, 0.0f);
    //add_render_command(game->graphics, cube_mesh(game->graphics), &game->color_material, t);

    t = transform_zero;
    add_render_command(game->graphics, game->terrain_mesh, &game->terrain_material, t);

    t = transform_zero;
    t.scale = 0.01f;
    //add_render_command(game->graphics, game->house_mesh, &game->house_material, t);

    set_view_transform(game->graphics, game->camera);
    add_directional_light(game->graphics, light);
}
void render_game(Game* game)
{
    render_graphics(game->graphics);
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
    //_print_touches(game);
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
    //_print_touches(game);
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

    //_print_touches(game);

    if(game->num_points == 1) {
        game->prev_single = game->points[0].pos;
    } else if(game->num_points == 2) {
        Vec2 avg = vec2_add(game->points[0].pos, game->points[1].pos);
        avg = vec2_mul_scalar(avg, 0.5f);
        game->prev_double = avg;
    }
}

