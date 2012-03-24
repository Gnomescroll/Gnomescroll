#include "refills.hpp"

#include <c_lib/common/enum_types.hpp>

namespace ItemDrops
{

/* Grenade Refills */


void GrenadeRefill::init()
{
    this->type = OBJ_TYPE_GRENADE_REFILL;
    this->texture_index = GRENADE_REFILL_TEXTURE_ID;
    this->scale = GRENADE_REFILL_TEXTURE_SCALE;
    this->ttl_max = GRENADE_REFILL_TTL;
    this->damp = GRENADE_REFILL_DAMP;
}

GrenadeRefill::GrenadeRefill(int id, float x, float y, float z, float mx, float my, float mz)
: PickupItem(id, x,y,z,mx,my,mz)
{
    this->init();
    this->born();
}

/* Laser Refills */

const int LASER_REFILL_TEXTURE_ID = 242;
const float LASER_REFILL_TEXTURE_SCALE = 0.5f;
const float LASER_REFILL_MASS = 1.0f;
const int LASER_REFILL_TTL = 30 * 12;   // 12 seconds
const float LASER_REFILL_DAMP = 0.4f;

void LaserRefill::init()
{
    this->type = OBJ_TYPE_LASER_REFILL;
    this->texture_index = LASER_REFILL_TEXTURE_ID;
    this->scale = LASER_REFILL_TEXTURE_SCALE;
    this->ttl_max = LASER_REFILL_TTL;
    this->damp = LASER_REFILL_DAMP;
}

LaserRefill::LaserRefill(int id, float x, float y, float z, float mx, float my, float mz)
: PickupItem(id, x,y,z,mx,my,mz)
{
    this->init();
    this->born();
}

}   // ItemDrops
