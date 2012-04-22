#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/object/object_list.hpp>

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

class Component;
Component* add_component_to_object(Object* object, ComponentType type);
void remove_component_from_object(Object* object, Component* component);

Object* create_switch(ObjectType type);
Object* create_switch(ObjectType type, int id);
void ready_switch(Object* object);
void destroy_switch(Object* object);
void destroy_switch(ObjectType type, int id);

Object* get_object(ObjectType type, int id);
bool full(ObjectType type);
void send_to_client(ObjectType type, int client_id);

// misc interators. move to another file later
bool point_occupied_by_type(ObjectType type, int x, int y, int z);
void damage_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage);
void damage_team_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage, int inflictor_team, int inflictor_id);
void spawn_mobs();

} // Objects
