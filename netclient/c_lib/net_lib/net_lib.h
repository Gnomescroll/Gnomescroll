#pragma once

//put stuff that gets exposed to cython at top here
//extern "C" {
	#include "./time/physics_timer.c" //physics timer
//

//extern "C" {
	#include <net_lib/client/pviz.c>
//}


//#include <c_lib.hpp> //DC_SERVER/DC_CLIENT define

#include <net_lib/common/net_time.c>
#include <net_lib/common/net_peer.cpp>
#include <net_lib/common/sequencer.c>
#include <net_lib/common/message_handler.c>

#include <net_lib/client/client.cpp>
#include <net_lib/server/server.c>

#include <net_lib/server.c>
#include <net_lib/client.c>
