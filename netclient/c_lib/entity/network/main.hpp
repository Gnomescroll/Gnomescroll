#pragma once

#include <entity/network/interfaces.hpp>

namespace Objects
{

extern CreatePacketNone* create_packet_none;
extern CreatePacket* create_packet;
extern CreatePacketOwnerTeam* create_packet_owner_team;
extern CreatePacketOwnerTeamIndex* create_packet_owner_team_index;
extern CreatePacketMomentum* create_packet_momentum;
extern CreatePacketMomentumAngles* create_packet_momentum_angles;

extern StatePacketNone* state_packet_none;
extern StatePacket* state_packet;
extern StatePacketMomentum* state_packet_momentum;
extern StatePacketMomentumAngles* state_packet_momentum_angles;

void init_net_interfaces();
void teardown_net_interfaces();

} // Objects
