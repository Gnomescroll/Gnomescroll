#include "interfaces.hpp"

namespace Entities
{

CreatePacketNone* create_packet_none = NULL;
CreatePacket* create_packet = NULL;
CreatePacketOwner* create_packet_owner = NULL;
CreatePacketMomentum* create_packet_momentum = NULL;
CreatePacketMomentumAngles* create_packet_momentum_angles = NULL;
CreatePacketMomentumAnglesHealth* create_packet_momentum_angles_health;

StatePacketNone* state_packet_none = NULL;
StatePacket* state_packet = NULL;
StatePacketMomentum* state_packet_momentum = NULL;
StatePacketMomentumAngles* state_packet_momentum_angles = NULL;

void init_net_interfaces()
{
    GS_ASSERT(create_packet_none == NULL);
    create_packet_none = new CreatePacketNone;
    create_packet = new CreatePacket;
    create_packet_owner = new CreatePacketOwner;
    create_packet_momentum = new CreatePacketMomentum;
    create_packet_momentum_angles = new CreatePacketMomentumAngles;
    create_packet_momentum_angles_health = new CreatePacketMomentumAnglesHealth;

    state_packet_none = new StatePacketNone;
    state_packet = new StatePacket;
    state_packet_momentum = new StatePacketMomentum;
    state_packet_momentum_angles = new StatePacketMomentumAngles;
}

void teardown_net_interfaces()
{
    delete create_packet_none;
    delete create_packet;
    delete create_packet_owner;
    delete create_packet_momentum;
    delete create_packet_momentum_angles;
    delete create_packet_momentum_angles_health;
    delete state_packet_none;
    delete state_packet;
    delete state_packet_momentum;
    delete state_packet_momentum_angles;
}

void init_packets()
{
    object_create_StoC::register_client_packet();
    object_create_owner_StoC::register_client_packet();
    object_create_momentum_StoC::register_client_packet();
    object_create_momentum_angles_StoC::register_client_packet();
    object_create_momentum_angles_health_StoC::register_client_packet();
    object_destroy_StoC::register_client_packet();
    object_state_StoC::register_client_packet();
    object_state_momentum_StoC::register_client_packet();
    object_state_momentum_angles_StoC::register_client_packet();
    object_state_health_StoC::register_client_packet();
    object_shot_object_StoC::register_client_packet();
    object_shot_terrain_StoC::register_client_packet();
    object_shot_nothing_StoC::register_client_packet();
    object_took_damage_StoC::register_client_packet();
}

} // Entities
