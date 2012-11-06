#pragma once

#include <entity/constants.hpp>
#include <entity/object/object_list.hpp>
#include <entity/object/filter.hpp>

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

class Component* add_component_to_object(class Entity* object, ComponentType type);
void release_object_components(Entity* object);

class Entity* create(EntityType type);
class Entity* create(EntityType type, int id);
void ready_switch(class Entity* object);
void destroy_switch(class Entity* object);
void destroy_switch(EntityType type, int id);

class Entity* get_object(EntityType type, int id);
int count(EntityType type);
bool full(EntityType type);
void send_to_client(EntityType type, ClientID client_id);

int get_all(const EntityType type, class Entity**& entities, char*& used);
void destroy_all();

// misc interators. move to another file later
bool point_occupied_by_type(EntityType type, int x, int y, int z);
void damage_objects_within_sphere(const EntityType* types, int n_types, Vec3 position, float radius, int damage);
void spawn_mobs();

#if DC_SERVER
void send_to_client(ClientID client_id);
// TODO -- generalize/remove
void send_object_state_machines(const EntityType type, const ClientID client_id);
#endif

} // Entities
