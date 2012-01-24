#pragma once

#include <c_lib/voxel/object_vox.hpp>

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

class Agent_state; // forward declaration

//class Agent_vox: public Object_vox<Agent_state, AGENT_PART_NUM>
//{
    //public:
    //void update(VoxBody* vox_dat);
    //Agent_vox(Agent_state* a, VoxBody* vox_dat) : Object_vox<Agent_state, AGENT_PART_NUM>(a, vox_dat) {}
    ////Agent_vox(Agent_state* a) : Object_vox<Agent_state, AGENT_PART_NUM>(a) {}
//};
class Agent_vox: public Team_vox<Agent_state, AGENT_PART_NUM>
{
    public:
    void update(VoxBody* vox_dat);
    void init_parts(VoxBody* vox_dat);
    Agent_vox(Agent_state* a) : Team_vox<Agent_state, AGENT_PART_NUM>(a) {}
};
