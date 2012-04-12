#include "interfaces.hpp"

namespace Objects
{

CreatePacketNone* create_packet_none = NULL;
CreatePacket* create_packet = NULL;
CreatePacketOwnerTeam* create_packet_owner_team = NULL;
CreatePacketOwnerTeamIndex* create_packet_owner_team_index = NULL;
CreatePacketMomentum* create_packet_momentum = NULL;
CreatePacketMomentumAngles* create_packet_momentum_angles = NULL;

StatePacketNone* state_packet_none = NULL;
StatePacket* state_packet = NULL;
StatePacketMomentum* state_packet_momentum = NULL;
StatePacketMomentumAngles* state_packet_momentum_angles = NULL;

void init_net_interfaces()
{
    create_packet_none = new CreatePacketNone;
    create_packet = new CreatePacket;
    create_packet_owner_team = new CreatePacketOwnerTeam;
    create_packet_owner_team_index = new CreatePacketOwnerTeamIndex;
    create_packet_momentum = new CreatePacketMomentum;
    create_packet_momentum_angles = new CreatePacketMomentumAngles;

    state_packet_none = new StatePacketNone;
    state_packet = new StatePacket;
    state_packet_momentum = new StatePacketMomentum;
    state_packet_momentum_angles = new StatePacketMomentumAngles;
}

void teardown_net_interfaces()
{
    if (create_packet_none != NULL) delete create_packet_none;
    if (create_packet != NULL) delete create_packet;
    if (create_packet_owner_team != NULL) delete create_packet_owner_team;
    if (create_packet_owner_team_index != NULL) delete create_packet_owner_team_index;
    if (create_packet_momentum != NULL) delete create_packet_momentum;
    if (create_packet_momentum_angles != NULL) delete create_packet_momentum_angles;

    if (state_packet_none != NULL) delete state_packet_none;
    if (state_packet != NULL) delete state_packet;
    if (state_packet_momentum != NULL) delete state_packet_momentum;
    if (state_packet_momentum_angles != NULL) delete state_packet_momentum_angles;
}

} // Objects
