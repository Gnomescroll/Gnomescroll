#pragma once

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/common/map_element.hpp>

namespace t_plant
{

class BlockList
{
    public:

    //CubeType bl[16]; //block lookup

    enum
    {
        tree_error,
        tree_master,
        tree_root,
        tree_trunk_alive,
        tree_trunk_dead,
        tree_leave_alive,
        tree_leave_dead,
    } BlockType;

    enum
    {
        tree_alive,
        tree_dead,
    } BlockState;

    struct BlockArrayElement
    {
        int x,y,z;
        int block_id;
        int type;
        int state;
    };

    struct BlockArrayElement* ba;
    int ban;
    int bam;

    void init()
    {
        ban = 0;
        bam = 1024;
        ba = new struct BlockArrayElement[bam];

        for(int i=0;i<bam;i++)
            ba[i].type = tree_error;
    }

    void teardown() {}

    void add(int x, int y, int z, int type, int state)
    {
        if(ban == bam)
        {
            printf("Warning: BlockList::add failed, list full \n");
            return;
        }

        ba[ban].x = x;
        ba[ban].y = y;
        ba[ban].z = z;
        ba[ban].type  = type;
        ba[ban].state = state;

        ban++;
    }

    void remove(int x, int y, int z)
    {

        for(int i=0;i<ban;i++)
        {
            if(ba[i].x == x && ba[i].y == y && ba[i].z == z)
            {
                GS_ASSERT(ban > 0);
                ban--;
                ba[i] = ba[ban];
                return;
            }
        }
        printf("ERROR BlockList: remove, could not remove block, block not in list \n");
    }

    void check_blocks()
    {
        for(int i=0; i<bam; i++)
        {
            // do something?
        }
    }

    static inline bool _adj(int x, int y, int z, int _x,int _y,int _z)
    {
        const int bit = 512-1;

        if( y == _y && z == _z && (((x == ((_x-1) & bit)) || (x == ((_x+1) & bit)))) )
        {
            return true;
        }

        if( x == _x && z == _z && ((y == ((_y-1) & bit)) || (y == ((_y+1) & bit))) )
        {
            return true;
        }

        if( x == _x && y == _y && ((z == ((_z-1) & bit)) || (z == ((_z+1) & bit))) )
        {
            return true;
        }

        return false;
    }
    //returns list indices of blocks adjccent to current block
    void adjacent_blocks(int index, int* b_array, int* num_results)
    {
        int num=0;

        int x = ba[index].x;
        int y = ba[index].y;
        int z = ba[index].z;

        for(int i=0; i<ban; i++)
        {

            if(!_adj(x,y,z, ba[i].x,ba[i].y,ba[i].z))
                continue;
            b_array[num] = i;
            num++;
        }
        *num_results = num;
    /*
        GS_ASSERT(num <= 6);
        if(num > 0)
            printf("adj: %d %d %d \n", x,y,z);
        for(int i=0; i<num; i++)
        {
            int in = b_array[i];
            printf("_adj: %d: %d %d %d \n", i, ba[in].x,ba[in].y,ba[in].z );
        }
    */
    }
};


struct PlantExampleStruct
{
    int x,y,z;  //tree master

    class BlockList bl;
    //bool alive;
    int counter;
};

void plant_example_init(struct PlantExampleStruct &m)
{
    //printf("plant_example_init: init plant \n");
    m.counter = 0;
    m.bl.init();

    m.x = plant_placement_data.x;
    m.y = plant_placement_data.y;
    m.z = plant_placement_data.z;

    //m.alive = true;

    printf("plant_example_init: %d %d %d \n", m.x,m.y,m.z);

    CubeType cube_id = t_map::get_cube_type("plant_example_master");

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

    CubeType plant_example_master       = t_map::get_cube_type("plant_example_master");
    CubeType plant_example_root         = t_map::get_cube_type("plant_example_root");
    CubeType plant_example_trunk        = t_map::get_cube_type("plant_example_trunk");
    //CubeType plant_example_trunk_dead     = t_map::get_cube_type("plant_example_trunk_dead");
    CubeType plant_example_leaves       = t_map::get_cube_type("plant_example_leaves");
    //CubeType plant_example_leaves_dead    = t_map::get_cube_type("plant_example_leaves_dead");

    m.bl.add(m.x, m.y, m.z, plant_example_master, 0);

    //roots
    for(int i=1;i<3; i++)
    {
        m.bl.add(m.x-1, m.y, m.z+i, plant_example_root, 0);
        m.bl.add(m.x+1, m.y, m.z+i, plant_example_root, 0);
        m.bl.add(m.x, m.y-1, m.z+i, plant_example_root, 0);
        m.bl.add(m.x, m.y+1, m.z+i, plant_example_root, 0);
    }
        m.bl.add(m.x, m.y, m.z-1, plant_example_root, 0);
        m.bl.add(m.x, m.y, m.z-2, plant_example_root, 0);

    //trunk

    int height = 4+rand()%6;

    for(int i=1;i<=height; i++)
    {
        m.bl.add(m.x, m.y, m.z+i, plant_example_trunk, 0);
    }

    int width = 1 + rand()%4;

    for(int i= -1*width;i<=width;i++)
    for(int j= -1*width;j<=width;j++)
    {
        if(i==0 && j== 0)
            continue;
        if( abs(i) + abs(j) <= width)
            continue;
        m.bl.add(m.x+i, m.y+j, m.z+height, plant_example_leaves, 0);
    }
}

void plant_example_teardown(struct PlantExampleStruct &m)
{

}

/*
    cube_def(SolidCube, "plant_example_master", CUBE_MATERIAL_STONE);
    set_max_damage(128);

    iso_texture(    tree_00, 1, 1);
    side_texture(T, tree_00, 2, 2);
    side_texture(B, tree_00, 2, 2);
    push_texture();

    iso_texture(    tree_00, 2, 1);
    side_texture(T, tree_00, 2, 2);
    side_texture(B, tree_00, 2, 2);
    push_texture();

    iso_texture(    tree_00, 3, 1);
    side_texture(T, tree_00, 3, 2);
    side_texture(B, tree_00, 3, 2);
    push_texture();

    iso_texture(    tree_00, 4, 1);
    side_texture(T, tree_00, 4, 2);
    side_texture(B, tree_00, 4, 2);
    push_texture();

    cube_def(SolidCube, "plant_example_root", CUBE_MATERIAL_STONE);
    iso_texture(    tree_00, 1, 2);
    push_texture();

    cube_def(SolidCube, "plant_example_trunk", CUBE_MATERIAL_STONE);
    iso_texture(    tree_00, 1, 3);
    push_texture();

    cube_def(SolidCube, "plant_example_trunk_dead", CUBE_MATERIAL_STONE);
    iso_texture(    tree_00, 2, 3);
    push_texture();

    cube_def(SolidCube, "plant_example_leaves", CUBE_MATERIAL_STONE);
    iso_texture(    tree_00, 1, 4);
    push_texture();
    iso_texture(    tree_00, 2, 4);
    push_texture();

    cube_def(SolidCube, "plant_example_leaves_dead", CUBE_MATERIAL_STONE);
*/

void plant_example_tick(struct PlantExampleStruct &m)
{
/*
    if(t_map::get(m.x,m.y,m.z) != t_map::get_cube_type("tree_example_master"))
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

    if(m.counter % 15 == 0)
    {

        for(int i=0; i<m.bl.ban; i++)
        {
            int x = m.bl.ba[i].x;
            int y = m.bl.ba[i].y;
            int z = m.bl.ba[i].z;

            CubeType current_block = t_map::get(x,y,z);
            if(current_block == plant_example_master)
            {
                if( rand()% 25 == 0)
                {
                    struct t_map::MAP_ELEMENT e;
                    e = t_map::get_element(x,y,z);

                    if(e.palette == 0)
                        t_map::broadcast_set_block_palette(x,y,z,plant_example_master , 1);
                    if(e.palette == 1)
                        t_map::broadcast_set_block_palette(x,y,z,plant_example_master , 0);
                    if(e.palette == 2)
                        t_map::broadcast_set_block_palette(x,y,z,plant_example_master , 3);
                    if(e.palette == 3)
                        t_map::broadcast_set_block_palette(x,y,z,plant_example_master , 2);

                    //t_map::broadcast_set_block_palette(x,y,z,plant_example_master , (e.pallete + 1)%3;
                }
            }

            if(current_block == plant_example_leaves)
            {
                if(rand()%60 != 0)
                    continue;

                struct t_map::MAP_ELEMENT e;
                e = t_map::get_element(x,y,z);

                if(e.palette == 0)
                    t_map::broadcast_set_block_palette(x,y,z,plant_example_leaves , 1);
                if(e.palette == 1)
                    t_map::broadcast_set_block_palette(x,y,z,plant_example_leaves , 0);
            }

            if(current_block == 0)
            {
                if( rand()% 30 != 0)
                    continue;
                int block_id = m.bl.ba[i].type;
                if(block_id != plant_example_master)
                    t_map::broadcast_set_block(x,y,z, (CubeType) block_id);
            }
        }
    }
/*
    int adj[6];  //adjacent blocks
    int adjn =0; //adjacent block array

    for(int i=0; i<m.bl.ban; i++)
    {
        int x = m.bl.ba[i].x;
        int y = m.bl.ba[i].y;
        int z = m.bl.ba[i].z;

        //etc does block exist
        if( t_map::get(x,y,z) == 0 )
        {

        }

        m.bl.adjacent_blocks(i, adj, &adjn);
    }
*/
}


}   // t_plant
