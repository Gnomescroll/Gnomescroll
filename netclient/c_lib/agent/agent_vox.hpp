#pragma once

#include <math.h>

#ifdef DC_CLIENT
#include <compat_gl.h>
#endif

#include <c_lib/voxel/common.h>
#include <c_lib/voxel/voxel_render.hpp>
#include <c_lib/voxel/voxel_body.hpp>
#include <c_lib/voxel/object_vox.hpp>
#include <physics/vector.hpp>

extern VoxBody agent_vox_dat;

const int AGENT_PART_NUM = 6;
enum AGENT_BODY_PARTS {
    AGENT_PART_HEAD,
    AGENT_PART_TORSO,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG
};

//class Agent_vox {
    //public:
        //void right(Vector *f, float theta);
        //void forward(Vector *f, float theta);
        //void look(Vector *f, float theta, float phi);

        //Agent_state* a;

        //Voxel_volume vv[AGENT_PART_NUM];

        //void init_parts();
        //void update();
        //void set_draw(bool draw);

        //Agent_vox(Agent_state* a);
        //~Agent_vox();
//};


class Agent_state; // forward declaration

class Agent_vox: public Object_vox<Agent_state, AGENT_PART_NUM>
{
    public:
    void update(VoxBody* vox_dat);
    Agent_vox(Agent_state* a, VoxBody* vox_dat) : Object_vox(a, vox_dat) {}
};
