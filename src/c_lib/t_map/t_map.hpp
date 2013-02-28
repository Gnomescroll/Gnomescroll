#pragma once

#include <physics/vec3.hpp>

#include <common/defines.h>
#include <t_map/common/types.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/map_element.hpp>

#include <t_map/t_properties.hpp>

namespace t_map
{

extern class Terrain_map* main_map;

class Terrain_map* get_map();


class GetNearestSurfaceBlockIter
{
    private:

    inline void init();

    public:
        static const int END = -1;
        struct MapPos pos;
        int clearance;
        int up, down;
        bool up_done, down_done;

    int next();

    int get(int i)
    {   // get the i'th nearest surface block, 1-indexed
        GS_ASSERT(i >= 1);
        int x = 0;
        int z = this->END;
        while ((z = this->next()) != this->END && ++x < i);
        return z;
    }

    void init(const struct MapPos& pos, int clearance)
    {
        this->pos = pos;
        this->clearance = clearance;
        this->init();
    }

    GetNearestSurfaceBlockIter() :
        pos(map_pos_init(0,0,0)), clearance(1), up(0), down(0),
        up_done(false), down_done(false)
    {
    }

    GetNearestSurfaceBlockIter(const struct MapPos& pos) :
        pos(pos), clearance(1), up(0), down(0), up_done(false),
        down_done(false)
    {
        this->init();
    }

    GetNearestSurfaceBlockIter(const struct MapPos& pos, int clearance) :
        pos(pos), clearance(clearance), up(0), down(0), up_done(false),
        down_done(false)
    {
        this->init();
    }
};

}   // t_map
