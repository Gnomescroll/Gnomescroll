#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/items/pickup2.hpp>

namespace ItemDrops
{

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

class PickupObject; // forward decl
typedef ObjectStateTemplate<PickupObject> PickupObjectState;
typedef DiePickup < NoDie(PickupObjectState) ,PickupObjectState> PickupDie;
typedef TickParticle < TickPickup < TickTTL < NoTick(PickupObjectState) ,PickupObjectState>,PickupObjectState>,PickupObjectState> ParticleTick;
typedef DrawBillboardSprite < NoDraw(PickupObjectState) ,PickupObjectState> BillboardSpriteDraw;

typedef ObjectPolicy <PickupObject, PickupDie, ParticleTick, BillboardSpriteDraw > PickupObjectParent;
class PickupObject: public PickupObjectParent
{
    public:
    PickupObject(int id, float x, float y, float z, float mx, float my, float mz)
    : PickupObjectParent
    (this)
    {
        this->_state.id = id;
        this->_state.pickup = true;
        this->_state.vp = new VerletParticle(x,y,z, mx,my,mz, DEFAULT_PICKUP_ITEM_MASS); //TODO mass; move verlet instantiation out of here
        this->_state.pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;
        this->_state.texture_scale = DEFAULT_PICKUP_ITEM_TEXTURE_SCALE;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;
    }

    void was_picked_up(const int agent_id);
    int nearest_agent_in_range(const Vec3 p, const float radius);
    void born();
};

} // ItemDrops
