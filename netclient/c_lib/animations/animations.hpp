#pragma once

namespace Animations {

void block_crumble(float x, float y, float z, int n, int cube_id);

// pos, incident vector, cube_id, side[3] array returned from ray_cast6
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side);

void grenade_explode(float x, float y, float z);

void slime_melt(float x, float y, float z);
}
