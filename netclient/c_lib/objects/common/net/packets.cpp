#include "packets.hpp"

#if DC_CLIENT

#include <c_lib/sound/sound.hpp>

#include <c_lib/agent/client/player_agent.hpp>

#include <c_lib/items/turret.hpp>
#include <c_lib/items/spawner.hpp>
#include <c_lib/monsters/box.hpp>

/* Construction */

// forward declarations
void turret_create(object_create_owner_team_StoC_model* msg);
void spawner_create(object_create_owner_team_index_StoC_model* msg);

inline void object_create_StoC_model::handle()
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_MEAT:
        case OBJ_TYPE_MALACHITE:
        case OBJ_TYPE_RUBY:
        case OBJ_TYPE_TURQUOISE:
        case OBJ_TYPE_SILVER:
        case OBJ_TYPE_AMETHYST:
        case OBJ_TYPE_JADE:
        case OBJ_TYPE_ONYX:
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
        case OBJ_TYPE_DIRT:
        case OBJ_TYPE_STONE:
        case OBJ_TYPE_MONSTER_SPAWNER:
            obj = ClientState::object_list->create((Object_types)type, (int)id);
            break;
            
        default: return;
    }
    if (obj != NULL)
    {
        obj->set_position(x,y,z);
        obj->born(0);   // TODO
    }
}

inline void object_create_momentum_StoC_model::handle()
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_MEAT:
        case OBJ_TYPE_MALACHITE:
        case OBJ_TYPE_RUBY:
        case OBJ_TYPE_TURQUOISE:
        case OBJ_TYPE_SILVER:
        case OBJ_TYPE_AMETHYST:
        case OBJ_TYPE_JADE:
        case OBJ_TYPE_ONYX:
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
        case OBJ_TYPE_DIRT:
        case OBJ_TYPE_STONE:
            obj = ClientState::object_list->create((Object_types)type, (int)id);
            break;
            
        default: return;
    }
    if (obj != NULL)
    {
        obj->set_position(x,y,z);
        obj->set_momentum(mx,my,mz);
        obj->born(0);   // TODO
    }
}

inline void object_create_momentum_angles_StoC_model::handle()
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_SLIME:
        case OBJ_TYPE_MONSTER_BOX:
            obj = ClientState::object_list->create((Object_types)type, (int)id);
            break;
        default: return;
    }
    if (obj != NULL)
    {
        obj->set_position(x,y,z);
        obj->set_angles(theta, phi, 0);
        obj->born(subtype);   // TODO
    }
}

inline void object_create_owner_team_StoC_model::handle()
{
    switch (type)
    {
        case OBJ_TYPE_TURRET:
            turret_create(this);
            break;
        default: return;
    }
}

inline void object_create_owner_team_index_StoC_model::handle()
{
    switch (type)
    {
        case OBJ_TYPE_SPAWNER:
            spawner_create(this);
            break;
        default: return;
    }
}

/* State */

inline void object_state_StoC_model::handle()
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_TURRET:
        case OBJ_TYPE_SPAWNER:
        case OBJ_TYPE_MONSTER_SPAWNER:
            obj = STATE::object_list->get((Object_types)type, id);
            break;

        default: return;
    }
    if (obj != NULL)
        obj->set_position(x,y,z);
}

inline void object_state_momentum_StoC_model::handle()
{
    switch (type)
    {
        default: break;
    }
}

inline void object_state_momentum_angles_StoC_model::handle()
{
    ObjectPolicyInterface* obj = NULL;
    switch (type)
    {
        case OBJ_TYPE_SLIME:
        case OBJ_TYPE_MONSTER_BOX:
            obj = STATE::object_list->get((Object_types)type, id);
            break;
            
        default: break;
    }
    if (obj != NULL)
    {
        obj->set_position(x,y,z);
        obj->set_momentum(mx,my,mz);
        obj->set_angles(theta, phi, 0);
    }
}

/* Destruction */

// use privately
static inline void _destroy_object_handler(Object_types type, int id) __attribute((always_inline));
static inline void _destroy_object_handler(Object_types type, int id)
{
    ClientState::object_list->destroy(type, id);
}

inline void object_destroy_StoC::handle()
{
    _destroy_object_handler((Object_types)type, id);
}

/* Actions */

inline void object_picked_up_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL && playerAgent_state.you->id == agent_id)
        Sound::pickup_item();
    _destroy_object_handler((Object_types)type, id);
}

/* Hitscan */

inline void object_shot_object_StoC::handle()
{
    switch (this->type)
    {
        case OBJ_TYPE_MONSTER_BOX:
            Monsters::box_shot_object(this);
            break;
            
        case OBJ_TYPE_TURRET:
            turret_shot_object(this);
            break;
            
        default:break;
    }
}

inline void object_shot_terrain_StoC::handle()
{
    switch (this->type)
    {
        case OBJ_TYPE_TURRET:
            turret_shot_terrain(this);
            break;
            
        default:break;
    }
}

inline void object_shot_nothing_StoC::handle()
{
    switch (this->type)
    {
        case OBJ_TYPE_TURRET:
            turret_shot_nothing(this);
            break;

        default:break;
    }
}

#endif

#if DC_SERVER
inline void object_create_StoC_model::handle() {}
inline void object_create_momentum_StoC_model::handle() {}
inline void object_create_momentum_angles_StoC_model::handle() {}
inline void object_create_owner_team_StoC_model::handle() {}
inline void object_create_owner_team_index_StoC_model::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_picked_up_StoC::handle() {}
inline void object_state_StoC_model::handle() {}
inline void object_state_momentum_StoC_model::handle() {}
inline void object_state_momentum_angles_StoC_model::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
#endif
