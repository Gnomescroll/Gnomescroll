#pragma once

#include <entity/constants.hpp>

namespace Entities
{

// forward declaration
class Entity;

int count(EntityType type);
bool full(EntityType type);
bool point_occupied_by_type(EntityType type, const Vec3i& position);
void spawn_mobs();

/* API */

class Entity* create(EntityType type);
class Entity* create(EntityType type, EntityID id);
class Entity* get(EntityType type, EntityID id);
class Entity* get_all(EntityType type, int& max, int& count);
void ready(class Entity* entity);
void destroy(class Entity* entity);
void destroy(EntityType type, EntityID id);

void stress_test();

void send_to_client(ClientID client_id);

void spawn_monsters(EntityType type, int n);
void spawn_slimes(int n);

} // Entities
