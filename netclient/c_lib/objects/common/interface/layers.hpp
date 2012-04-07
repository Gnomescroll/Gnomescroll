#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/component/component.hpp>

#include <c_lib/objects/components/include.hpp>

/* Parameterized template state layers */

typedef ObjectStateLayer<OwnedDefault, TeamDefault, HealthDefault, SpatialDefault> DefaultState;
typedef ObjectStateLayer<OwnedDefault, TeamDefault, HealthDefault, VerletComponent> VerletState;
//typedef ObjectStateLayer<OwnedDefault, TeamComponent, HealthDefault> TeamState;
typedef ObjectStateLayer<OwnedComponent, TeamDefault, HealthDefault, SpatialDefault> OwnedState;
//typedef ObjectStateLayer<OwnedComponent, TeamComponent, HealthDefault> OwnedTeamState;

//typedef ObjectStateLayer<OwnedDefault, TeamDefault, HealthComponent> HealthState;
//typedef ObjectStateLayer<OwnedDefault, TeamComponent, HealthComponent> TeamHealthState;
//typedef ObjectStateLayer<OwnedComponent, TeamDefault, HealthComponent> OwnedHealthState;
typedef ObjectStateLayer<OwnedComponent, TeamComponent, HealthComponent, PositionVoxelComponent> OwnedTeamHealthPositionVoxelState;



/* Placeholder typedef
 *
 *  Inherit this to provide an object with polymorphic abilities
 * */

typedef DummyObjectInterface< DummyNetworkInterface < DefaultState > > ObjectInterfacePlaceholder;
