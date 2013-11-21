/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <entity/constants.hpp>
#include <entity/entity/entity_list.hpp>
#include <entity/entity/filter.hpp>

namespace Components
{
class Component;
}

namespace Entities
{

extern class EntityList* entity_list;
extern class EntityDataList* entity_data;
extern class EntityListFilter* filter;

void init();
void teardown();

void tick();
void update();
void harvest();

size_t get_components_needed(ComponentType type);
Components::Component* get_component_interface_reference(
    EntityType type, ComponentInterfaceType interface);

class Entity* create(EntityType type);
class Entity* create(EntityType type, EntityID id);
void ready(class Entity* entity);
void destroy(class Entity* entity);
void destroy(EntityType type, EntityID id);

class Entity* get(EntityType type, EntityID id);
int count(EntityType type);
bool full(EntityType type);

class Entity* get_all(EntityType type, int& max, int& count);
void destroy_all();

// misc interators. move to another file later
bool point_occupied_by_type(EntityType type, const Vec3i& position);
void damage_entities_within_sphere(const EntityType* types, int n_types,
                                   const Vec3& position, float radius,
                                   int damage, AgentID attacker_id);
void spawn_mobs();

#if DC_SERVER
void send_to_client(ClientID client_id);
#endif

} // Entities
