#include "packets.hpp"

#if DC_CLIENT

//#include <c_lib/sound/sound.hpp>

#include <c_lib/agent/client/player_agent.hpp>

//#include <c_lib/items/turret.hpp>
//#include <c_lib/items/spawner.hpp>
//#include <c_lib/monsters/box.hpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/objects.hpp>

/* Construction */

inline void object_create_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }
    Objects::ready(obj);
}

inline void object_create_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
    }
    Objects::ready(obj);
}

inline void object_create_momentum_angles_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
    Objects::ready(obj);
}

inline void object_create_owner_team_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::TeamComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;

    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }

    OwnerComponent* owner_component = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner_component != NULL)
    {
        owner_component->set_owner(owner);
    }
    
    TeamComponent* team_component = (TeamComponent*)obj->get_component_interface(COMPONENT_INTERFACE_TEAM);
    if (team_component != NULL)
    {
        team_component->set_team(team);
    }
    
    Objects::ready(obj);
}

inline void object_create_owner_team_index_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;
    using Components::OwnerComponent;
    using Components::IndexedTeamComponent;

    Object* obj = Objects::create((ObjectType)type, id);
    if (obj == NULL) return;

    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }

    OwnerComponent* owner_component = (OwnerComponent*)obj->get_component_interface(COMPONENT_INTERFACE_OWNER);
    if (owner_component != NULL)
    {
        owner_component->set_owner(owner);
    }
    
    IndexedTeamComponent* team_component = (IndexedTeamComponent*)obj->get_component(COMPONENT_INDEXED_TEAM);
    if (team_component != NULL)
    {
        team_component->set_team(team);
        team_component->set_team_index(team_index);
    }
    
    Objects::ready(obj);
}

/* State */

inline void object_state_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
    }
}

inline void object_state_momentum_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
    }
}

inline void object_state_momentum_angles_StoC::handle()
{
    using Objects::Object;
    using Components::PhysicsComponent;

    Object* obj = Objects::get((ObjectType)type, id);
    if (obj == NULL) return;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    if (physics != NULL)
    {
        physics->set_position(vec3_init(x,y,z));
        physics->set_momentum(vec3_init(mx,my,mz));
        physics->set_angles(vec3_init(theta, phi, 0));
    }
}

/* Destruction */

inline void object_destroy_StoC::handle()
{
    Objects::destroy((ObjectType)type, id);
}

/* Actions */

inline void object_picked_up_StoC::handle()
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL && playerAgent_state.you->id == agent_id)
        Sound::pickup_item();
    Objects::destroy((ObjectType)type, id);
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
