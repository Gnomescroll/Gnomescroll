#pragma once

#include <entity/network/interfaces.hpp>

namespace Entities
{

extern CreatePacketNone* create_packet_none;
extern CreatePacket* create_packet;
extern CreatePacketOwner* create_packet_owner;
extern CreatePacketMomentum* create_packet_momentum;
extern CreatePacketMomentumAngles* create_packet_momentum_angles;
extern CreatePacketMomentumAnglesHealth* create_packet_momentum_angles_health;

extern StatePacketNone* state_packet_none;
extern StatePacket* state_packet;
extern StatePacketMomentum* state_packet_momentum;
extern StatePacketMomentumAngles* state_packet_momentum_angles;

void init_net_interfaces();
void teardown_net_interfaces();

} // Entities
