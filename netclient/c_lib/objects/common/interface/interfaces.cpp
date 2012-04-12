#include "interfaces.hpp"

/* Reusable null object state interfaces */
// Use these for objects with no behaviour relating to a corresponding component

namespace Objects
{

OwnedNone* owned_none = NULL;
TeamNone* team_none = NULL;
HealthNone* health_none = NULL;
SpatialNone* spatial_none = NULL;

void init_state_interfaces()
{
    owned_none = new OwnedNone;
    team_none = new TeamNone;
    health_none = new HealthNone;
    spatial_none = new SpatialNone;
}

void teardown_state_interfaces()
{
    if (owned_none != NULL) delete owned_none;
    if (team_none != NULL) delete team_none;
    if (health_none != NULL) delete health_none;
    if (spatial_none != NULL) delete spatial_none;
}

} // Objects
