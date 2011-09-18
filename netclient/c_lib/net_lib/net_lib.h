#pragma once

//put stuff that gets exposed to cython at top here
extern "C" {
	#include "./time/physics_timer.c" //physics timer
}

#include <c_lib.hpp> //DC_SERVER/DC_CLIENT define

#include <net_lib/common/net_time.c>
#include <net_lib/common/net_peer.c>
#include <net_lib/common/sequencer.c>
#include <net_lib/common/message_handler.c>

#include <net_lib/client/pviz.c>

#include <net_lib/client/client.cpp>
#include <net_lib/server/server.c>

#include <net_lib/server.c>
#include <net_lib/client.c>
