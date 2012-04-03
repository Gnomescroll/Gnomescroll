#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/components/components.hpp>

/* Parameterized template state layers */

typedef ObjectStateLayer<OwnedDefault, TeamDefault> DefaultState;
typedef ObjectStateLayer<OwnedDefault, TeamComponent> TeamState;
typedef ObjectStateLayer<OwnedComponent, TeamDefault> OwnedState;
typedef ObjectStateLayer<OwnedComponent, TeamComponent> OwnedTeamState;

