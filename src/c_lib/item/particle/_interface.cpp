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
    if (item != NULL) item->location = IL_NOWHERE;

    broadcast_particle_item_destroy(particle->id);
    #endif
    item_particle_list->destroy(particle_id);
}

class ItemParticle* get(int particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    if (particle_id == NULL_PARTICLE) return NULL;
    return item_particle_list->get(particle_id);
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
//void destroy_silently(int particle_id)
//{
    //#if DC_SERVER
    //GS_ASSERT(particle_id != NULL_PARTICLE);
    //ItemParticle* particle = item_particle_list->get(particle_id);
    //GS_ASSERT(particle != NULL);
    //if (particle == NULL) return;
    //Item::Item* item = Item::get_item(particle->item_id);
    //if (item != NULL) item->location = IL_NOWHERE;
    //#endif
    //item_particle_list->destroy(particle_id);
//}

ItemParticle* create_item_particle(
    ItemID item_id, int item_type,
    float x, float y, float z, 
    float vx, float vy, float vz
) {
    Item::Item* item = Item::get_item(item_id);
    if (item == NULL) return NULL;
    // transitioning to item particle, remove all subscribers
    Item::unsubscribe_all_from_item(item->id);
    
    ItemParticle* particle = item_particle_list->create();
    if (particle == NULL) return NULL;
    particle->init(item_id, item_type, x,y,z,vx,vy,vz);
    GS_ASSERT(item->location == IL_NOWHERE);
    item->location = IL_PARTICLE;
    item->location_id = particle->id;
    return particle;
}

// create Item and ItemParticle
Item::Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz)
{    
    Item::Item* item = Item::create_item(item_type);
    if (item == NULL) return NULL;
    // no subscribers
    ItemParticle* particle = create_item_particle(item->id, item->type, x,y,z,vx,vy,vz);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return item;
    GS_ASSERT(item->location == IL_PARTICLE);
    // location stuff was already set
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


// this method is huge and doesnt use auto_add_item_to_container because it has to handle certain transitions from particle state
// into container/etc that I was not able to write generically while ensuring correct state transition order
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
        GS_ASSERT(item->stack_size > 0);
        GS_ASSERT(item->type != NULL_ITEM_TYPE);

        const static float pick_up_distance = 1.1f;
        Agent_state* agent = nearest_living_agent_in_range(item_particle->verlet.position, pick_up_distance);
        if (agent == NULL) continue;

        // try to add to toolbelt first
        bool item_remaining = true;
        bool was_picked_up = false;
        ItemContainer::ItemContainerInterface* container = NULL;
        
        int container_id = ItemContainer::get_agent_toolbelt(agent->id);
        if (container_id != NULL_CONTAINER)
            container = ItemContainer::get_container(container_id);
            
        if (container != NULL)
        {
            int slot = container->get_stackable_slot(item->type, item->stack_size);
            if (slot == NULL_SLOT)
            {   // no easy stack slot found
                // try to put in empty slot
                slot = container->get_empty_slot();
                if (slot == NULL_SLOT)
                {   // no empty slot found, try to break this stack up and redistribute
                    int starting_stack_size = item->stack_size;
                    int stack_size = item->stack_size;
                    int item_type = item->type;

                    // distribute as much as we can into slots
                    for (int i=0; i<container->slot_max; i++)
                    {
                        GS_ASSERT(stack_size > 0);
                        if (container->slot[i] == NULL_ITEM) continue;
                        Item::Item* slot_item = Item::get_item(container->slot[i]);
                        GS_ASSERT(slot_item != NULL);
                        if (slot_item == NULL) continue;
                        if (slot_item->type != item_type) continue;
                        int stack_space = Item::get_stack_space(slot_item->id);
                        if (stack_space == 0) continue;

                        if (stack_space >= stack_size)
                        {   // full, final merge
                            Item::merge_item_stack(item->id, slot_item->id);
                            stack_size = 0;
                        }
                        else
                        {
                            Item::merge_item_stack(item->id, slot_item->id, stack_space);
                            Item::send_item_state(slot_item->id);
                            stack_size -= stack_space;
                            GS_ASSERT(stack_size > 0);
                        }
                        if (stack_size <= 0) break;
                    }
                    
                    if (stack_size <= 0)
                    {
                        was_picked_up = true;
                        broadcast_particle_item_picked_up(agent->id, item_particle->id);
                        Item::destroy_item(item->id);
                        item_remaining = false;
                    }
                    else if (starting_stack_size != stack_size)
                    {   // source item was only partially consumed
                        was_picked_up = true;
                        broadcast_particle_item_picked_up(agent->id, item_particle->id);
                        Item::broadcast_item_state(item->id); // broadcast modified source item's state
                        item_remaining = true;
                    }
                }
                else
                {   // empty slot found, put it there
                    was_picked_up = true;
                    item_particle->picked_up(agent->id);
                    broadcast_particle_item_picked_up(agent->id, item_particle->id);
                    ItemContainer::transfer_particle_to_container(item->id, item_particle->id, container_id, slot);
                    //destroy(item_particle->id);
                    //container->insert_item(slot, item->id);
                    //ItemContainer::send_container_item_create(agent->client_id, item->id, container_id, slot);
                    item_remaining = false;
                }
            }
            else
            {   // stack
                broadcast_particle_item_picked_up(agent->id, item_particle->id);
                was_picked_up = true;
                ItemID slot_item_id = container->get_item(slot);
                GS_ASSERT(slot_item_id != NULL_ITEM);
                Item::merge_item_stack(item->id, slot_item_id);
                Item::destroy_item(item->id);
                Item::send_item_state(slot_item_id);
                item_remaining = false;
            }
        }

        // skip if we've already put everything in the toolbelt
        if (!item_remaining) continue;

        // get the agent inventory
        container = NULL;
        container_id = ItemContainer::get_agent_container(agent->id);
        if (container_id != NULL_CONTAINER)
            container = ItemContainer::get_container(container_id);

        // attempt to place in agent inventory
        if (container != NULL)
        {
            int slot = container->get_stackable_slot(item->type, item->stack_size);
            if (slot == NULL_SLOT)
            {   // no easy stack slot found
                // try to put in empty slot
                slot = container->get_empty_slot();
                if (slot == NULL_SLOT)
                {   // no empty slot found, try to break this stack up and redistribute
                    int starting_stack_size = item->stack_size;
                    int stack_size = item->stack_size;
                    int item_type = item->type;

                    // distribute as much as we can into slots
                    for (int i=0; i<container->slot_max; i++)
                    {
                        GS_ASSERT(stack_size > 0);
                        if (container->slot[i] == NULL_ITEM) continue;
                        Item::Item* slot_item = Item::get_item(container->slot[i]);
                        GS_ASSERT(slot_item != NULL);
                        if (slot_item == NULL) continue;
                        if (slot_item->type != item_type) continue;
                        int stack_space = Item::get_stack_space(slot_item->id);
                        if (stack_space == 0) continue;

                        if (stack_space >= stack_size)
                        {   // full, final merge
                            Item::merge_item_stack(item->id, slot_item->id);
                            stack_size = 0;
                        }
                        else
                        {
                            Item::merge_item_stack(item->id, slot_item->id, stack_space);
                            Item::send_item_state(slot_item->id);
                            stack_size -= stack_space;
                            GS_ASSERT(stack_size > 0);
                        }
                        if (stack_size <= 0) break;
                    }
                    
                    if (stack_size <= 0)
                    {
                        if (!was_picked_up) broadcast_particle_item_picked_up(agent->id, item_particle->id);
                        Item::destroy_item(item->id);
                    }
                    else if (starting_stack_size != stack_size)
                    {   // source item was only partially consumed
                        if (!was_picked_up) broadcast_particle_item_picked_up(agent->id, item_particle->id);
                        Item::broadcast_item_state(item->id); // broadcast modified source item's state
                        item_remaining = true;
                    }
                }
                else
                {   // empty slot found, put it there
                    item_particle->picked_up(agent->id);
                    if (!was_picked_up) broadcast_particle_item_picked_up(agent->id, item_particle->id);
                    ItemContainer::transfer_particle_to_container(item->id, item_particle->id, container_id, slot);
                    //destroy(item_particle->id);
                    //container->insert_item(slot, item->id);
                    //ItemContainer::send_container_item_create(agent->client_id, item->id, container_id, slot);
                }
            }
            else
            {   // stack
                if (!was_picked_up) broadcast_particle_item_picked_up(agent->id, item_particle->id);
                ItemID slot_item_id = container->get_item(slot);
                GS_ASSERT(slot_item_id != NULL_ITEM);
                Item::merge_item_stack(item->id, slot_item_id);
                Item::destroy_item(item->id);
                Item::send_item_state(slot_item_id);
            }
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
    item->location = IL_NOWHERE;

    // create particle
    ItemParticle* particle = create_item_particle(
        item->id, item->type,
        position.x, position.y, position.z,
        velocity.x, velocity.y, velocity.z
    );
    if (particle == NULL) return;
    GS_ASSERT(item->location == IL_PARTICLE);
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
