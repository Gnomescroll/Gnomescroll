#pragma once

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/common/map_element.hpp>
#include <t_plant/block_list.hpp>

namespace t_plant
{

struct PlantXiStruct
{
    Vec3i position;  //tree master
    class BlockList bl;
    //bool alive;
    int counter;
};

void plant_xi_init(struct PlantXiStruct &m)
{
    //printf("plant_xi_init: init plant \n");
    m.counter = 0;
    m.bl.init();

    m.position = plant_placement_data.position;

    printf("plant_xi_init: ");
    vec3i_print(m.position);

    CubeType cube_id = t_map::get_cube_type("plant_xi_master");

    t_map::set(m.position, cube_id);
    t_map::broadcast_set_block(m.position, cube_id);

    //CubeType plant_xi_master      = t_map::get_cube_type("plant_xi_master");
    CubeType plant_xi_root      = t_map::get_cube_type("plant_example_root");
    CubeType plant_xi_trunk         = t_map::get_cube_type("plant_example_trunk");
    //CubeType plant_xi_trunk_dead  = t_map::get_cube_type("plant_xi_trunk_dead");
    //CubeType plant_xi_leaves      = t_map::get_cube_type("plant_xi_leaves");
    //CubeType plant_xi_leaves_dead     = t_map::get_cube_type("plant_xi_leaves_dead");

    //roots
    Vec3i p = m.position;
    for (int i=1;i<3; i++)
    {
        m.bl.add(p.x-1, p.y, p.z+i, plant_xi_root, 0);
        m.bl.add(p.x+1, p.y, p.z+i, plant_xi_root, 0);
        m.bl.add(p.x, p.y-1, p.z+i, plant_xi_root, 0);
        m.bl.add(p.x, p.y+1, p.z+i, plant_xi_root, 0);
    }
    m.bl.add(p.x, p.y, p.z-1, plant_xi_root, 0);
    m.bl.add(p.x, p.y, p.z-2, plant_xi_root, 0);

    //trunk

    int height = 4+rand()%6;

    for (int i=1;i<=height; i++)
    {
        m.bl.add(p.x, p.y, p.z+i, plant_xi_trunk, 0);
    }

    for (int i=0; i<m.bl.ban; i++)
    {
        int adj[6];  //adjacent blocks
        int adjn =0; //adjacent block array

        m.bl.adjacent_blocks(i, adj, &adjn);

        if (adjn == 0)
        {
            printf("ERROR: plant_xi_init, block is floating in air!  Has no neightbors\n");
        }
        //GS_ASSERT(adjn > 0);
    }
}

void plant_xi_teardown(struct PlantXiStruct &m)
{

}

void plant_xi_tick(struct PlantXiStruct &m)
{

#if 0
    //static CubeType plant_xi_master       = t_map::get_cube_type("plant_xi_master");
    static CubeType plant_xi_root           = t_map::get_cube_type("plant_example_root");
    static CubeType plant_xi_trunk      = t_map::get_cube_type("plant_example_trunk");
    //static CubeType plant_xi_trunk_dead   = t_map::get_cube_type("plant_xi_trunk_dead");
    //static CubeType plant_xi_leaves       = t_map::get_cube_type("plant_xi_leaves");
    //static CubeType plant_xi_leaves_dead  = t_map::get_cube_type("plant_xi_leaves_dead");
    if (m.counter % 15 == 0)
    {

        for (int i=0; i<m.bl.ban; i++)
        {
            Vec3i position = m.bl.ba[i].position;

            CubeType current_block = t_map::get(position);
            if (current_block == plant_xi_master)
            {
                if (rand()% 25 == 0)
                {
                    struct t_map::MapElement e;
                    e = t_map::get_element(position);

                    if (e.palette == 0)
                        t_map::broadcast_set_block_palette(position, plant_xi_master, 1);
                    if (e.palette == 1)
                        t_map::broadcast_set_block_palette(position, plant_xi_master, 0);
                    if (e.palette == 2)
                        t_map::broadcast_set_block_palette(position, plant_xi_master, 3);
                    if (e.palette == 3)
                        t_map::broadcast_set_block_palette(position, plant_xi_master, 2);

                    //t_map::broadcast_set_block_palette(position, plant_xi_master, (e.pallete + 1)%3;
                }
            }

            if (current_block == plant_xi_leaves)
            {
                if (rand()%60 != 0)
                    continue;

                struct t_map::MapElement e;
                e = t_map::get_element(x,y,z);

                if (e.palette == 0)
                    t_map::broadcast_set_block_palette(position, plant_xi_leaves, 1);
                if (e.palette == 1)
                    t_map::broadcast_set_block_palette(position, plant_xi_leaves, 0);
            }

            if (current_block == 0)
            {
                if (rand()% 30 != 0)
                    continue;
                int block_id = m.bl.ba[i].type;
                if (block_id != plant_xi_master)
                    t_map::broadcast_set_block(position, (CubeType) block_id);
            }
        }
    }

#endif

}

}   // t_plant
