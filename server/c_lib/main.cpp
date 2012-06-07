#pragma once

#include <common/time/physics_timer.hpp>
#include <map_gen/map_generator.hpp>
#include <map_gen/recipes.hpp>

#include <t_gen/_interface.hpp>
#include <t_map/_interface.hpp>

namespace Main
{



void init()
{
    init_c_lib();

    // generate floor map
    //_floor(512,512,0,32,1);
    
    if(0)
    {
        MapGen::init();
        MapRecipes::simple_map();
        MapGen::teardown();
        Dragon::caves();
        Dragon::test_filters();
    }
    else
    {
        t_gen::noise_map_generate_map();
    }   

    srand(time(NULL));
    
    //t_gen::gen_map();

    t_map::map_post_processing();

    int address[4];
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);
    ServerState::start_game();

    #if !PRODUCTION
    //Item::test_item_list_capacity();
    //ItemContainer::test_container_list_capacity();
    #endif
}
   
void tick()
{
    static int counter = 0; counter ++;

    t_map::t_map_send_map_chunks();  //every tick

    if(counter % 15 == 0) 
    {
        ServerState::agent_list->update_map_manager_positions();
        t_map::t_map_manager_update();
        t_map::t_map_sort_map_chunk_ques();
    }

    Toolbelt::tick();

    ServerState::agent_list->update_models(); // sets skeleton
    
    Particle::grenade_list->tick();
    ItemParticle::tick();

    Objects::tick();
    Objects::harvest();
    Objects::update();

    Objects::spawn_mobs();

    Components::rate_limit_component_list->call(); // advance rate limiter ticks

    if (counter % 10 == 0) ItemParticle::check_item_pickups();
    if (counter % 6  == 0) ItemContainer::check_agents_in_container_range();

    ServerState::ctf->check_agent_proximities();
    ServerState::ctf->update();
    ServerState::ctf->tick();

    //ServerState::spawn_items(2);
    ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 120);
    ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 8);

    ItemContainer::digest_nanite_food();
    Toolbelt::update_toolbelt_items();
}
 
int run()
{
    //int tick = 0;
    int tc;

    while (1)
    {
        tc = 0;
        while(1)
        {
            int ti = _GET_TICK();
            if(ti == 0 || tc > 1) break;

            tick();

            tc++;
            break;
        }

        if(tc > 0)
        {
            NetServer::flush_to_net();
        }

        if(tc > 1)
        {
            printf("Warning:: %i ticks this frame", tc);
        }
        NetServer::dispatch_network_events();

        usleep(1000);
    }
    close_c_lib();
    return 0;
}

}
