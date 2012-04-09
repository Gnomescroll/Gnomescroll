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
< HealthPositionMomentumChangedState, object_create_momentum_angles_StoC, object_state_momentum_angles_StoC >
MonsterInterface;

class Slime: public VoxelComponent, public MonsterInterface
{
    private:
        void load_data(int id);
    public:
    
    #if DC_SERVER
    int tick_num;
    int network_state_update_interval;
    #endif

    void tick();
    void die();
    void born();
    void update();
    void draw(){/*Empty*/}
    
    explicit Slime(int id);
};

void populate_slimes(int n_max) GNOMESCROLL_API;
}
