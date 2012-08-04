#pragma once

#if DC_CLIENT
    #include <camera/camera.hpp>
    #include <physics/ray_trace/hitscan.hpp>
#endif

namespace t_mech
{

enum MECH_TYPE
{
	MECH_CRYSTAL,
	MECH_CROP,
	MECH_WIRE,
	MECH_SWITCH
};

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