#pragma once

#if DC_CLIENT
    #include <camera/camera.hpp>
    #include <physics/ray_trace/hitscan.hpp>
#endif

namespace t_mech
{


struct MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

    bool active;
};


void init() {}
void teardown() {}

}