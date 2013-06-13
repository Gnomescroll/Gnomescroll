#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_gen/cave_generator.hpp>
namespace t_gen
{

int bounce_point(int n)
{
    if(n < MAX_HALL_SPHERE_RADIUS) return 0 - n + MAX_HALL_SPHERE_RADIUS;
    if(n >= map_dim.x - MAX_HALL_SPHERE_RADIUS) return map_dim.x - 1 - n - MAX_HALL_SPHERE_RADIUS;
    return n;
}

int clamp_cave_height(int z)
{
    if(z <= MAX_HALL_SPHERE_RADIUS) return MAX_HALL_SPHERE_RADIUS + 1;
    if(z >= map_dim.z - MAX_HALL_SPHERE_RADIUS) return map_dim.z - MAX_HALL_SPHERE_RADIUS - 1;
    return z;
}

void generate_caves()
{
    //noise_caves();
    //test_caves();
    printf("Starting to generate caves...\n");
    int x;
    int y;
    for(int done = 0; done < N_CAVES; done++)
    {
        x = randrange(MAX_SIZE, map_dim.x - MAX_SIZE - 1);
        y = randrange(MAX_SIZE, map_dim.y - MAX_SIZE - 1);
        create_node(x, y, clamp_cave_height(t_map::get_highest_open_block(x, y)), x, y, clamp_cave_height(t_map::get_highest_open_block(x, y) - 3));
    }
    printf("Cave generation finished.\n");
    return;
}

void create_node(int x, int y, int z, int ox, int oy, int oz)
{
    printf("Creating a cave node from %d, %d, %d to %d, %d, %d...\n", x, y, z, ox, oy, oz);
    generate_line(x, y, z, ox, oy, oz, EMPTY_CUBE, randrange(MIN_SIZE, MAX_SIZE));
    for(int tried = 0; tried < MAX_TRIES; tried++)
    if(randrange(0, 99) < NODE_PROBABILITY) create_node(ox, oy, oz, bounce_point(ox + randrange(0 - MAX_DISTANCE, MAX_DISTANCE)), bounce_point(oy + randrange(0 - MAX_DISTANCE, MAX_DISTANCE)), clamp_cave_height(oz + randrange(MAX_FALL, MAX_RISE)));
    if(randrange(0, 99) < HALL_PROBABILITY) create_hall(ox, oy, oz);
    return;
}

void create_hall(int x, int y, int z)
{
    printf("Creating a cave hall at %d, %d, %d...\n", x, y, z);
    for(int done = 0; done < N_HALL_SPHERES; done++)
    {
        x += randrange(0 - MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        x = bounce_point(x);
        y += randrange(0 - MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        y = bounce_point(y);
        z += randrange(0 - MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        z = clamp_cave_height(z);
        generate_sphere(vec3i_init(x, y, z), randrange(MIN_HALL_SPHERE_RADIUS, MAX_HALL_SPHERE_RADIUS), EMPTY_CUBE);
    }
    return;
}

void test_caves()
{
    create_hall(20, 20, 20);
    create_node(50, 50, 40, 40, 40, 35);
    generate_sphere(vec3i_init(100, 100, 40), randrange(MIN_HALL_SPHERE_RADIUS, MAX_HALL_SPHERE_RADIUS), EMPTY_CUBE);
    generate_line(150, 150, 40, 160, 160, 36, EMPTY_CUBE, 3);
    return;
}
/*
void noise_caves()
{
    make_iceflame_noise();
    for(int z = 5; z < map_dim.z; z++)
    {
        shuffle_iceflame_noise(3);
        for(int x = 0; x < map_dim.x; x++)
        for(int y = 0; y < map_dim.y; y++)
        if(get_iceflame_noise(x, y) > 500) t_map::set(x, y, z, EMPTY_CUBE);
    }
}
*/
}//end of t_gen
