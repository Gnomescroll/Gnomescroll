#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <c_lib/agent/agent.hpp>


/*

class AgentState {
    public:
        int seq;
        float theta;
        float phi;        
        float x,y,z;
        float vx,vy,vz;
        float jump_pow;
        
        AgentState();
        
        void forward_vector(float f[3]);
        Vec3 forward_vector();
};

*/

// TODO: detect if player is on ground
// TODO: detect when player goes form on-ground to ground (fall event)
// TODO: detect the type of block the player is walking on
// TODO: random plus-minue variation in footstep

void player_agent_sound_movement_event(class AgentState s0,  class AgentState s1)
{
    static const float distance_per_step = 1.0;
    static float total_distance = 0.0;

    float dx = (s1.x - s0.x);
    float dy = (s1.y - s0.y);

    float d = sqrt(dx*dx + dy*dy);

    total_distance += d;

    if(total_distance > distance_per_step )
    {
        //play sound
        //printf("play agent sound !!! \n");
        total_distance -= distance_per_step;
    }

}
