#pragma once

#include <entity/constants.hpp>
#include <entity/object/object_list.hpp>
#include <entity/object/filter.hpp>

namespace Objects
{

extern class ObjectList* object_list;
extern class ObjectDataList* object_data;
extern class ObjectListFilter* filter;

void init();
void teardown();

void tick();
void update();
void harvest();

class Component* add_component_to_object(class Object* object, ComponentType type);
void release_object_components(Object* object);

class Object* create(ObjectType type);
class Object* create(ObjectType type, int id);
void ready_switch(class Object* object);
void destroy_switch(class Object* object);
void destroy_switch(ObjectType type, int id);

class Object* get_object(ObjectType type, int id);
int count(ObjectType type);
bool full(ObjectType type);
void send_to_client(ObjectType type, ClientID client_id);

// misc interators. move to another file later
bool point_occupied_by_type(ObjectType type, int x, int y, int z);
void damage_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage);
void spawn_mobs();

#if DC_SERVER
// TODO -- generalize/remove
void send_object_state_machines(const ObjectType type, const ClientID client_id);
void send_to_client(ClientID client_id);
#endif

} // Objects
