#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/objects/components/voxel/component.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/net/packets.hpp>

namespace Monsters {

extern VoxDat slime_vox_dat;


typedef ObjectInterface
< HealthPositionMomentumChangedState, object_create_momentum_angles_StoC, object_state_momentum_angles_StoC > // TODO -- state packet including theta,phi
MonsterInterface;

class Slime: public VoxelComponent, public MonsterInterface
{
    private:
        void load_data(int id);
    public:
    
    //Voxel_model* vox;

    //Object_types type;

    #ifdef DC_SERVER
    int tick_num;
    int network_state_update_interval;
    #endif

    void tick();
    void die();
    void born();
    void update();
    void draw(){/*Empty*/}
    
    void set_state(float x, float y, float z, float vx, float vy, float vz);
    void set_state(float x, float y, float z, float vx, float vy, float vz, float theta, float phi);
    void set_angles(float theta, float phi);

    //void init_vox();
    
    //int take_damage(int dmg);

    explicit Slime(int id);
    Slime(int id, float x, float y, float z, float vx, float vy, float vz);
};

void populate_slimes(int n_max) GNOMESCROLL_API;
}
