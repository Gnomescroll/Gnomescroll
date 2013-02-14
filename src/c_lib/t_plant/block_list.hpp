#pragma once

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

    struct BlockArrayElement* c;    //current block/last added block

    void init()
    {
        ban = 0;
        bam = 1024;
        ba = new struct BlockArrayElement[bam];

        for (int i=0;i<bam;i++)
            ba[i].type = tree_error;
    }

    void teardown() {}

    void add(int x, int y, int z, int type, int state)
    {
        if (ban == bam)
        {
            printf("Warning: BlockList::add failed, list full \n");
            return;
        }

        ba[ban].x = x;
        ba[ban].y = y;
        ba[ban].z = z;
        ba[ban].type  = type;
        ba[ban].state = state;

        c = &ba[ban];
        ban++;
    }

    void remove(int x, int y, int z)
    {

        for (int i=0;i<ban;i++)
        {
            if (ba[i].x == x && ba[i].y == y && ba[i].z == z)
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
        for (int i=0; i<bam; i++)
        {
            // do something?
        }
    }

    static inline bool _adj(int x, int y, int z, int _x,int _y,int _z)
    {
        const int bit = 512-1;

        if (y == _y && z == _z && (((x == ((_x-1) & bit)) || (x == ((_x+1) & bit)))))
        {
            return true;
        }

        if (x == _x && z == _z && ((y == ((_y-1) & bit)) || (y == ((_y+1) & bit))))
        {
            return true;
        }

        if (x == _x && y == _y && ((z == ((_z-1) & bit)) || (z == ((_z+1) & bit))))
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

        for (int i=0; i<ban; i++)
        {

            if (!_adj(x,y,z, ba[i].x,ba[i].y,ba[i].z))
                continue;
            b_array[num] = i;
            num++;
        }
        *num_results = num;
    /*
        GS_ASSERT(num <= 6);
        if (num > 0)
            printf("adj: %d %d %d \n", x,y,z);
        for (int i=0; i<num; i++)
        {
            int in = b_array[i];
            printf("_adj: %d: %d %d %d \n", i, ba[in].x,ba[in].y,ba[in].z);
        }
    */
    }
};

} // end t_plant
