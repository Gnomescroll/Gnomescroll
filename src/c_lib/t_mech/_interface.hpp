#pragma once

#if DC_CLIENT
    #include <camera/camera.hpp>
    #include <physics/ray_trace/hitscan.hpp>
#endif

namespace t_mech
{

class MECH_LIST* mech_list;

enum MECH_TYPE
{
	MECH_CRYSTAL,
	MECH_CROP,
	MECH_WIRE,
	MECH_SWITCH
};

struct _MECH
{
    int x,y,z; //position
    int type;  //type
    int direction; //direction

    bool active;
};


void init() {}
void teardown() {}

void init_packets();

#if DC_CLIENT

//class ControlNodeRenderer* mech_list_renderer = NULL;
class MechListRenderer* mech_list_renderer = NULL;

void prep()
{

}

void draw()
{


}

#endif

#if DC_SERVER


#endif


}