#pragma once

#include <entity/constants.hpp>

namespace Entities
{

// forward declaration
class Entity;

int count(EntityType type);
bool full(EntityType type);
bool point_occupied_by_type(EntityType type, int x, int y, int z);
void spawn_mobs();

/* API */

class Entity* create(EntityType type);
class Entity* create(EntityType type, int id);
class Entity* get(EntityType type, int id);
void ready(class Entity* object);
void destroy(class Entity* object);
void destroy(EntityType type, int id);

int get_all(const EntityType type, class Entity**& entities, char*& used);

void stress_test();

void send_to_client(ClientID client_id);

} // Entities
