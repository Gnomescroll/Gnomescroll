#pragma once

#include <c_lib/objects/common/interface/state.hpp>
#include <c_lib/objects/common/component/component.hpp>
#include <c_lib/objects/components/include.hpp>

namespace Objects
{

extern OwnedNone* owned_none;
//extern OwnedComponent* owned;

extern TeamNone* team_none;
//extern TeamComponent* team;

extern HealthNone* health_none;
//extern HealthComponent health;

extern SpatialNone* spatial_none;
//extern VerletComponent* spatial_verlet;
//extern PositionChangedComponent* spatial_position_changed;
//extern PositionMomentumChangedComponent* spatial_position_momentum_changed;

void init_state_interfaces();
void teardown_state_interfaces();

} // Objects
