#pragma once

#include <agent/constants.hpp>

const int NULL_SLOT = 0xff;           // invalid slot value
const ItemGroup NULL_ITEM_GROUP = IG_NONE;
const int NULL_ITEM_TYPE = 0xff;
const int NULL_DURABILITY = 0xffff;
const int NULL_ENERGY = 0xffff;
const int NULL_CONTAINER = 0xffff;
const int NULL_PARTICLE = 0xffff;
const int NULL_COST = 0;
const int NULL_CRAFTING_RECIPE = 0xffff;
const int NULL_SMELTING_RECIPE = 0xffff;

const int ERROR_SPRITE = 0; // id of item that renders as error
const int UNKNOWN_SPRITE = 14; // id of item that renders as unknown

const int AGENT_CONTAINER_X = 6;
const int AGENT_CONTAINER_Y = 3;

const int AGENT_TOOLBELT_X = 9;
const int AGENT_TOOLBELT_Y = 1;

const int AGENT_SYNTHESIZER_X = 1;
const int AGENT_SYNTHESIZER_Y = 1;
const int AGENT_SYNTHESIZER_SHOPPING_X = 5;
const int AGENT_SYNTHESIZER_SHOPPING_Y = 2;
const int MAX_SYNTHESIZER_OUTPUTS = AGENT_SYNTHESIZER_SHOPPING_X*AGENT_SYNTHESIZER_SHOPPING_Y;
const int SYNTHESIZER_ITEM_COST_MAX_STRLEN = 3;

const int CRAFTING_BENCH_UTILITY_X = 4;
const int CRAFTING_BENCH_UTILITY_Y = 1;

const int CRAFTING_BENCH_REFINERY_X = 2;
const int CRAFTING_BENCH_REFINERY_Y = 1;

const int STORAGE_BLOCK_SMALL_X = 3;
const int STORAGE_BLOCK_SMALL_Y = 3;

const int CRYOFREEZER_SMALL_X = 2;
const int CRYOFREEZER_SMALL_Y = 2;

const int SMELTER_ONE_X = 3;
const int SMELTER_ONE_Y = 2;

const int ENERGY_TANK_MAX = 4;

const int MAX_ITEMS = 256;

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

const int ITEM_GAS_LIFETIME = 20 * 30; // 60 seconds

const int AGENT_HAND = NULL_CONTAINER - 1;

const int ITEM_PARTICLE_MAX = 1024;
const int ITEM_PARTICLE_HARD_MAX = 0xffff - 1;

const int ITEM_PARTICLE_TTL = 600*30; // 5 minutes
const float ITEM_PARTICLE_DAMPENING = 0.50f;
const float ITEM_PARTICLE_RADIUS = 0.35f;

// how long to wait before allowing item to be picked up after being thrown by agent
const int ITEM_PICKUP_PREVENTION_DELAY = 30 * 3; // 4 seconds

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
