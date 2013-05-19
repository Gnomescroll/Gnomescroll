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
            Vec3i position;
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

    void add(const Vec3i& position, int type, int state)
    {
        if (ban == bam)
        {
            printf("Warning: BlockList::add failed, list full \n");
            return;
        }

        ba[ban].position = position;
        ba[ban].type  = type;
        ba[ban].state = state;

        c = &ba[ban];
        ban++;
    }

    void add(int x, int y, int z, int type, int state)
    {
        this->add(vec3i_init(x, y, z), type, state);
    }

    void remove(const Vec3i& position)
    {

        for (int i=0;i<ban;i++)
        {
            if (is_equal(ba[i].position, position))
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
        //for (int i=0; i<bam; i++)
        //{
            //// do something?
        //}
    }

    static inline bool _adj(const Vec3i& a, const Vec3i& b)
    {
        const int bit = 512 - 1;
        return ((a.y == b.y && a.z == b.z && ((a.x == ((b.x - 1) & bit)) ||
                                              (a.x == ((b.x + 1) & bit)))) ||
                (a.x == b.x && a.z == b.z && ((a.y == ((b.y - 1) & bit)) ||
                                              (a.y == ((b.y + 1) & bit)))) ||
                (a.x == b.x && a.y == b.y && ((a.z == ((b.z - 1) & bit)) ||
                                              (a.z == ((b.z + 1) & bit)))));
    }
    //returns list indices of blocks adjccent to current block
    void adjacent_blocks(int index, int* b_array, int* num_results)
    {
        int num=0;

        Vec3i p = ba[index].position;

        for (int i=0; i<ban; i++)
        {

            if (!_adj(p, ba[i].position))
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

} // t_plant
