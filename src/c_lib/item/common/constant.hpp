#pragma once

const int NULL_SLOT = 0xff;           // invalid slot value
const int NULL_ITEM_GROUP = 0xff;
const int NULL_ITEM_TYPE = 0xff;
const int NULL_DURABILITY = 0xffff;
const int NULL_ENERGY = 0xffff;
const int NULL_CONTAINER = 0xffff;
const int NULL_PARTICLE = -1;
const int NULL_ITEM_LEVEL = -1;
const int NULL_COST = 0;
const int NULL_CRAFTING_RECIPE = 0xffff;

const int ERROR_SPRITE = 0; // id of item that renders as error
const int UNKNOWN_SPRITE = 14; // id of item that renders as unknown

const int AGENT_CONTAINER_X = 6;
const int AGENT_CONTAINER_Y = 3;

const int AGENT_TOOLBELT_X = 9;
const int AGENT_TOOLBELT_Y = 1;

const int AGENT_NANITE_X = 2;
const int AGENT_NANITE_Y = 1;

const int CRAFTING_BENCH_UTILITY_X = 4;
const int CRAFTING_BENCH_UTILITY_Y = 1;

const int CRAFTING_BENCH_REFINERY_X = 2;
const int CRAFTING_BENCH_REFINERY_Y = 1;

const int STORAGE_BLOCK_SMALL_X = 3;
const int STORAGE_BLOCK_SMALL_Y = 3;

const int CRYOFREEZER_SMALL_X = 2;
const int CRYOFREEZER_SMALL_Y = 2;

const int MAX_ITEMS = 256;
const int MAX_CRAFTING_RECIPE = 256;

const int NANITE_DIGESTION_RATE = 30 * 5;

const int CRAFT_BENCH_INPUTS_MAX = 9;
const int CRAFT_BENCH_OUTPUTS_MAX = 6;

const float AGENT_CONTAINER_REACH = 4.0f;

const float NULL_FIRING_RANGE = 0.0f;
const float DEFAULT_FIRING_RANGE = 4.0f;

const float OBJECT_DEPTH_MAX = -128.0f;

const int ITEM_GAS_LIFETIME = 20 * 30; // 60 seconds

const int AGENT_HAND = NULL_CONTAINER - 1;
