#include "_interface.hpp"

#include <t_mech/net/CtoS.hpp>
#include <t_mech/net/StoC.hpp>

namespace t_mech
{

void init_packets()
{
	mech_create_StoC::register_client_packet();
	mech_delete_StoC::register_client_packet();
}

}