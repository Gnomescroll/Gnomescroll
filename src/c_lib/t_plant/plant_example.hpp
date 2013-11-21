/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/common/map_element.hpp>

#include <t_plant/block_list.hpp>

namespace t_plant
{

struct PlantExampleStruct
{
    Vec3i position;
    class BlockList bl;
    //bool alive;
    int counter;
};

void plant_example_init(struct PlantExampleStruct &m)
{
    //printf("plant_example_init: init plant \n");
    m.counter = 0;
    m.bl.init();

    m.position = plant_placement_data.position;

    //m.alive = true;

    printf("plant_example_init: ");
    vec3i_print(m.position);

    CubeType cube_id = t_map::get_cube_type("plant_example_master");

    //t_map::broadcast_set_block_palette(
    //t_map::set_palette(const Vec3i& position, int palette);

    t_map::set(m.position, cube_id);
    t_map::broadcast_set_block(m.position, cube_id);

    //t_map::set(m.x,m.y,m.z+1s, cube_id);
    //t_map::broadcast_set_block(m.x,m.y,m.z+1, cube_id);

    //t_map::set(m.x,m.y,m.z, cube_id);
    //t_map::broadcast_set_block(m.x,m.y,m.z+2, cube_id);
    //m.bl.add(m.x, m.y, m.z+1, 0,0);
    //m.bl.add(m.x, m.y, m.z+2, 0,0);

    CubeType plant_example_master       = t_map::get_cube_type("plant_example_master");
    CubeType plant_example_root         = t_map::get_cube_type("plant_example_root");
    CubeType plant_example_trunk        = t_map::get_cube_type("plant_example_trunk");
    //CubeType plant_example_trunk_dead     = t_map::get_cube_type("plant_example_trunk_dead");
    CubeType plant_example_leaves       = t_map::get_cube_type("plant_example_leaves");
    //CubeType plant_example_leaves_dead    = t_map::get_cube_type("plant_example_leaves_dead");

    m.bl.add(m.position, plant_example_master, 0);

    //roots
    for (int i=1;i<3; i++)
    {
        m.bl.add(m.position.x-1, m.position.y, m.position.z+i, plant_example_root, 0);
        m.bl.add(m.position.x+1, m.position.y, m.position.z+i, plant_example_root, 0);
        m.bl.add(m.position.x, m.position.y-1, m.position.z+i, plant_example_root, 0);
        m.bl.add(m.position.x, m.position.y+1, m.position.z+i, plant_example_root, 0);
    }
    m.bl.add(m.position.x, m.position.y, m.position.z-1, plant_example_root, 0);
    m.bl.add(m.position.x, m.position.y, m.position.z-2, plant_example_root, 0);

    //trunk

    int height = 4+rand()%6;

    for (int i=1; i<=height; i++)
    {
        m.bl.add(m.position.x, m.position.y, m.position.z+i, plant_example_trunk, 0);
    }

    int width = 1 + rand()%4;

    for (int i = -1*width; i<=width; i++)
    for (int j = -1*width; j<=width; j++)
    {
        if (i==0 && j== 0)
            continue;
        if (abs(i) + abs(j) <= width)
            continue;
        m.bl.add(m.position.x + i, m.position.y + j, m.position.z + height,
                 plant_example_leaves, 0);
    }
}

void plant_example_teardown(struct PlantExampleStruct &m)
{

}

/*
    cube_def(SolidCube, "plant_example_master", CUBE_MATERIAL_STONE);
    set_max_damage(128);

    iso_texture(   tree_00, 1, 1);
    side_texture(T, tree_00, 2, 2);
    side_texture(B, tree_00, 2, 2);
    push_texture();

    iso_texture(   tree_00, 2, 1);
    side_texture(T, tree_00, 2, 2);
    side_texture(B, tree_00, 2, 2);
    push_texture();

    iso_texture(   tree_00, 3, 1);
    side_texture(T, tree_00, 3, 2);
    side_texture(B, tree_00, 3, 2);
    push_texture();

    iso_texture(   tree_00, 4, 1);
    side_texture(T, tree_00, 4, 2);
    side_texture(B, tree_00, 4, 2);
    push_texture();

    cube_def(SolidCube, "plant_example_root", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 2);
    push_texture();

    cube_def(SolidCube, "plant_example_trunk", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 3);
    push_texture();

    cube_def(SolidCube, "plant_example_trunk_dead", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 2, 3);
    push_texture();

    cube_def(SolidCube, "plant_example_leaves", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 4);
    push_texture();
    iso_texture(   tree_00, 2, 4);
    push_texture();

    cube_def(SolidCube, "plant_example_leaves_dead", CUBE_MATERIAL_STONE);
*/

void plant_example_tick(struct PlantExampleStruct &m)
{
/*
    if (t_map::get(m.x,m.y,m.z) != t_map::get_cube_type("tree_example_master"))
    {
        m.alive = false;
    }
*/

    static CubeType plant_example_master        = t_map::get_cube_type("plant_example_master");
    //static CubeType plant_example_root          = t_map::get_cube_type("plant_example_root");
    //static CubeType plant_example_trunk         = t_map::get_cube_type("plant_example_trunk");
    //static CubeType plant_example_trunk_dead    = t_map::get_cube_type("plant_example_trunk_dead");
    static CubeType plant_example_leaves        = t_map::get_cube_type("plant_example_leaves");
    //static CubeType plant_example_leaves_dead   = t_map::get_cube_type("plant_example_leaves_dead");

    if (m.counter % 15 == 0)
    {

        for (int i=0; i<m.bl.ban; i++)
        {
            Vec3i p = m.bl.ba[i].position;

            CubeType current_block = t_map::get(p);
            if (current_block == plant_example_master)
            {
                if (rand()% 25 == 0)
                {
                    struct t_map::MapElement e;
                    e = t_map::get_element(p);

                    if (e.palette == 0)
                        t_map::broadcast_set_block_palette(p, plant_example_master , 1);
                    if (e.palette == 1)
                        t_map::broadcast_set_block_palette(p, plant_example_master , 0);
                    if (e.palette == 2)
                        t_map::broadcast_set_block_palette(p, plant_example_master , 3);
                    if (e.palette == 3)
                        t_map::broadcast_set_block_palette(p, plant_example_master , 2);

                    //t_map::broadcast_set_block_palette(p, plant_example_master , (e.pallete + 1)%3;
                }
            }

            if (current_block == plant_example_leaves)
            {
                if (rand() % 60 != 0)
                    continue;

                struct t_map::MapElement e;
                e = t_map::get_element(p);

                if (e.palette == 0)
                    t_map::broadcast_set_block_palette(p, plant_example_leaves , 1);
                if (e.palette == 1)
                    t_map::broadcast_set_block_palette(p, plant_example_leaves , 0);
            }

            if (current_block == 0)
            {
                if (rand() % 30 != 0)
                    continue;
                int block_id = m.bl.ba[i].type;
                if (block_id != plant_example_master)
                    t_map::broadcast_set_block(p, (CubeType) block_id);
            }
        }
    }
}

}   // t_plant
