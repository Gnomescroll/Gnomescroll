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
