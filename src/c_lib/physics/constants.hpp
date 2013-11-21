/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

const float kPI = 3.14159f;

// tickrate, tickrate_squared, used in physics calculations
// it doesn't match the actual dt because... i couldn't get the right
// results in the agent's physics tick with 1/30
const float PHYSICS_TICK_RATE = 1.0f / 10.0f;
const float PHYSICS_TICK_RATE_SQ = PHYSICS_TICK_RATE*PHYSICS_TICK_RATE;
