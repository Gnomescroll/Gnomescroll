#pragma once

#include <entity/constants.hpp>
#include <entity/object/object_list.hpp>

namespace Objects
{

extern ObjectList* object_list;
extern ObjectDataList* object_data;
extern ObjectListFilter* filter;

void init();
void teardown();

void tick();
void update();
void harvest();

class Component* add_component_to_object(class Object* object, ComponentType type);
void release_object_components(Object* object);

Object* create(ObjectType type);
Object* create(ObjectType type, int id);
void ready_switch(Object* object);
void destroy_switch(Object* object);
void destroy_switch(ObjectType type, int id);

Object* get_object(ObjectType type, int id);
int count(ObjectType type);
bool full(ObjectType type);
void send_to_client(ObjectType type, int client_id);

// misc interators. move to another file later
bool point_occupied_by_type(ObjectType type, int x, int y, int z);
void damage_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage);
void spawn_mobs();

} // Objects
