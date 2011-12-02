#pragma once

namespace Animations {

void block_crumble(float x, float y, float z, int n, int cube_id);
void grenade_explode(float x, float y, float z);


}

#include <c_lib/particles/object_lib.hpp>
#include <c_lib/common/random.h>
#include <c_lib/state/client_state.hpp>

