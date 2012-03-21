#pragma once

#include <c_lib/particles/textures.hpp>
#include <c_lib/particles/particles.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/template/object_list.hpp>

// encapsulates all information needed for any object
class ObjectState
{
    public:
    int id;
    bool blow_up_on_death;
    bool pickup;
    
    ObjectState()
    : id(-1), blow_up_on_death(false), pickup(false)
    {}
};

template <class Super>
class DieMixin: public Super
{
    protected:
        ObjectState* state;
    public:
        void die() { Super::die(this->state); }

    DieMixin<Super>(ObjectState* state)
    : state(state) {}
};

template <class Super>
class BlowupDie: public Super
{
    public:
        void die(ObjectState* state) { if (state->blow_up_on_death) printf("blowup die\n"); Super::die(state); }
};

template <class Super>
class PickupDie: public Super
{
    public:
        void die(ObjectState* state) { printf("%d\n", state->id); if (state->pickup) printf("pickup die\n"); Super::die(state); }
};

class AnchorDie
{
    public:
        void die(ObjectState* state) 
        { 
            printf("Reached anchor, id=%d\n", state->id);
        }
};

typedef DieMixin < BlowupDie < PickupDie < AnchorDie > > > MergedDieSuper;
class MergedDie: public MergedDieSuper
{
    public:
    explicit MergedDie(ObjectState* state)
    : MergedDieSuper(state) {}
};

class MainObject: public MergedDie
{
    public:
        ObjectState state;
        
    MainObject()
    : MergedDie(&this->state)
    {}
};

// alternate form
// eliminates the MergedDie and constructor passing
// but will have to define this die() in every object
// desirable?
typedef PickupDie < BlowupDie < AnchorDie > > MainObject2Super;
class MainObject2: public MainObject2Super
{
    public:
        ObjectState state;
        void die() { MainObject2Super::die(&this->state); }
};

// using a policy template, this is the better way, because we will only need to define the die() wrapper once
// AnchorDie can stub out the functionality

class NoTick
{ void tick() {} };
class NoDraw
{ void draw() {} };
class NoDie
{ void die() {} };

template <class Tick = NoTick, class Draw = NoDraw, class Die = NoDie>
class AnObject
{
    void tick() { Tick::tick(); }
    void draw() { Draw::draw(); }
    void die() { Die::die(); }
};

void init_die()
{
    ObjectState _state;
    MergedDie* m = new MergedDie(&_state);
    m->die();
    printf("should have died\n");
    printf("\n\n");
    
    MainObject n;
    n.state.id = 666;
    n.die();
    
    n.state.pickup = true;
    n.die();
    
    n.state.blow_up_on_death = true;
    n.die();
    
    printf("\n\n");
    
    MainObject2 p;
    p.state.blow_up_on_death = true;
    p.state.pickup = true;
    p.state.id = 777;
    p.die();
    printf("\n\n");
}


/*
 *  Don't instantiate any of these classes directly
 *
 * See c_lib/items/grenades.*pp for an example of how to use
 *
 */

namespace ItemDrops
{

class Pickup
{
    private:
        const float radius;
    public:
        int nearest_agent_in_range(Vec3 p);
        explicit Pickup(const float radius) : radius(radius) {}
};

class PickupItem: public Particle, public BillboardSprite, public Pickup
{
    private:
        bool broadcast_death;
        int picked_up_by;
    public:
        virtual void init();
        void tick();
        void was_picked_up(int agent_id);

        void born();
        void die();
        
        explicit PickupItem(int id);
        PickupItem(int id, float x, float y, float z, float mx, float my, float mz);
};

template<class Obj, int obj_max>
class PickupItem_list: public Object_list<Obj, obj_max>
{
    public:
        void tick();
        void draw();
        PickupItem_list() {}
};


} // ItemDrops
