#include "interfaces.hpp"

namespace Objects
{

OwnedNone* owned_none = NULL;
//OwnedComponent* owned = NULL;

TeamNone* team_none = NULL;
//TeamComponent* team = NULL;

HealthNone* health_none = NULL;
//HealthComponent health = NULL;

SpatialNone* spatial_none = NULL;
//VerletComponent* spatial_verlet = NULL;
//PositionChangedComponent* spatial_position_changed = NULL;
//PositionMomentumChangedComponent* spatial_position_momentum_changed = NULL;


void init_state_interfaces()
{
    owned_none = new OwnedNone;
    //owned = new OwnedComponent;

    team_none = new TeamNone;
    //team = new TeamComponent;

    health_none = new HealthNone;
    //HealthComponent health = NULL;

    spatial_none = new SpatialNone;
    //spatial_verlet = new VerletComponent;
    //spatial_position_changed = new PositionChangedComponent;
    //spatial_position_momentum_changed = new PositionMomentumChangedComponent;
}

void teardown_state_interfaces()
{
    if (owned_none != NULL) delete owned_none;
    //if (owned != NULL) delete owned;

    if (team_none != NULL) delete team_none;
    //if (team != NULL) delete team;

    if (health_none != NULL) delete health_none;
    //HealthComponent health = NULL;

    if (spatial_none != NULL) delete spatial_none;
    //if (spatial_verlet != NULL) delete spatial_verlet;
    //if (spatial_position_changed != NULL) delete spatial_position_changed;
    //if (spatial_position_momentum_changed != NULL) delete spatial_position_momentum_changed;
}

} // Objects
