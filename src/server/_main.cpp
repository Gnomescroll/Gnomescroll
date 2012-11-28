#pragma once

#include <common/time/physics_timer.hpp>

#include <t_gen/_interface.hpp>
#include <t_map/_interface.hpp>

#include <net_lib/server.hpp>

namespace Main
{


void default_map_gen()
{
    t_gen::noise_map_generate_map();

    t_map::map_post_processing(); //regolith stuff
    t_gen::generate_rock_layer();
    t_gen::start_cave_generator();
    t_gen::populate_ore();
    t_gen::generate_ruins();
    t_gen::add_terrain_features();

    map_gen::rough_floor(XMAX,YMAX,0,3, t_map::get_cube_id("bedrock"));
}

void init_world()
{
    srand(Options::seed);

    bool new_map = false;
    bool valgrind_map = false;
    bool corpusc = false;
    bool art_map = false;

    if (strcmp(Options::map, "valgrind") == 0)
        valgrind_map = true;
    else
    if (strcmp(Options::map, "new") == 0)
        new_map = true;
    else
    if (strcmp(Options::map, "art") == 0)
        art_map = true;
    else
    if (strcmp(Options::map, "corpusc") == 0)
        corpusc = true;
    else
    if (serializer::load_data())
    {
        if (Options::serializer)
        {   // only resave the data if we're in serializer mode
            bool saved = serializer::save_data();    // re-save immediately after loading, so that palette changes are up to date
            GS_ASSERT_ABORT(saved);
            serializer::wait_for_save_complete();
        }
    }
    else
        new_map = true;

    if (new_map)
    {
        serializer::begin_new_world_version();
        default_map_gen();
        t_gen::populate_crystals();
        t_map::environment_process_startup();
        if (Options::serializer)
        {
            bool saved = serializer::save_data();
            GS_ASSERT_ABORT(saved);
            serializer::wait_for_save_complete();
        }
    }
    else
    if (corpusc)
    {
        map_gen::floor(XMAX,YMAX,0, 1, t_map::get_cube_id("bedrock"));
        t_gen::set_region(0,0,1, XMAX,YMAX,ZMAX/2, t_map::get_cube_id("regolith") );
        t_gen::excavate();
        t_gen::add_terrain_features();
    }
    else
    if (valgrind_map)
    {
        map_gen::floor(XMAX,YMAX,0, 1, t_map::get_cube_id("bedrock"));
        map_gen::floor(XMAX,YMAX,1, 9, t_map::get_cube_id("regolith"));
        map_gen::floor(XMAX,YMAX,20,ZMAX-20, t_map::get_cube_id("regolith"));
    }
    else
    if (art_map)
    {
        // corpus do it here
    }

    srand((unsigned int)time(NULL));
}

void init(int argc, char* argv[])
{
    init_c_lib(argc, argv);

    init_world();

    int address[4] = {0};
    address_from_string(Options::ip_address, address);
    NetServer::init_server(address[0],address[1],address[2],address[3], Options::port);

    ServerState::init_base();

    ServerState:: main_inited = true;
    printf("Game started\n");
}

void tick()
{
    static int counter = 0;

    t_map::t_map_send_map_chunks();  //every tick

    if (counter % 15 == 0)
    {
        Agents::agent_list->update_map_manager_positions();
        t_map::t_map_manager_update();
        //t_map::t_map_sort_map_chunk_ques();
    }

    Toolbelt::update_toolbelt_items();
    Toolbelt::tick();

    Agents::agent_list->update_models(); // sets skeleton

    Particle::grenade_list->tick();
    ItemParticle::tick();

    Entities::tick();
    Entities::harvest();
    Entities::update();

    Entities::spawn_mobs();

    ServerState::move_base();

    Components::rate_limit_component_list->call(); // advance rate limiter ticks
    Components::motion_targeting_component_list->call(); // update target lock ticks
    Components::agent_targeting_component_list->call(); // update target lock ticks

    if (counter % 10 == 0) ItemParticle::check_item_pickups();
    if (counter % 6  == 0)
    {
        ItemContainer::check_agents_in_container_range();
        Components::healer_component_list->call();
        ServerState::check_agents_at_base();
    }
    if (counter % 30000 == 1)
    {
        //t_gen::meteor_fall();
    }

    //ServerState::spawn_items(2);
    ServerState::spawn_monsters(OBJECT_MONSTER_BOMB, 50);
    ServerState::spawn_monsters(OBJECT_MONSTER_SPAWNER, 6);

    ItemContainer::update_smelters();
    Item::tick();

    t_map::environment_process_tick(); //refresh regolith etc...

    Auth::update(); // do it here because i need constant timer

    counter++;
}

int run()
{
    while (!ServerState::signal_exit)
    {
        int tc = 0;
        if (_GET_TICK())
        {
            tick();
            tc++;
        }

        if (tc > 0)
            NetServer::flush_to_net();
        if (tc > 1)
            printf("Warning:: %d ticks this frame", tc);

        NetServer::dispatch_network_events();
        if (Options::auth)
            NetServer::check_client_authorizations();

        if (serializer::should_save_world)
        {
            bool saved = serializer::save_data();
            GS_ASSERT(saved);
            serializer::should_save_world = false;
        }

        serializer::update();

        // update the world save, else sleep
        if (serializer::update_save_state(2) == WORLD_SAVE_IDLE)
            gs_millisleep(1);

        if (ServerState::reload_settings)
        {
            LUA::load_options();
            ServerState::reload_settings = false;
        }
    }

    if (serializer::should_save_world)
    {
        bool saved = serializer::save_data();
        GS_ASSERT(saved);
        serializer::wait_for_save_complete();
        serializer::should_save_world = false;
    }

    return 0;
}

}
