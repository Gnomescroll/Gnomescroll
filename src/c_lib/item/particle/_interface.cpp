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
    item_particle_list = new ItemParticle_list(MAX_ITEM_PARTICLES);
}

void teardown()
{
    if (item_particle_list != NULL) delete item_particle_list;
}

void init_packets()
{
    item_particle_create_StoC::register_client_packet();
    item_particle_destroy_StoC::register_client_packet();
    item_particle_state_StoC::register_client_packet();
    item_particle_picked_up_StoC::register_client_packet();
    item_particle_pickup_cancelled_StoC::register_client_packet();
}

void tick()
{
    GS_ASSERT(item_particle_list != NULL);
    if (item_particle_list == NULL) return;

    item_particle_list->tick();

    #if DC_SERVER
    static int tick = 0;
    for (unsigned int i=0; i<item_particle_list->max; i++)
    {
        if (item_particle_list->objects[i].id == item_particle_list->null_id) continue;
        if ((tick + item_particle_list->objects[i].broadcast_tick) % ITEM_PARTICLE_STATE_BROADCAST_TICK_RATE == 0)
            broadcast_particle_item_state(item_particle_list->objects[i].id);
    }
    tick++;
    #endif
}

void destroy(ItemParticleID particle_id)
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

class ItemParticle* get(ItemParticleID particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    if (particle_id == NULL_PARTICLE) return NULL;
    return item_particle_list->get(particle_id);
}

#if DC_CLIENT

void draw()
{
    GS_ASSERT(item_particle_list != NULL);
    item_particle_list->draw();
}

ItemParticle* create_item_particle(
    ItemParticleID particle_id, int item_type,
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
    // transitioning to item particle, remove all subscribers
    if (item->location == IL_CONTAINER)
    {
        ItemContainer::ItemContainerInterface* container =
            ItemContainer::get_container(item->location_id);
        GS_ASSERT(container != NULL);
        if (container != NULL
         && ItemContainer::get_agent_toolbelt(container->owner) == container->id
         && Toolbelt::get_agent_selected_item(container->owner) == item->id)
        {
            GS_ASSERT(Toolbelt::get_agent_selected_slot(container->owner) == item->container_slot);
            Toolbelt::force_remove_selected_item(container->owner);
        }
    }
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
class Item::Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz)
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

class Item::Item* create_item_particle(int item_type, Vec3 position, Vec3 momentum)
{
    return create_item_particle(item_type, position.x, position.y, position.z, momentum.x, momentum.y, momentum.z);
}

static bool pack_particle_item_create(ItemParticleID particle_id, item_particle_create_StoC* msg)
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

void broadcast_particle_item_create(ItemParticleID particle_id)
{
    item_particle_create_StoC msg;
    if (!pack_particle_item_create(particle_id, &msg)) return;
    msg.broadcast();
}

static void send_particle_item_create_to_client(ItemParticleID particle_id, ClientID client_id)
{
    item_particle_create_StoC msg;
    if (!pack_particle_item_create(particle_id, &msg)) return;
    msg.sendToClient(client_id);
}

void send_particle_items_to_client(ClientID client_id)
{
    for (unsigned int i=0; i<item_particle_list->max; i++)
    {
        if (item_particle_list->objects[i].id == item_particle_list->null_id) continue;
        ItemParticle* p = &item_particle_list->objects[i];
        send_particle_item_create_to_client(p->id, client_id);
        if (p->target_agent != NULL_AGENT)
            send_particle_item_picked_up(client_id, p->target_agent, p->id);
    }
}

void broadcast_particle_item_state(ItemParticleID particle_id)
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

void broadcast_particle_item_destroy(ItemParticleID particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    if (particle_id == NULL_PARTICLE) return;
    class item_particle_destroy_StoC msg;
    msg.id = particle_id;
    msg.broadcast();
}

void send_particle_item_picked_up(ClientID client_id, AgentID agent_id, ItemParticleID particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    GS_ASSERT(isValid(agent_id));
    item_particle_picked_up_StoC msg;
    msg.agent_id = agent_id;
    msg.id = particle_id;
    msg.sendToClient(client_id);
}

void broadcast_particle_item_picked_up(AgentID agent_id, ItemParticleID particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    GS_ASSERT(isValid(agent_id));
    item_particle_picked_up_StoC msg;
    msg.agent_id = agent_id;
    msg.id = particle_id;
    msg.broadcast();
}

void broadcast_particle_item_pickup_cancelled(ItemParticleID particle_id)
{
    GS_ASSERT(particle_id != NULL_PARTICLE);
    item_particle_pickup_cancelled_StoC msg;
    msg.id = particle_id;
    msg.broadcast();
}

// 1
// split item into new item
// create particle for it
// set to be pickup
static void split_item_particle(Item::Item* item, ItemParticle* particle, int item_type, int stack_size, AgentID target_agent)
{
    GS_ASSERT(item != NULL);
    GS_ASSERT(particle != NULL);
    if (item == NULL || particle == NULL) return;
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    GS_ASSERT(stack_size > 0);
    GS_ASSERT(item->stack_size > stack_size);    // its not splitting if it uses up the whole item
    
    // decrement source item stack size
    item->stack_size -= stack_size;
        
    // set position
    Vec3 p = particle->verlet.position;
    Vec3 v = particle->verlet.velocity;

    // create new particle
    Item::Item* split_item = create_item_particle(item_type, p, v);
    GS_ASSERT(split_item != NULL);
    if (split_item == NULL) return;
    GS_ASSERT(split_item->location = IL_PARTICLE);
    GS_ASSERT(split_item->location_id != NULL_PARTICLE);
    // set stack size
    split_item->stack_size = stack_size;
    
    // set new particle as picked_up
    ItemParticle* split_particle = get((ItemParticleID)split_item->location_id);
    GS_ASSERT(split_particle != NULL);
    if (split_particle == NULL) return;
    split_particle->picked_up(target_agent);
}

// 2
// set to be pickup


// TODO -
// Need to do the actual item inserts when particle hits player
// NOTE -- they may reach player out of order, screwing up the prediction
// In that case, unpickup the item

// this method is huge and doesnt use auto_add_item_to_container because it has to handle certain transitions from particle state
// into container/etc that I was not able to write generically while ensuring correct state transition order
void check_item_pickups()
{
    static int coin_type = Item::get_item_type("synthesizer_coin");
    GS_ASSERT(coin_type != NULL_ITEM_TYPE);
    
    static int energy_tank_type = Item::get_item_type("energy_tank");
    GS_ASSERT(energy_tank_type != NULL_ITEM_TYPE);
    
    for (unsigned int i=0; i<item_particle_list->max; i++)
    {
        if (item_particle_list->objects[i].id == item_particle_list->null_id) continue;
        ItemParticle* item_particle = &item_particle_list->objects[i];
        if (!item_particle->can_be_picked_up()) continue;
        GS_ASSERT(item_particle->item_id != NULL_ITEM);
        Item::Item* item = Item::get_item(item_particle->item_id);
        GS_ASSERT(item != NULL);
        if (item == NULL)
        {   // BAD ERROR -- source item is missing. just die
            destroy(item_particle->id);
            continue;
        }
        GS_ASSERT(item->stack_size > 0);
        GS_ASSERT(item->type != NULL_ITEM_TYPE);

        Agent* agent = nearest_living_agent_in_range(
            item_particle->verlet.position, ITEM_PARTICLE_PICKUP_BEGIN_DISTANCE);
        if (agent == NULL) continue;

        bool item_remaining = true;
        bool item_altered = false;
        ItemContainer::ItemContainerInterface* container = NULL;
        int container_id = NULL_CONTAINER;
        
        // for chips/coins, try to add to synthesizer first
        if (item->type == coin_type)
        {
            container_id = ItemContainer::get_agent_synthesizer(agent->id);
            if (container_id != NULL_CONTAINER)
                container = ItemContainer::get_container(container_id);
                
            GS_ASSERT(container != NULL);
            if (container != NULL)
            {
                // empty slot
                // full stack
                // partial stack
                
                int slot = container->get_empty_slot();
                if (slot != NULL_SLOT)
                {
                    item_particle->picked_up(agent->id);
                    item_remaining = false;
                }
                else
                {
                    int slot = container->get_stackable_slot(item->type, item->stack_size);
                    if (slot != NULL_SLOT)
                    {
                        item_particle->picked_up(agent->id);
                        item_remaining = false;
                    }
                    else
                    {
                        using ItemContainer::ItemContainerSynthesizer;
                        ItemID slot_item = ((ItemContainerSynthesizer*)container)->get_coins();
                        GS_ASSERT(slot_item != NULL_ITEM);
                        int slot_item_type = Item::get_item_type(slot_item);
                        GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
                        GS_ASSERT(slot_item_type == item->type);
                        int slot_item_stack_size = Item::get_stack_size(slot_item);
                        int slot_item_max_stack = Item::get_max_stack_size(slot_item_type);
                        int stack_space = slot_item_max_stack - slot_item_stack_size;
                        if (stack_space > 0)
                        {
                            item_remaining = true;
                            item_altered = true;
                            split_item_particle(item, item_particle, item->type, stack_space, agent->id);                            
                        }
                    }
                }
            }
        }
        else
        if (item->type == energy_tank_type)
        {
            container_id = ItemContainer::get_agent_energy_tanks(agent->id);
            if (container_id != NULL_CONTAINER)
                container = ItemContainer::get_container(container_id);
                
            GS_ASSERT(container != NULL);
            if (container != NULL)
            {
                // empty slot
                // full stack
                // partial stack
                int slot = container->get_empty_slot();
                if (slot != NULL_SLOT)
                {
                    item_particle->picked_up(agent->id);
                    item_remaining = false;
                }
                else
                {
                    int slot = container->get_stackable_slot(item->type, item->stack_size);
                    if (slot != NULL_SLOT)
                    {   
                        item_particle->picked_up(agent->id);
                        item_remaining = false;
                    }
                    else
                    {   // distribute among slots
                        int stack_size = item->stack_size;
                        int starting_stack_size = item->stack_size;
                        for (int i=0; i<container->slot_max; i++)
                        {
                            ItemID slot_item = container->slot[i];
                            GS_ASSERT(slot_item != NULL_ITEM);
                            int slot_item_type = Item::get_item_type(slot_item);
                            GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
                            GS_ASSERT(slot_item_type == item->type);
                            int stack_space = Item::get_stack_space(slot_item);
                            if (stack_space == 0) continue;

                            if (stack_space >= stack_size)
                            {   // full, final merge
                                stack_size = 0;
                                item_particle->picked_up(agent->id);
                            }
                            else
                            {
                                stack_size -= stack_space;
                                GS_ASSERT(stack_size > 0);
                                split_item_particle(item, item_particle, item->type, stack_space, agent->id);
                            }

                            if (stack_size <= 0) break;
                        }
                        
                        if (stack_size <= 0)
                        {    // source item was fully consumed
                            item_altered = true;
                            item_remaining = false;
                        }
                        else if (starting_stack_size != stack_size)
                        {   // source item was only partially consumed
                            item_altered = true;
                            item_remaining = true;
                        }
                    }
                }
            }
        }        
        
        if (!item_remaining) continue;

        // try to add to toolbelt
        
        container_id = ItemContainer::get_agent_toolbelt(agent->id);
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
                            stack_size = 0;
                            item_particle->picked_up(agent->id);
                        }
                        else
                        {
                            stack_size -= stack_space;
                            GS_ASSERT(stack_size > 0);
                            split_item_particle(item, item_particle, item_type, stack_space, agent->id);
                        }

                        if (stack_size <= 0) break;
                    }
                    
                    if (stack_size <= 0)
                    {    // source item was fully consumed
                        item_altered = true;
                        item_remaining = false;
                    }
                    else if (starting_stack_size != stack_size)
                    {   // source item was only partially consumed
                        item_altered = true;
                        item_remaining = true;
                    }
                }
                else
                {   // empty slot found, put it there
                    item_particle->picked_up(agent->id);
                    item_remaining = false;
                }
            }
            else
            {    // item stacks fully with another slot
                item_particle->picked_up(agent->id);
                item_remaining = false;
            }
        }

        // skip if we've already put everything in the toolbelt
        if (!item_remaining) continue;

        // get the agent inventory
        container = NULL;
        container_id = ItemContainer::get_agent_inventory(agent->id);
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
                            stack_size = 0;
                            item_particle->picked_up(agent->id);
                        }
                        else
                        {
                            stack_size -= stack_space;
                            GS_ASSERT(stack_size > 0);
                            split_item_particle(item, item_particle, item_type, stack_space, agent->id);
                        }

                        if (stack_size <= 0) break;
                    }
                    
                    if (stack_size <= 0)
                    {
                        item_altered = true;
                        item_remaining = false;
                    }
                    else if (starting_stack_size != stack_size)
                    {   // source item was only partially consumed
                        item_altered = true;
                        item_remaining = true;
                    }
                }
                else
                {   // empty slot found, put it there
                    item_particle->picked_up(agent->id);
                    item_remaining = false;
                }
            }
            else
            {    // item stacks fully in a slot
                item_particle->picked_up(agent->id);
                item_remaining = false;
            }
        }

        if (item_altered)
            Item::send_item_state(item->id);
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

    position = translate_position(position);

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

void throw_agent_item(AgentID agent_id, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    
    Agent* a = Agents::get_agent(agent_id);
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
    if (vec3_length_squared(force) == 0.0f) force.x = 1.0f;
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
    if (vec3_length_squared(force) == 0.0f) force.x = 0.2f;
    normalize_vector(&force);
    force = vec3_scalar_mult(force, mom);

    throw_item(item_id, position, force);
}

#endif

}   // ItemParticle
