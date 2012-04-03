#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/entity/policy.hpp>
#include <c_lib/lists/lists.hpp>

//typedef ObjectStateLayer<OwnedComponent, TeamComponent> OwnedState;     // all components parameterized here
typedef ObjectStateLayer<OwnedComponent, TeamDefault> OwnedState;     // all components parameterized here
typedef ObjectStateLayer<OwnedDefault, TeamDefault> DefaultState;

