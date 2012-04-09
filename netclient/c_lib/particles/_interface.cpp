#include "_interface.hpp"

#include "grenade.hpp"
#include "cspray.hpp"

class Grenade_list* grenade_list = NULL;
class Cspray_list* cspray_list = NULL;

void init_particles()
{
    cspray_list = new Cspray_list;
    grenade_list = new Grenade_list;
}

void tear_down_particles()
{
	delete cspray_list;
	delete grenade_list;
}