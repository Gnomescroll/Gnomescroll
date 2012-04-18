#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/include.hpp>
#include <c_lib/entity/network/packets.hpp>
#include <c_lib/entity/network/interfaces.hpp>

namespace Monsters {

extern VoxDat slime_vox_dat;

class Slime:
    public VoxelComponent, 
    public RateLimitedStateBroadcastComponent,
    #if DC_CLIENT
    public AnimationVoxelComponent,
    #endif
    public ObjectStateLayer
{
    public:
        HealthComponent health;
        PositionMomentumChangedComponent spatial;
    
    void tick();
    void die();
    void born(int subtype);
    void update();
    
    explicit Slime(int id);
};

void populate_slimes(int n_max) GNOMESCROLL_API;
}
