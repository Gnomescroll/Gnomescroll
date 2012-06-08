#include "_interface.hpp"

#include <agent/agent.hpp>

#include <item/_interface.hpp>
#include <item/particle/item_particle.hpp>
#include <item/particle/net/StoC.hpp>

namespace ItemParticle
{

class ItemParticle_list* item_particle_list = NULL;

void init()
{
    item_particle_list = new ItemParticle_list;
}

void teardown()
{
    delete item_particle_list;
}

void tick()
{
    GS_ASSERT(item_particle_list != NULL);
    item_particle_list->tick();

    #if DC_SERVER
    static int tick = 0;
    for (int i=0; i<item_particle_list->n_max; i++)
    {
        if (item_particle_list->a[i] == NULL) continue;
        if ((tick + item_particle_list->a[i]->broadcast_tick) % ITEM_PARTICLE_STATE_BROADCAST_TICK_RATE == 0)
            broadcast_particle_item_state(item_particle_list->a[i]->id);
    }
    tick++;
    #endif
}

void destroy(int particle_id)
{
    #if DC_SERVER
    GS_ASSERT(particle_id != NULL_PARTICLE);
    ItemParticle* particle = item_particle_list->get(particle_id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return;
    Item::Item* item = Item::get_item(particle->item_id);
    if (item != NULL) item->particle_id = NULL_PARTICLE;

    broadcast_particle_item_destroy(particle->id);
    #endif
    item_particle_list->destroy(particle_id);
}

#if DC_CLIENT
void draw_init()
{
    init_item_particle();
}

void draw_teardown()
{
    teardown_item_particle();
}

void draw()
{
    GS_ASSERT(item_particle_list != NULL);
    item_particle_list->draw();
}

ItemParticle* create_item_particle(
    int particle_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
) {
    ItemParticle* particle = item_particle_list->create(particle_id);
    if (particle == NULL) return NULL;
    particle->init(item_type, x,y,z,vx,vy,vz);
    return particle;
}
#endif

#if DC_SERVER
ItemParticle* create_item_particle(
    ItemID item_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
) {
    Item::Item* item = Item::get_item(item_id);
    if (item == NULL) return NULL;
    ItemParticle* particle = item_particle_list->create();
    if (particle == NULL) return NULL;
    particle->init(item_id, item_type, x,y,z,vx,vy,vz);
    GS_ASSERT(item->container_id == NULL_CONTAINER);
    item->particle_id = particle->id;
    return particle;
}

// create Item and ItemParticle
Item::Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz)
{    
    Item::Item* item = Item::create_item(item_type);
    if (item == NULL) return NULL;
    ItemParticle* particle = create_item_particle(item->id, item->type, x,y,z,vx,vy,vz);
    if (particle == NULL) return item;
    item->particle_id = particle->id;
    broadcast_particle_item_create(particle->id);
    return item;
}

static bool pack_particle_item_create(int particle_id, item_particle_create_StoC* msg)
{
    ItemParticle* particle = item_particle_list->get(particle_id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return false;
    
    msg->id = particle->id;
    msg->item_type = particle->item_type;
    msg->x = particle->verlet.position.x;
    msg->y = particle->verlet.position.y;
    msg->z = particle->verlet.position.z;
    msg->mx = particle->verlet.velocity.x;
    msg->my = particle->verlet.velocity.y;
    msg->mz = particle->verlet.velocity.z;
    return true;
}

void broadcast_particle_item_create(int particle_id)
{
    item_particle_create_StoC msg;
    if (!pack_particle_item_create(particle_id, &msg)) return;
    msg.broadcast();
}

void send_particle_item_create_to_client(int particle_id, int client_id)
{
    item_particle_create_StoC msg;
    if (!pack_particle_item_create(particle_id, &msg)) return;
    msg.sendToClient(client_id);
}

void send_particle_items_to_client(int client_id)
{
    for (int i=0; i<item_particle_list->n_max; i++)
        if (item_particle_list->a[i] != NULL)
            send_particle_item_create_to_client(item_particle_list->a[i]->id, client_id);
}

void broadcast_particle_item_state(int particle_id)
{
    ItemParticle* particle = item_particle_list->get(particle_id);
    if (particle == NULL) return;
    item_particle_state_StoC msg;
    msg.id = particle->id;
    msg.x = particle->verlet.position.x;
    msg.y = particle->verlet.position.y;
    msg.z = particle->verlet.position.z;
    msg.mx = particle->verlet.velocity.x;
    msg.my = particle->verlet.velocity.y;
    msg.mz = particle->verlet.velocity.z;
    msg.broadcast();
}

void broadcast_particle_item_destroy(int particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    if (particle_id == NULL_PARTICLE) return;
    class item_particle_destroy_StoC msg;
    msg.id = particle_id;
    msg.broadcast();
}

void broadcast_particle_item_picked_up(int agent_id, int particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    ASSERT_VALID_AGENT_ID(agent_id);
    item_particle_picked_up_StoC msg;
    msg.agent_id = agent_id;
    msg.id = particle_id;
    msg.broadcast();
}

void check_item_pickups()
{
    for (int i=0; i<item_particle_list->n_max; i++)
    {
        if (item_particle_list->a[i] == NULL) continue;
        ItemParticle* item_particle = item_particle_list->a[i];
        if (!item_particle->can_be_picked_up()) continue;
        Item::Item* item = Item::get_item(item_particle->item_id);
        GS_ASSERT(item != NULL);
        if (item == NULL) continue;

        int starting_size = item->stack_size;
    
        const static float pick_up_distance = 1.1f;
        Agent_state* agent = nearest_living_agent_in_range(item_particle->verlet.position, pick_up_distance);
        if (agent == NULL) continue;

        // try to add to toolbelt first
        ContainerActionType toolbelt_event = CONTAINER_ACTION_NONE;
        ContainerActionType inventory_event = CONTAINER_ACTION_NONE;
        int container_id = ItemContainer::get_agent_toolbelt(agent->id);
        if (container_id != NULL_CONTAINER)
            toolbelt_event = ItemContainer::auto_add_item_to_container(agent->client_id, container_id, item->id);   //insert item on server

        // then try to add to inventory
        if (toolbelt_event == CONTAINER_ACTION_NONE || toolbelt_event == PARTIAL_WORLD_TO_OCCUPIED_SLOT)
        {
            container_id = ItemContainer::get_agent_container(agent->id);
            if (container_id != NULL_CONTAINER)
                inventory_event = ItemContainer::auto_add_item_to_container(agent->client_id, container_id, item->id);   //insert item on server
        }

        if ((toolbelt_event != CONTAINER_ACTION_NONE && toolbelt_event != PARTIAL_WORLD_TO_OCCUPIED_SLOT)
         || (inventory_event != CONTAINER_ACTION_NONE && inventory_event != PARTIAL_WORLD_TO_OCCUPIED_SLOT))
        {
            if (toolbelt_event == FULL_WORLD_TO_OCCUPIED_SLOT || inventory_event == FULL_WORLD_TO_OCCUPIED_SLOT)
            {
                broadcast_particle_item_picked_up(agent->id, item_particle->id);
                Item::destroy_item(item->id);
            }
            else if (toolbelt_event == FULL_WORLD_TO_EMPTY_SLOT || inventory_event == FULL_WORLD_TO_EMPTY_SLOT)
            {
                // update particle
                item_particle->picked_up(agent->id);
                // remove from item
                destroy(item_particle->id);
                item->particle_id = NULL_PARTICLE;
            }
        }
        else if (toolbelt_event == PARTIAL_WORLD_TO_OCCUPIED_SLOT || inventory_event == PARTIAL_WORLD_TO_OCCUPIED_SLOT)
        {   // partial stack pickup
            GS_ASSERT(item->stack_size > 0);
            GS_ASSERT(starting_size != item->stack_size);
            // send a pickup notification so sounds/anim can playX
            broadcast_particle_item_picked_up(agent->id, item_particle->id);
        }
    }
}

static void throw_item(ItemID item_id, Vec3 position, Vec3 velocity)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;
    item->container_id = NULL_CONTAINER;

    // create particle
    ItemParticle* particle = create_item_particle(
        item->id, item->type,
        position.x, position.y, position.z,
        velocity.x, velocity.y, velocity.z
    );
    if (particle == NULL) return;
    broadcast_particle_item_create(particle->id);
    particle->lock_pickup();
}

void throw_agent_item(int agent_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL)
    {   // we cannot get the agent state, so just destroy the item
        GS_ASSERT(false);
        Item::destroy_item(item_id);
        return;
    }

    Vec3 position = a->get_center();

    const float mom = 2.0f;
    Vec3 force = a->forward_vector();
    force.z = 0;
    normalize_vector(&force);
    force = vec3_scalar_mult(force, mom);
    force = vec3_bias(force, (randf()-0.5f) * 30);

    throw_item(item_id, position, force);
}

void dump_container_item(ItemID item_id, float x, float y, float z)
{
    GS_ASSERT(item_id != NULL_ITEM);
    
    Vec3 position = vec3_init(x,y,z);
    position = vec3_add(position, vec3_init(randf(), randf(), randf())); // random point inside box

    const float mom = 2.0f;
    Vec3 force = vec3_init(randf()-0.5f, randf()-0.5f, randf()-0.5f);
    normalize_vector(&force);
    force = vec3_scalar_mult(force, mom);

    throw_item(item_id, position, force);
}

#endif

}   // ItemParticle
