#include "packets.hpp"

#if DC_CLIENT

#include <c_lib/sound/sound.hpp>

#include <c_lib/agent/client/player_agent.hpp>

#include <c_lib/items/turret.hpp>
#include <c_lib/items/spawner.hpp>
#include <c_lib/monsters/box.hpp>

/* Construction */

inline void object_create_StoC_model::handle()
{
    ObjectPolicyInterface* obj = ClientState::object_list->create((Object_types)type, (int)id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->born(subtype);   // TODO
}

inline void object_create_momentum_StoC_model::handle()
{
    ObjectPolicyInterface* obj = ClientState::object_list->create((Object_types)type, (int)id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_momentum(mx,my,mz);
    obj->born(subtype);
}

inline void object_create_momentum_angles_StoC_model::handle()
{
    ObjectPolicyInterface* obj = ClientState::object_list->create((Object_types)type, (int)id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_angles(theta, phi, 0);
    obj->born(subtype);
}

inline void object_create_owner_team_StoC_model::handle()
{
    ObjectPolicyInterface* obj = ClientState::object_list->create((Object_types)type, (int)id);
    if (obj == NULL) return;
    obj->set_position(x, y, z);
    obj->set_team(team);
    obj->set_owner(owner);
    obj->born(subtype);
    system_message->object_created(obj);
}

inline void object_create_owner_team_index_StoC_model::handle()
{
    ObjectPolicyInterface* obj = ClientState::object_list->create((Object_types)type, (int)id);
    if (obj == NULL) return;
    obj->set_position(x, y, z);
    obj->set_team(team);
    obj->set_owner(owner);
    obj->set_team_index((unsigned int)team_index);
    obj->born(subtype);
    system_message->object_created(obj);
}

/* State */

inline void object_state_StoC_model::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((Object_types)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
}

inline void object_state_momentum_StoC_model::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((Object_types)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_momentum(mx,my,mz);
}

inline void object_state_momentum_angles_StoC_model::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((Object_types)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_momentum(mx,my,mz);
    obj->set_angles(theta, phi, 0);
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
            Monsters::box_shot_object(this);    // TODO -- replace handlers
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
