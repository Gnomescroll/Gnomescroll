#include "grenades.hpp"

#include <c_lib/common/enum_types.hpp>

namespace ItemDrops
{

const int GRENADES_TEXTURE_ID = 240;
const float GRENADES_TEXTURE_SCALE = 0.5f;
const float GRENADES_MASS = 1.0f;
const int GRENADES_TTL = 30 * 12;   // 12 seconds
const float GRENADES_DAMP = 0.1f;

void Grenades::init()
{
    this->type = OBJ_TYPE_GRENADE_DROP;
    this->texture_index = GRENADES_TEXTURE_ID;
    this->scale = GRENADES_TEXTURE_SCALE;
    this->ttl_max = GRENADES_TTL;
    this->damp = GRENADES_DAMP;
}

Grenades::Grenades(int id, float x, float y, float z, float mx, float my, float mz)
: PickupItem(id, x,y,z,mx,my,mz)
{
    this->init();
    this->born();
}

}   // ItemDrops
