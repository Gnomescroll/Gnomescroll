#pragma once

namespace ItemParticle
{

extern class ItemParticle_list* item_particle_list;

void init();
void teardown();

void tick();
void destroy(int particle_id);

class ItemParticle* get(int particle_id);

#if DC_CLIENT
void draw();

class ItemParticle* create_item_particle(
    int particle_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
);
#endif

#if DC_SERVER

class ItemParticle* create_item_particle(
    ItemID item_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
);

// create Item and ItemParticle
class Item::Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz);
class Item::Item* create_item_particle(int item_type, Vec3 position, Vec3 momentum);

void broadcast_particle_item_create(int particle_id);
void send_particle_item_create_to_client(int particle_id, int client_id);
void send_particle_items_to_client(int client_id);
void broadcast_particle_item_state(int particle_id);
void broadcast_particle_item_destroy(int particle_id);
void broadcast_particle_item_picked_up(int agent_id, int particle_id);
void broadcast_particle_item_pickup_cancelled(int particle_id);

void check_item_pickups();
void throw_agent_item(int agent_id, ItemID item_id);
void dump_container_item(ItemID item_id, float x, float y, float z);

#endif

}   // ItemParticle
