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

    if (Options::map[0] == '\0')
    {   // use map gen
        srand(Options::seed);

        t_gen::noise_map_generate_map();
        
        t_map::map_post_processing(); //regolith stuff
        t_gen::generate_rock_layer();
        t_gen::start_cave_generator();
        t_gen::populate_ore();

        //t_gen::gen_rivers(60, "terminal_blue");

        //map_gen::floor(512,512,0,1, t_map::get_cube_id((char*)"regolith"));
        map_gen::rough_floor(512,512,0,3, t_map::dat_get_cube_id("bedrock"));
        //Dragon::caves();
        //Dragon::flat_veins();
    }
    else if (!strcmp(Options::map, "fast"))
    {
        map_gen::floor(512,512,0,1, t_map::dat_get_cube_id("bedrock"));
        map_gen::floor(512,512,1,9, t_map::dat_get_cube_id("regolith"));

        map_gen::floor(512,512, 20,1, t_map::dat_get_cube_id("regolith"));
    }
    else
    {   // use map file
        t_map::load_map(Options::map);
    }   

    srand((unsigned int)time(NULL));
    
    int address[4];
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    #if !PRODUCTION
    //Item::test_item_list_capacity();
    //ItemContainer::test_container_list_capacity();
    //Objects::stress_test();
    #endif

    ServerState::init_base();

    main_inited = true;
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
    Components::agent_targeting_component_list->call(); // update target lock ticks

    if (counter % 10 == 0) ItemParticle::check_item_pickups();
    if (counter % 6  == 0) ItemContainer::check_agents_in_container_range();
    if (counter % 6  == 0)
    {
        Components::healer_component_list->call();
        ServerState::check_agents_at_base();
    }

    //ServerState::spawn_items(2);
    //ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 50);
    //ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 6);

    ItemContainer::update_smelters();
    Item::tick();

    t_map::environment_process_tick(); //refresh regolith etc...
}
 
int run()
{
    //int tick = 0;
    int tc;

    while (!signal_exit)
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

        if (should_save_map)
        {
            t_map::save_map();
            should_save_map = false;
        }

        #ifdef __GNUC__
        usleep(1000);
        #endif
    
        #ifdef __MSVC__
        Sleep(1);
        #endif
    }

    if (should_save_map)
    {
        t_map::save_map();
        should_save_map = false;
    }
    
    close_c_lib();
    return 0;
}

}
