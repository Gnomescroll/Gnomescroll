#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/template/object_list.hpp>

#include <c_lib/items/pickup.hpp>
#include <c_lib/items/refills.hpp>

namespace ItemDrops
{

struct ObjectData
{
    Object_types type;
    
};

// encapsulates all information needed for any object
class ObjectState
{
    public:
    int id;
    Object_types type;
    bool blow_up_on_death;
    bool pickup;
    float pickup_radius;
    int ttl;
    int ttl_max;
    float damp;
    VerletParticle* vp;
    float mass;
    float texture_scale;
    int texture_index;
    bool broadcast_death;
    int picked_up_by;
    
    ObjectState()
    : id(-1), type(OBJ_TYPE_NONE), blow_up_on_death(false), pickup(false),
    pickup_radius(1.0f), ttl(0), ttl_max(1), damp(1.0f), vp(NULL),
    mass(1.0f), texture_scale(1.0f), texture_index(0),
    broadcast_death(false), picked_up_by(-1)
    {}

    ~ObjectState()
    {
        if (this->vp != NULL)
            delete this->vp;
    }

    // TODO: setters
};

// encapsulates all information needed for any object
template <class Subclass>
class ObjectStateTemplate: public ObjectState
{
    public:
    Subclass* object;
    ObjectStateTemplate<Subclass>() {}
};

template <class Super, typename State>
class DrawCombiner: public Super
{
    public:
        void draw(State* state)
        {
            Super::draw(state);
        }
};

template <typename State>
class DrawAnchor
{
    public:
        void draw(State* state) 
        { 
            //printf("Reached draw anchor, id=%d\n", state->id);
        }
};

template <class Super, typename State>
class TickCombiner: public Super
{
    public:
        void tick(State* state)
        {
            Super::tick(state);
        }
};

template <typename State>
class TickAnchor
{
    public:
        void tick(State* state) 
        { 
            //printf("Reached tick anchor, id=%d\n", state->id);
        }
};

template <class Super, typename State>
class DieCombiner: public Super
{
    public:
        void die(State* state)
        {
            Super::die(state);
        }
};

template <typename State>
class DieAnchor
{
    public:
        void die(State* state) 
        { 
            printf("Reached die anchor, id=%d\n", state->id);
        }
};

template <class Super, typename State>
class DieBlowup: public Super
{
    public:
        void die(State* state)
        {
            //if (state->blow_up_on_death)
                //printf("blowup die\n");
                //printf("\tcalling tick() of subclass object\n\t");
            state->object->tick();
            Super::die(state);
        }
};

template <class Super, typename State>
class DiePickup: public Super
{
    public:
        void die(State* state)
        {
            #if DC_SERVER
            if (state->broadcast_death)
            {
                //item_destroy_StoC msg;
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

#define NoDie(STATE) DieAnchor<STATE>
#define NoTick(STATE) TickAnchor<STATE>
#define NoDraw(STATE) DrawAnchor<STATE>

class ObjectPolicyInterface
{
    public:
        virtual void die() = 0;
        virtual void tick() = 0;
        virtual void draw() = 0;
        virtual ObjectState* state() = 0;
};

template
<
    class Wrapper,
    class DieSuper,
    class TickSuper,
    class DrawSuper
>
class ObjectPolicy:
public DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >,
public TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >,
public DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >,
public ObjectPolicyInterface
{
    public:
        ObjectStateTemplate<Wrapper> _state;
    
    void die() { DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >::die(&this->_state); }
    void tick() { TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >::tick(&this->_state); }
    void draw() { DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >::draw(&this->_state); }
    ObjectState* state() { return &this->_state; }

    ObjectPolicy<Wrapper, DieSuper, TickSuper, DrawSuper>(Wrapper* wrapper)
    {
        _state.object = wrapper;
    }
};


///* BEGIN TYPICAL CLASS DEFINITION */

//class Object; // forward decl
//typedef ObjectStateTemplate<Object> ObjectState;
//typedef DieBlowup < DiePickup < NoDie(ObjectState) ,ObjectState>,ObjectState> Die_BlowupPickup;

//class Object: public ObjectPolicy <Object, Die_BlowupPickup, NoTick(ObjectState), NoDraw(ObjectState) >
//{
    //public:
    //Object()
    //: ObjectPolicy<Object, Die_BlowupPickup, NoTick(ObjectState), NoDraw(ObjectState) >
    //(this)
    //{}
//};

//// this class will have a default draw and tick ("none")
//// and 2 combined die() methods -- blowup and pickup, which will be called in that order

///* END */

//void init_die()
//{
    //printf("\n\n");
    
    //Object n;
    //n.state()->id = 666;
    //n.die();

    //printf("\n\n");

    //n.state()->pickup = true;
    //n.die();

    //printf("\n\n");
    
    //n.state()->blow_up_on_death = true;
    //n.die();

    //printf("\n\n");

    //ObjectPolicyInterface* m = new Object;

    //m->state()->id = 666;
    //m->die();
//}


template <class Super, typename State>
class TickParticle: public Super
{
    public:
    void tick(State* state)
    {
        Verlet::bounce(state->vp, state->damp);
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickTTL: public Super
{
    public:
    void tick(State* state)
    {
        state->ttl++;
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickPickup: public Super
{
    public:
    void tick(State* state);
};

template <class Super, typename State>
class DrawBillboardSprite: public Super
{
    public:
    void draw(State* state);
};

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
        this->_state.vp = new VerletParticle(x,y,z, mx,my,mz, DEFAULT_PICKUP_ITEM_MASS); //TODO mass
        this->_state.pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;
        this->_state.texture_scale = DEFAULT_PICKUP_ITEM_TEXTURE_SCALE;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;
    }

    void was_picked_up(const int agent_id)
    {
        this->_state.broadcast_death = true;
        this->_state.picked_up_by = agent_id;
    }

    int nearest_agent_in_range(const Vec3 p, const float radius);
    void born();
};


//ObjectPolicyInterface* create_object_type(Object_types type, ObjectPolicyInterface* obj)
//{
    //switch (type)
    //{
        //case OBJ_TYPE_GRENADE_REFILL:
            //return create_grenade_refill(obj);
            //break;
        //case OBJ_TYPE_LASER_REFILL:
            //return create_laser_refill(obj);
            //break;
        //default: break;
    //}
    //return NULL;
//}

class GameObject_list: public Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>
{
    public:
        const char* name() { return "GameObject"; }
        void tick()
        {
            if (this->num == 0) return;
            for (int i=0; i<this->n_max; i++)
                if (this->a[i] != NULL)
                {
                    this->a[i]->tick();
                    if (this->a[i]->state()->ttl >= this->a[i]->state()->ttl_max)
                    {
                        this->a[i]->die();
                        this->destroy(this->a[i]->state()->id);
                    }
                }
        }
        
        void draw()
        {
            if (this->num == 0) return;
            for (int i=0; i<this->n_max; i++)
                if (this->a[i] != NULL)
                    this->a[i]->draw();
        }

        ObjectPolicyInterface* create(float x, float y, float z, float mx, float my, float mz, ObjectData type)
        {
            int id = this->get_free_id();
            if (id < 0) return NULL;
            this->num++;
            this->id_c = id+1;

            PickupObject* obj = new PickupObject(id, x,y,z, mx,my,mz);
            this->a[id] = obj;

            obj->state()->type = type;
            obj->state()->texture_index = GRENADE_REFILL_TEXTURE_ID;
            obj->state()->texture_scale = GRENADE_REFILL_TEXTURE_SCALE;
            obj->state()->mass = GRENADE_REFILL_MASS;
            obj->state()->ttl_max = GRENADE_REFILL_TTL;
            obj->state()->damp = GRENADE_REFILL_DAMP;
            obj->born();

            return obj;
        }

        void destroy(int id)
        {
            ObjectPolicyInterface* obj = this->a[id];
            if (obj == NULL) return;
            obj->die();
            Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>::destroy(id);
        }
        
        GameObject_list() { print(); }
};

// TODO: restructure the list creation to adapt based on type

} // ItemDrops
