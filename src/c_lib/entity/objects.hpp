#pragma once

#include <entity/constants.hpp>

namespace Objects
{

// forward declaration
class Object;

int count(ObjectType type);
bool full(ObjectType type);
bool point_occupied_by_type(ObjectType type, int x, int y, int z);
void spawn_mobs();

/* API */

Object* create(ObjectType type);
Object* create(ObjectType type, int id);
Object* get(ObjectType type, int id);
void ready(Object* object);
void destroy(Object* object);
void destroy(ObjectType type, int id);

void stress_test();

void send_to_client(ClientID client_id);

} // Objects
