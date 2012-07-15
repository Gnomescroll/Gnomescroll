#pragma once

#include <common/time/physics_timer.hpp>
#include <map_gen/map_generator.hpp>
#include <map_gen/recipes.hpp>

#include <t_gen/_interface.hpp>
#include <t_map/_interface.hpp>

namespace Main
{



void init(int argc, char* argv[])
{
    init_c_lib(argc, argv);

    if (0)
    {
        MapGen::init();
        MapRecipes::simple_map();
        MapGen::teardown();
        Dragon::caves();
        Dragon::surface_veins();

        t_map::map_post_processing();
    }
    else
    {
        srand(Options::seed);

        t_gen::noise_map_generate_map();
        
        t_map::map_post_processing(); //regolith stuff
        t_gen::start_cave_generator();
        t_gen::populate_ore();

        //map_gen::floor(512,512,0,1, t_map::get_cube_id((char*)"regolith"));
        map_gen::rough_floor(512,512,0,3, t_map::get_cube_id((char*)"bedrock"));
        //Dragon::caves();
        //Dragon::flat_veins();
    }   

    srand((unsigned int)time(NULL));
    
    //t_gen::gen_map();

    int address[4];
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    #if !PRODUCTION
    //Item::test_item_list_capacity();
    //ItemContainer::test_container_list_capacity();
    //Objects::stress_test();
    #endif

    ServerState::init_base();

    printf("Game starting\n");
}
   
void tick()
{
    static int counter = 0; counter ++;

    t_map::t_map_send_map_chunks();  //every tick

    if(counter % 15 == 0) 
    {
        ServerState::agent_list->update_map_manager_positions();
        t_map::t_map_manager_update();
        //t_map::t_map_sort_map_chunk_ques();
    }

    Toolbelt::update_toolbelt_items();
    Toolbelt::tick();

    ServerState::agent_list->update_models(); // sets skeleton
    
    Particle::grenade_list->tick();
    ItemParticle::tick();

    Objects::tick();
    Objects::harvest();
    Objects::update();

    Objects::spawn_mobs();

    Components::rate_limit_component_list->call(); // advance rate limiter ticks
    Components::motion_targeting_component_list->call(); // update target lock ticks

    if (counter % 10 == 0) ItemParticle::check_item_pickups();
    if (counter % 6  == 0) ItemContainer::check_agents_in_container_range();
    if (counter % 6  == 0) ServerState::check_agents_at_base();

    //ServerState::spawn_items(2);
    ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 100);
    ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 8);

    ItemContainer::update_smelters();
    Item::item_list->tick();
    Item::item_list->verify_items();


    t_map::environment_process_tick(); //refresh regolith etc...
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

    #ifdef __GNUC__
        usleep(1000);
    #endif
    
    #ifdef __MSVC__
        Sleep(1);
    #endif
    }
    close_c_lib();
    return 0;
}

}
