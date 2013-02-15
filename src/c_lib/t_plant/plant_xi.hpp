#pragma once

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/common/map_element.hpp>
#include <t_plant/block_list.hpp>

namespace t_plant
{

struct PlantXiStruct
{
    int x,y,z;  //tree master

    class BlockList bl;
    //bool alive;
    int counter;
};

void plant_xi_init(struct PlantXiStruct &m)
{
    //printf("plant_xi_init: init plant \n");
    m.counter = 0;
    m.bl.init();

    m.x = plant_placement_data.x;
    m.y = plant_placement_data.y;
    m.z = plant_placement_data.z;

    //m.alive = true;

    printf("plant_xi_init: %d %d %d \n", m.x,m.y,m.z);

    CubeType cube_id = t_map::get_cube_type("plant_xi_master");

    //t_map::broadcast_set_block_palette(
    //t_map::set_palette(int x, int y, int z, int palette);

    t_map::set(m.x,m.y,m.z, cube_id);
    t_map::broadcast_set_block(m.x,m.y,m.z, cube_id);

    //t_map::set(m.x,m.y,m.z+1s, cube_id);
    //t_map::broadcast_set_block(m.x,m.y,m.z+1, cube_id);

    //t_map::set(m.x,m.y,m.z, cube_id);
    //t_map::broadcast_set_block(m.x,m.y,m.z+2, cube_id);
    //m.bl.add(m.x, m.y, m.z+1, 0,0);
    //m.bl.add(m.x, m.y, m.z+2, 0,0);

    //CubeType plant_xi_master      = t_map::get_cube_type("plant_xi_master");
    CubeType plant_xi_root      = t_map::get_cube_type("plant_example_root");
    CubeType plant_xi_trunk         = t_map::get_cube_type("plant_example_trunk");
    //CubeType plant_xi_trunk_dead  = t_map::get_cube_type("plant_xi_trunk_dead");
    //CubeType plant_xi_leaves      = t_map::get_cube_type("plant_xi_leaves");
    //CubeType plant_xi_leaves_dead     = t_map::get_cube_type("plant_xi_leaves_dead");

    //m.bl.add(m.x, m.y, m.z, plant_xi_master, 0);

    //roots
    for (int i=1;i<3; i++)
    {
        m.bl.add(m.x-1, m.y, m.z+i, plant_xi_root, 0);
        m.bl.add(m.x+1, m.y, m.z+i, plant_xi_root, 0);
        m.bl.add(m.x, m.y-1, m.z+i, plant_xi_root, 0);
        m.bl.add(m.x, m.y+1, m.z+i, plant_xi_root, 0);
    }
        m.bl.add(m.x, m.y, m.z-1, plant_xi_root, 0);
        m.bl.add(m.x, m.y, m.z-2, plant_xi_root, 0);

    //trunk

    int height = 4+rand()%6;

    for (int i=1;i<=height; i++)
    {
        m.bl.add(m.x, m.y, m.z+i, plant_xi_trunk, 0);
    }

/*
    int width = 1 + rand()%4;
    for (int i= -1*width;i<=width;i++)
    for (int j= -1*width;j<=width;j++)
    {
        if (i==0 && j== 0)
            continue;
        if (abs(i) + abs(j) <= width)
            continue;
        m.bl.add(m.x+i, m.y+j, m.z+height, plant_xi_leaves, 0);
    }
*/

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

/*
    cube_def(SolidCube, "plant_xi_master", CUBE_MATERIAL_STONE);
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

    cube_def(SolidCube, "plant_xi_root", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 2);
    push_texture();

    cube_def(SolidCube, "plant_xi_trunk", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 3);
    push_texture();

    cube_def(SolidCube, "plant_xi_trunk_dead", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 2, 3);
    push_texture();

    cube_def(SolidCube, "plant_xi_leaves", CUBE_MATERIAL_STONE);
    iso_texture(   tree_00, 1, 4);
    push_texture();
    iso_texture(   tree_00, 2, 4);
    push_texture();

    cube_def(SolidCube, "plant_xi_leaves_dead", CUBE_MATERIAL_STONE);
*/

void plant_xi_tick(struct PlantXiStruct &m)
{
/*
    if (t_map::get(m.x,m.y,m.z) != t_map::get_cube_type("tree_example_master"))
    {
        m.alive = false;
    }
*/

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
            int x = m.bl.ba[i].x;
            int y = m.bl.ba[i].y;
            int z = m.bl.ba[i].z;

            CubeType current_block = t_map::get(x,y,z);
            if (current_block == plant_xi_master)
            {
                if (rand()% 25 == 0)
                {
                    struct t_map::MapElement e;
                    e = t_map::get_element(x,y,z);

                    if (e.palette == 0)
                        t_map::broadcast_set_block_palette(x,y,z,plant_xi_master , 1);
                    if (e.palette == 1)
                        t_map::broadcast_set_block_palette(x,y,z,plant_xi_master , 0);
                    if (e.palette == 2)
                        t_map::broadcast_set_block_palette(x,y,z,plant_xi_master , 3);
                    if (e.palette == 3)
                        t_map::broadcast_set_block_palette(x,y,z,plant_xi_master , 2);

                    //t_map::broadcast_set_block_palette(x,y,z,plant_xi_master , (e.pallete + 1)%3;
                }
            }

            if (current_block == plant_xi_leaves)
            {
                if (rand()%60 != 0)
                    continue;

                struct t_map::MapElement e;
                e = t_map::get_element(x,y,z);

                if (e.palette == 0)
                    t_map::broadcast_set_block_palette(x,y,z,plant_xi_leaves , 1);
                if (e.palette == 1)
                    t_map::broadcast_set_block_palette(x,y,z,plant_xi_leaves , 0);
            }

            if (current_block == 0)
            {
                if (rand()% 30 != 0)
                    continue;
                int block_id = m.bl.ba[i].type;
                if (block_id != plant_xi_master)
                    t_map::broadcast_set_block(x,y,z, (CubeType) block_id);
            }
        }
    }

#endif

}

/*
    int adj[6];  //adjacent blocks
    int adjn =0; //adjacent block array

    for (int i=0; i<m.bl.ban; i++)
    {
        int x = m.bl.ba[i].x;
        int y = m.bl.ba[i].y;
        int z = m.bl.ba[i].z;

        //etc does block exist
        if (t_map::get(x,y,z) == 0)
        {

        }

        m.bl.adjacent_blocks(i, adj, &adjn);
    }
*/

}   // t_plant
