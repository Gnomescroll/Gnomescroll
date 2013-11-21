/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

const int LINE_MIN_SIZE = 1;
const int LINE_MAX_SIZE = 5;
const float NODE_PROBABILITY = 0.45f;
const int MAX_DISTANCE = 10;
const int MIN_HALL_SPHERE_RADIUS = 2;
const int MAX_HALL_SPHERE_RADIUS = 6;
const int N_HALL_SPHERES = 3;
const int MAX_FALL = -6;
const int MAX_RISE = 4;
const int MAX_TRIES = 2;
const float HALL_PROBABILITY = 0.03f;

void generate_caves(int n);
void create_node(Vec3i p, Vec3i q);
void create_hall(Vec3i p);
int clamp_cave_height(int z);
int bounce_point(int n);
void test_caves();
void noise_caves();

}   // t_gen
