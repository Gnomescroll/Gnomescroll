#pragma once

#ifdef DC_CLIENT
# error Don't include this file in the client
#endif

#include <t_plant/_interface.hpp>
#include <t_plant/_data.hpp>
#include <t_plant/_interface.cpp>
#include <t_plant/block_list.hpp>
#include <t_plant/plant_example.hpp>
#include <t_plant/plant_xi.hpp>
