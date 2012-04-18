#include "packets.hpp"

#if DC_CLIENT

#include <c_lib/sound/sound.hpp>

#include <c_lib/agent/client/player_agent.hpp>

#include <c_lib/items/turret.hpp>
#include <c_lib/items/spawner.hpp>
#include <c_lib/monsters/box.hpp>

/* Construction */

inline void object_create_StoC::handle()
{
    //ObjectPolicyInterface* obj = ClientState::object_list->create((ObjectType)type, (int)subtype, (int)id);
    //if (obj == NULL) return;
    //obj->set_position(x,y,z);
    //obj->born(subtype);   // TODO
}

inline void object_create_momentum_StoC::handle()
{
    //ObjectPolicyInterface* obj = ClientState::object_list->create((ObjectType)type, (int)subtype, (int)id);
    //if (obj == NULL) return;
    //obj->set_position(x,y,z);
    //obj->set_momentum(mx,my,mz);
    //obj->born(subtype);
}

inline void object_create_momentum_angles_StoC::handle()
{
    //ObjectPolicyInterface* obj = ClientState::object_list->create((ObjectType)type, (int)subtype, (int)id);
    //if (obj == NULL) return;
    //obj->set_position(x,y,z);
    //obj->set_angles(theta, phi, 0);
    //obj->born(subtype);
}

inline void object_create_owner_team_StoC::handle()
{
    //ObjectPolicyInterface* obj = ClientState::object_list->create((ObjectType)type, (int)subtype, (int)id);
    //if (obj == NULL) return;
    //obj->set_position(x, y, z);
    //obj->set_team(team);
    //obj->set_owner(owner);
    //obj->born(subtype);
    //system_message->object_created(obj);
}

inline void object_create_owner_team_index_StoC::handle()
{
    //ObjectPolicyInterface* obj = ClientState::object_list->create((ObjectType)type, (int)subtype, (int)id);
    //if (obj == NULL) return;
    //obj->set_position(x, y, z);
    //obj->set_team(team);
    //obj->set_owner(owner);
    //obj->set_team_index((unsigned int)team_index);
    //obj->born(subtype);
    //system_message->object_created(obj);
}

/* State */

inline void object_state_StoC::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((ObjectType)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
}

inline void object_state_momentum_StoC::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((ObjectType)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_momentum(mx,my,mz);
}

inline void object_state_momentum_angles_StoC::handle()
{
    ObjectPolicyInterface* obj = STATE::object_list->get((ObjectType)type, id);
    if (obj == NULL) return;
    obj->set_position(x,y,z);
    obj->set_momentum(mx,my,mz);
    obj->set_angles(theta, phi, 0);
}

/* Destruction */

// use privately
static inline void _destroy_object_handler(ObjectType type, int id) __attribute((always_inline));
static inline void _destroy_object_handler(ObjectType type, int id)
{
    ClientState::object_list->destroy(type, id);
}

inline void object_destroy_StoC::handle()
{
    _destroy_object_handler((ObjectType)type, id);
}

/* Actions */

inline void object_picked_up_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL && playerAgent_state.you->id == agent_id)
        Sound::pickup_item();
    _destroy_object_handler((ObjectType)type, id);
}

/* Hitscan */

inline void object_shot_object_StoC::handle()
{
    switch (this->type)
    {
        case OBJECT_MONSTER_BOX:
            Monsters::box_shot_object(this);    // TODO -- replace handlers
            break;
            
        case OBJECT_TURRET:
            turret_shot_object(this);
            break;
            
        default:break;
    }
}

inline void object_shot_terrain_StoC::handle()
{
    switch (this->type)
    {
        case OBJECT_TURRET:
            turret_shot_terrain(this);
            break;
            
        default:break;
    }
}

inline void object_shot_nothing_StoC::handle()
{
    switch (this->type)
    {
        case OBJECT_TURRET:
            turret_shot_nothing(this);
            break;

        default:break;
    }
}

#endif

#if DC_SERVER
inline void object_create_StoC::handle() {}
inline void object_create_momentum_StoC::handle() {}
inline void object_create_momentum_angles_StoC::handle() {}
inline void object_create_owner_team_StoC::handle() {}
inline void object_create_owner_team_index_StoC::handle() {}
inline void object_destroy_StoC::handle() {}
inline void object_picked_up_StoC::handle() {}
inline void object_state_StoC::handle() {}
inline void object_state_momentum_StoC::handle() {}
inline void object_state_momentum_angles_StoC::handle() {}
inline void object_shot_object_StoC::handle() {}
inline void object_shot_terrain_StoC::handle() {}
inline void object_shot_nothing_StoC::handle() {}
#endif
