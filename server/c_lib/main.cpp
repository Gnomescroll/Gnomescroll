#include "main.hpp"

namespace Main
{


void init()
{
	init_c_lib();
	
	NetServer::init_server(127,0,0,1, Options::port);
	ServerState::start_game();

}

int run()
{


    while (1)
    {

	}

    return 0;
}

}
