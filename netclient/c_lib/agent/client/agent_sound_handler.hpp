#pragma once

#include <c_lib/agent/agent.hpp>

float distance_per_step = 1.0;
float total_distance = 0.0;

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

void player_agent_sound_movement_event(class AgentState s0,  class AgentState s1)
{

	float dx = (s1.x - s0.x);
	float dy = (s1.y - s0.y);

	float d = sqrt(dx*dx + dy*dy);

	total_distance += d;

	if(total_distance > distance_per_step )
	{
		//play sound
		printf("play agent sound !!! \n");
		total_distance -= distance_per_step;
	}

}