#pragma once

#include <agent/constants.hpp>
#include <item/common/enum.hpp>

const size_t ITEM_PRETTY_NAME_MAX_LENGTH = 64;

const int MAX_ITEMS = NULL_ITEM;

const size_t MAX_ITEM_TYPES = NULL_ITEM_TYPE;
const size_t MAX_CONTAINER_TYPES = 16;

const int NULL_SLOT = 0xFF;           // invalid slot value
const ItemGroup NULL_ITEM_GROUP = IG_NONE;
const int NULL_DURABILITY = 0xFFFF;
const int NULL_COST = 0;
const int NULL_CRAFTING_RECIPE = 0xFFFF;
const int NULL_SMELTING_RECIPE = 0xFFFF;
const int NULL_STACK_SIZE = 0;
const int NULL_GAS_LIFETIME = 1;
const int NULL_LOCATION = -1;   // this represents a "not set" value, helpful for error checking

// These are hard maximums for configuration, the actual maximum is defined per-item
// and you should use the appropriate functions defined in properties.hpp
const int MAX_STACK_SIZE = 0xFFFF;
const int MAX_DURABILITY = 0xFFFF;

const size_t CONTAINER_PRETTY_NAME_MAX_LENGTH = 63;

const unsigned int MAX_CONTAINER_SIZE = NULL_SLOT;

const int NULL_CHARGES = -1;

const int MAX_CONTAINERS = NULL_CONTAINER;

const int MAX_MAX_CHARGES = 0xFF;

const int ERROR_SPRITE = 0; // id of item that renders as error
const int UNKNOWN_SPRITE = 14; // id of item that renders as unknown

const int SYNTHESIZER_ITEM_COST_MAX_STRLEN = 3;

const int MAX_CRAFTING_RECIPE = 256;
const int CRAFT_BENCH_INPUTS_MAX = 4;
const int CRAFT_BENCH_OUTPUTS_MAX = 3;

const int MAX_SMELTING_RECIPE = 256;
const int SMELTER_INPUTS_MAX = 4;
const int SMELTER_OUTPUTS_MAX = 2;

const int DEFAULT_SMELTING_RECIPE_CREATION_TIME = 1;

const float AGENT_CONTAINER_REACH = 4.0f;

const float NULL_FIRING_RANGE = 0.0f;
const float DEFAULT_FIRING_RANGE = 4.0f;

const float OBJECT_DEPTH_MAX = -128.0f;

const int MAX_ITEM_PARTICLES = NULL_PARTICLE; // initial (just start maxed out)

const int ITEM_PARTICLE_TTL = 600*30; // 5 minutes
const float ITEM_PARTICLE_DAMPENING = 0.50f;
const float ITEM_PARTICLE_RADIUS = 0.35f;
const float ITEM_PARTICLE_SPRITE_RENDER_SCALE = 0.25f;
const float ITEM_PARTICLE_VOXEL_RENDER_SCALE = 0.20f;

const size_t N_PLAYER_CONTAINERS = 7;
    // hand, inventory, synthesizer, energy_tanks, toolbelt, premium_cache, equipment

// how long to wait before allowing item to be picked up after being thrown by agent
const int ITEM_PICKUP_PREVENTION_DELAY = 30 * 3; // 4 seconds
const int ITEM_INITIAL_PICKUP_PREVENTION = 20;

// frequency of state update broadcasts for an item particle
const int ITEM_PARTICLE_STATE_BROADCAST_TICK_RATE = 30 * 10;    // 10 seconds

// after pickup initiated, max ttl before being considered picked up
const int ITEM_PARTICLE_PICKED_UP_TTL = 30 * 3;
// momentum for flying at agent
const int ITEM_PARTICLE_PICKUP_MOMENTUM = 5.0f;
// distance where pickup initiates
const float ITEM_PARTICLE_PICKUP_BEGIN_DISTANCE = 1.5f;
// distance where initiated pickup will become final
const float ITEM_PARTICLE_PICKUP_END_DISTANCE = AGENT_BOX_RADIUS;

const int N_ACCESSORIES = 3;

inline bool isValid(ItemID item_id)
{
    return (item_id >= 0 && item_id < MAX_ITEMS);
}

inline bool isValid(ItemType item_type)
{
    return (item_type >= 0 && item_type < MAX_ITEM_TYPES);
}

inline bool isValid(ItemContainerID container_id)
{
    return (container_id >= 0 && container_id < MAX_CONTAINERS);
}

inline bool isValid(ItemContainerType container_type)
{
    return (container_type >= 0 && container_type < MAX_CONTAINER_TYPES);
}

inline bool isValid(ItemParticleID particle_id)
{
    return (particle_id >= 0 && particle_id < MAX_ITEM_PARTICLES);
}

inline bool is_tangible_group(ItemGroup group)
{
    return (group != IG_NONE && group != IG_ERROR &&
            group != IG_UNKNOWN && group != IG_DEBUG);
}
