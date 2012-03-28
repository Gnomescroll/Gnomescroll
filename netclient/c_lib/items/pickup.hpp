#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/entity/entity.hpp>
#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

/* State */

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

/* Behaviour */

template <class Super, typename State>
class DiePickup: public Super
{
    public:
    inline void die(State* state)
    {
        #if DC_SERVER
        if (state->broadcast_death)
        {
            item_picked_up_StoC msg;
            msg.type = state->type;
            msg.id = state->id;
            msg.agent_id = state->picked_up_by;
            msg.broadcast();
        }
        #endif
        Super::die(state);
    }
};

template <class Super, typename State>
class BornPickup: public Super
{
    public:
    inline void born(State* state)
    {
        #if DC_SERVER
        item_create_StoC msg;
        msg.type = state->type;
        msg.id = state->id;
        msg.x = state->vp->p.x;
        msg.y = state->vp->p.y;
        msg.z = state->vp->p.z;
        Vec3 m = state->vp->get_momentum();
        msg.mx = m.x;
        msg.my = m.y;
        msg.mz = m.z;
        msg.broadcast();
        #endif
        Super::born(state);
    }
};

template <class Super, typename State>
class TickPickup: public Super
{
    public:
    inline void tick(State* state)
    {
        #if DC_SERVER
        int agent_id = state->object->nearest_agent_in_range(state->vp->p, state->pickup_radius);
        if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, state->type))
        {   // was picked up, die
            state->object->was_picked_up(agent_id);
            state->ttl = state->ttl_max;
        }
        #endif
        Super::tick(state);
    }
};

/* Composition */

class PickupObject; // forward decl
typedef ObjectStateTemplate<PickupObject> PickupObjectState;

typedef TickParticle < TickPickup < TickTTL < NoTick(PickupObjectState) ,PickupObjectState>,PickupObjectState>,PickupObjectState> ParticleTick;
typedef DrawBillboardSprite < NoDraw(PickupObjectState) ,PickupObjectState> BillboardSpriteDraw;
typedef BornPickup < NoBorn(PickupObjectState) ,PickupObjectState> PickupBorn;
typedef DiePickup < NoDie(PickupObjectState) ,PickupObjectState> PickupDie;

typedef ObjectPolicy <PickupObject, ParticleTick, BillboardSpriteDraw, PickupBorn, PickupDie > PickupObjectParent;
class PickupObject: public PickupObjectParent
{
    public:
    PickupObject(int id, float x, float y, float z, float mx, float my, float mz)
    : PickupObjectParent
    (this)
    {
        this->_state.id = id;
        this->_state.pickup = true;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.create_particle(x,y,z, mx,my,mz);
        this->_state.pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;
        this->_state.texture_scale = DEFAULT_PICKUP_ITEM_TEXTURE_SCALE;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;
    }

    void was_picked_up(const int agent_id);
    int nearest_agent_in_range(const Vec3 p, const float radius);
};

} // ItemDrops
