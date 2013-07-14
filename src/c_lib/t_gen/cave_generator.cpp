#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_gen/cave_generator.hpp>
namespace t_gen
{

int clamp_cave_height(int z)
{
    return GS_MIN(GS_MAX(z, MAX_HALL_SPHERE_RADIUS + 1),
                  map_dim.z - MAX_HALL_SPHERE_RADIUS - 1);
}

void generate_caves(int n)
{
    //noise_caves();
    //test_caves();
    printf("Starting to generate caves...\n");
    for (int done = 0; done < n; done++)
    {
        int x = randrange(0, map_dim.x - 1);
        int y = randrange(0, map_dim.y - 1);
        int z = t_map::get_highest_open_block(x, y) - randrange(0, 10);
        Vec3i p = vec3i_init(x, y, z);
        Vec3i q = vec3i_init(x, y, z - randrange(3, 5));
        create_node(p, q);
    }
    printf("Cave generation finished.\n");
}

void create_node(Vec3i p, Vec3i q)
{
    p.z = clamp_cave_height(p.z);
    q.z = clamp_cave_height(q.z);
    generate_line(p, q, EMPTY_CUBE, randrange(LINE_MIN_SIZE, LINE_MAX_SIZE));
    for (int tried = 0; tried < MAX_TRIES; tried++)
    {
        if (mrandf() < NODE_PROBABILITY)
        {
            Vec3i r = vec3i_init(q.x + randrange(-MAX_DISTANCE, MAX_DISTANCE),
                                 q.y + randrange(-MAX_DISTANCE, MAX_DISTANCE),
                                 q.z + randrange(MAX_FALL, MAX_RISE));
            create_node(q, r);
        }
        if (mrandf() < HALL_PROBABILITY)
            create_hall(q);
    }
}

void create_hall(Vec3i p)
{
    //printf("Creating a cave hall at %d, %d, %d...\n", x, y, z);
    for (int done = 0; done < N_HALL_SPHERES; done++)
    {
        int radius = randrange(MIN_HALL_SPHERE_RADIUS, MAX_HALL_SPHERE_RADIUS);
        p = translate_position(p);
        generate_sphere(p, radius, EMPTY_CUBE);
        p.x += randrange(-MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        p.y += randrange(-MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        p.z += randrange(-MIN_HALL_SPHERE_RADIUS, MIN_HALL_SPHERE_RADIUS);
        p.z = clamp_cave_height(p.z);
    }
}

void test_caves()
{
    create_hall(vec3i_init(20));
    create_node(vec3i_init(50, 50, 40), vec3i_init(40, 40, 35));
    generate_sphere(vec3i_init(100, 100, 40), randrange(MIN_HALL_SPHERE_RADIUS, MAX_HALL_SPHERE_RADIUS), EMPTY_CUBE);
    generate_line(vec3i_init(150, 150, 40), vec3i_init(160, 160, 36), EMPTY_CUBE, 3);
}

/*
void noise_caves()
{
    make_iceflame_noise();
    for (int z = 5; z < map_dim.z; z++)
    {
        shuffle_iceflame_noise(3);
        for (int x = 0; x < map_dim.x; x++)
        for (int y = 0; y < map_dim.y; y++)
        if (get_iceflame_noise(x, y) > 500) t_map::set(x, y, z, EMPTY_CUBE);
    }
}
*/

} // t_gen
