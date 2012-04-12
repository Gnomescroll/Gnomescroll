#pragma once

#include <c_lib/objects/common/net/interfaces.hpp>
#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/interfaces.hpp>

/* Common state initializers */

#define DefaultState Objects::owned_none, Objects::team_none, Objects::health_none, Objects::spatial_none
//#define VerletState Objects::owned_none, Objects::team_none, Objects::health_none, Objects::spatial_verlet
//#define OwnedState Objects::owned, Objects::team_none, Objects::health_none, Objects::spatial_none
//#define OwnedTeamHealthPositionChangedState Objects::owned, Objects::team, Objects::health, Objects::spatial_position_changed
//#define HealthPositionMomentumChangedState Objects::owned_none, Objects::team_none, Objects::health, Objects::spatial_position_momentum_changed
