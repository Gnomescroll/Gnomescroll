#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <common/random.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/config/block_dat.hpp>
#include <t_gen/shapes.hpp>
//#include <t_gen/iceflame_noise.hpp>

namespace t_gen
{

const int N_CAVES = 50;
const int MIN_SIZE = 1;
const int MAX_SIZE = 5;
const int NODE_PROBABILITY = 45; //probability of a next node generating, in percent
const int MAX_DISTANCE = 10;
const int MIN_HALL_SPHERE_RADIUS = 2;
const int MAX_HALL_SPHERE_RADIUS = 6;
const int N_HALL_SPHERES = 10;
const int MAX_FALL = -6;
const int MAX_RISE = 4;
const int MAX_TRIES = 2;
const int HALL_PROBABILITY = 3;

void generate_caves();
void create_node(int x, int y, int z, int ox, int oy, int oz);
void create_hall(int x, int y, int z);
int clamp_cave_height(int z);
int bounce_point(int n);
void test_caves();
void noise_caves();

}
