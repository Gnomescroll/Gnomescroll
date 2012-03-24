#pragma once

#include <c_lib/items/pickup.hpp>

namespace ItemDrops
{

/* Grenade refill */

const int GRENADE_REFILL_TEXTURE_ID = 240;
const float GRENADE_REFILL_TEXTURE_SCALE = 0.5f;
const float GRENADE_REFILL_MASS = 1.0f;
const int GRENADE_REFILL_TTL = 30 * 12;   // 12 seconds
const float GRENADE_REFILL_DAMP = 0.4f;

/* Laser refill */

const int LASER_REFILL_TEXTURE_ID = 242;
const float LASER_REFILL_TEXTURE_SCALE = 0.5f;
const float LASER_REFILL_MASS = 1.0f;
const int LASER_REFILL_TTL = 30 * 12;   // 12 seconds
const float LASER_REFILL_DAMP = 0.4f;

} // ItemDrops
