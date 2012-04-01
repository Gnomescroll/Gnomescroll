#include "packets.hpp"

#if DC_CLIENT

/* Construction */

// forward declarations
void turret_create(object_create_owner_team_StoC_model* msg);
void spawner_create(object_create_owner_team_index_StoC_model* msg);

inline void object_create_StoC_model::handle()
{
    ObjectPolicyInterface* obj;
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
            obj = ClientState::object_list->create((Object_types)type, x,y,z, 0,0,0);
            break;
            
        default: return;
    }
    if (obj != NULL)
        obj->born();
}

inline void object_create_vel_StoC_model::handle()
{
    ObjectPolicyInterface* obj;
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
            obj = ClientState::object_list->create((Object_types)type, x,y,z, mx,my,mz);
            break;
            
        default: return;
    }
    if (obj != NULL)
        obj->born();
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

/* Destruction */

// forward declarations
void spawner_destroy(Object_types type, int id);
void turret_destroy(Object_types type, int id);

// use privately
static inline void _destroy_object_handler(int type, int id) __attribute((always_inline));
static inline void _destroy_object_handler(int type, int id)
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
        case OBJ_TYPE_LASER_REFILL:
            ClientState::object_list->destroy((Object_types)type, id);
            break;

        case OBJ_TYPE_SPAWNER:
            spawner_destroy((Object_types)type, id);
            break;
        case OBJ_TYPE_TURRET:
            turret_destroy((Object_types)type, id);
            break;

        default: return;
    }    
}

inline void object_destroy_StoC::handle()
{
    _destroy_object_handler(type, id);
}

/* State */

// forward declarations
void turret_state(object_state_StoC_model* msg);
void spawner_state(object_state_StoC_model* msg);

inline void object_state_StoC_model::handle()
{
    switch (type)
    {
        case OBJ_TYPE_TURRET:
            turret_state(this);
            break;
        case OBJ_TYPE_SPAWNER:
            spawner_state(this);
            break;

        default: return;
    }
}

inline void object_state_vel_StoC_model::handle()
{
    switch (type)
    {
        default: break;
    }
}

inline void object_shot_object_StoC::handle()
{
    switch (this->type)
    {
        case OBJ_TYPE_TURRET:
            turret_shot_object(this);
            break;
            
        default:break;
    }
}

/* Actions */

inline void object_picked_up_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL && playerAgent_state.you->id == agent_id)
        Sound::pickup_item();
    _destroy_object_handler(type, id);
}

/* Hitscan */

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
inline void object_create_vel_StoC_model::handle() {}
inline void object_create_owner_team_StoC_model::handle() {}
inline void object_create_owner_team_index_StoC_model::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_picked_up_StoC::handle() {}
inline void object_state_StoC_model::handle() {}
inline void object_state_vel_StoC_model::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
#endif
