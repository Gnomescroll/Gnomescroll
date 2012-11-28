#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent.hpp>

// TODO: detect if player is on ground
// TODO: detect when player goes form on-ground to ground (fall event)
// TODO: detect the type of block the player is walking on
// TODO: random plus-minue variation in footstep

void player_agent_sound_ground_movement_event(class AgentState s0,  class AgentState s1, bool s1_on_ground, bool camera_on_ground)
{
    static const float SOFT_FALL_VELOCITY_THRESHOLD   = 0.05f;
    static const float MEDIUM_FALL_VELOCITY_THRESHOLD = 0.50f;
    static const float HARD_FALL_VELOCITY_THRESHOLD   = 0.75f;
    
    static const float distance_per_step = 1.5f;
    static float total_distance = 0.0f;


    float dx = (s1.x - s0.x);
    float dy = (s1.y - s0.y);

    float d = sqrtf(dx*dx + dy*dy);

    total_distance += d;

    float dvz = s1.vz - s0.vz;
    float dz  = s1.z  - s0.z;
    if (/* TODO */ false && dz < 0) // was falling & hit ground
    {
        //int snd_id = 0;
        // TODO -- multiply dvz by base gain to get playable gain for fall range
        if (dvz > HARD_FALL_VELOCITY_THRESHOLD)
            //snd_id =
                Sound::play_2d_sound("hard_fall");
        else
        if (dvz > MEDIUM_FALL_VELOCITY_THRESHOLD)
            //snd_id =
                Sound::play_2d_sound("medium_fall");
        else
        if (dvz > SOFT_FALL_VELOCITY_THRESHOLD)
            //snd_id =
                Sound::play_2d_sound("soft_fall");
        //GS_ASSERT(snd_id >= 0);
    }

    if (!camera_on_ground) return;

    const int n_footsteps = 16;
    int (*footsteps[n_footsteps])(void) =
    {
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_3,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_4,
        
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_2,
        Sound::soft_step_3,
        Sound::soft_step_1,
        Sound::soft_step_4,
        Sound::soft_step_2,
    };

    const int n_perturb_footsteps = 4;
    int (*perturb_footsteps[n_perturb_footsteps])(void) =
    {
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_3,
        Sound::soft_step_4,
    };

    
    #define RANDOM_STEPS 0
    #define RANDOM_CHANGE 1
    #define RANDOM_CHANGE_PER_LOOP 1

    if (total_distance > distance_per_step)
    {
        //play sound
        //printf("play agent sound !!! \n");
        //total_distance -= distance_per_step;
        total_distance = fmodf(total_distance, distance_per_step);
        //Sound::walking();
        #if RANDOM_STEPS
        footsteps[randrange(0,n_footsteps-1)]();
        #else
        static int step = 0;
        step++;
        step %= n_footsteps;
        if (RANDOM_CHANGE && randf() < (((float)RANDOM_CHANGE_PER_LOOP)/((float)n_footsteps)))
            perturb_footsteps[randrange(0,n_perturb_footsteps-1)]();
        else
            footsteps[step]();
        #endif
    }

    #undef RANDOM_STEPS
    #undef RANDOM_CHANGE
    #undef RANDOM_CHANGE_PER_LOOP
}
