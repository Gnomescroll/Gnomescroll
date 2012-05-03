#pragma once

#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
#include <string.h>
#include <math.h>

/* Common headers */
#include "defines.h"
#include "compat_gl.h"
#include "compat_al.h"


/* Compression */
#include <common/compression/miniz.c>

/* Templates */
 
#include <common/template/object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>

/* configuration options */
#include <common/option_macros.hpp>
#include <options.cpp>

/* Network */
#include <net_lib/net_lib.h>

//utility
#include <common/gl_assert.hpp>
#include <common/list_assert.hpp>
#include <common/asserts.hpp>
#include <common/common.cpp>
#include <common/files.cpp>

// logging
#include <common/logger.cpp>

// time 
#include <common/time/physics_timer.cpp>

/* Vectors, Ray Tracers, Physics */
#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>
#include <physics/affine.hpp>

#include <physics/color_matrix.cpp>
#include <physics/ray_trace/ray_trace.cpp>
#include <physics/ray_trace/hitscan.cpp>
#include <physics/ray_trace/handlers.cpp>
#include <physics/common.cpp>
#include <physics/verlet.cpp>
#include <physics/verlet_particle.cpp>
#include <physics/motion.cpp>

/* SDL */
#include <SDL/shader_loader.cpp>
#include <SDL/texture_loader.cpp>
#include <SDL/texture_sheet_loader.cpp>
#include <SDL/draw_functions.cpp>
#include <SDL/particle_functions.c>
#include <SDL/SDL_functions.c>

/* Draw lists */
#include <draw/constants.hpp>
#include <draw/draw.cpp>
#include <draw/items.cpp>
#include <draw/lists.cpp>

/* Voxel Models */
#include <voxel/voxel_volume.cpp>
#include <voxel/voxel_render.cpp>
#include <voxel/voxel_hitscan.cpp>
#include <voxel/voxel_dat.cpp>
#include <voxel/voxel_model.cpp>
#include <voxel/voxel_loader.cpp>
#include <voxel/vox_dat_init.cpp>

/* Entity system */
#include <entity/include.cpp>

/* particles */
//#include <particle/particle_lib.cpp>
#include <particle/_include.hpp>

/* Items */
#include <game/constants.hpp>

/* Skybox */
#include <camera/skybox/skybox.cpp>

/* Game Stuff */
#include <game/flag.cpp>
#include <game/base.cpp>
#include <game/game.cpp>
#include <game/teams.cpp>
#include <game/ctf.cpp>
#include <game/packets.cpp>

/* Agents */
#include <agent/agent_include.h>

/* Monsters */
//#include <monsters/monsters.cpp>

/* Weapons */
#include <weapons/weapons.cpp>
#include <weapons/packets.cpp>

/* animations */
#include <animations/_include.hpp>

/* Profiling */

#include <common/profiling/texture_surface.cpp>
#include <common/profiling/frame_graph.cpp>
#include <common/profiling/map_vbo_graph.cpp>

/* LUA */

#include <common/lua/lua.cpp>

/* map */

#include <t_map/_include.hpp>



#include <t_gen/_include.hpp>

/* mechanisms */
#include <t_mech/_include.hpp>

/* items */
#include <item/_include.hpp>

/* toolbelt */
#include <toolbelt/_include.hpp>

/* hud */
#include <t_hud/_include.hpp>

/* camera */
#include <camera/camera.cpp>
#include <camera/fulstrum_test.cpp>
 
//#ifdef linux
//#include <SDL/IMG_savepng.c>
//#endif

/* HUD */

//#include <hud/font/font_loader.hpp> /* TEMP */
//#include <hud/font/struct.hpp>

#include <hud/constants.hpp>

#include <hud/reticle.cpp>
#include <hud/cube_selector.cpp>
#include <hud/font.cpp>

#include <hud/text.cpp>
#include <hud/map.cpp>
#include <hud/equipment.cpp>
#include <hud/compass.cpp>
#include <hud/hud.cpp>

/* input */
#include <input/input.cpp>
#include <input/handlers.cpp>
#include <input/skeleton_editor.cpp>

/* sound */
#include <sound/wav.cpp>
#include <sound/triggers.cpp>
#include <sound/csv_parser.cpp>
#include <sound/openal.cpp>
#include <sound/sound.cpp>

/* chat */
#include <chat/packets.cpp>
#include <chat/client.cpp>

#include <chat/interface.cpp>

/* client side map gen / utils */
//#include <map_gen/hopcroft-karp.cpp>
//#include <map_gen/dragon.cpp>

#include <state/client_state.cpp>
#include <state/packet_init.cpp>

//#include <unistd.h>

int init_c_lib() 
{
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: Attempt to init c_lib more than once\n");
        return 0;
    }
    Log::init();
    printf("init c_lib\n");

    LUA::load_options(); //load game options
    srand(time(NULL));   // seed the RNG

    Components::init();
    Objects::init_net_interfaces();
    Objects::init();    // Entity system

    _set_resolution(Options::width, Options::height, Options::fullscreen);
    init_video();
    Sound::init();

    init_image_loader();
    TextureSheetLoader::init();
    Draw::init();
    t_map::init_t_map();
    HudCubeSelector::init();
    lua_load_block_dat();  /* Load Block Dat */
    t_map::init_for_draw();


    Item::init();

// Load Dats

    //t_map::load_map_dat();
    Item::load_item_dat();

    t_map::load_map_drop_dat(); //load drop dat after items

    Toolbelt::init();

    t_hud::init();
    t_hud::draw_init();

    t_mech::draw_init();
    //t_mech::state_init();

    Particle::init_particles();

    Skybox::init();
    //Sound::test();

    HudText::init();
    HudFont::init();
    HudMap::init();

    HudEquipment::init();
    HudReticle::init();
    Compass::init();
    Hud::init();
    //vn::init();

    ClientState::init();

    VoxDats::init();
    init_voxel_volume();
    

    init_network();
    NetClient::init_net_client();
    
    init_input();
    init_handlers();
    init_cameras();
    init_chat_client();
    Particle::draw_init();
    init_item_texture();

    ClientState::init_ctf();
    
    Animations::init();

    return 0;
}

void close_c_lib() {
    printf("Closing game...\n");
 
    t_map::end_t_map();

    Item::teardown();
    Toolbelt::teardown();

    t_hud::draw_teardown();
    t_hud::teardown();
    t_mech::draw_teardown();
    //t_mech::state_teardown();

    Particle::teardown_particles();

    Skybox::teardown();

    TextureSheetLoader::teardown();

    NetClient::shutdown_net_client();

    teardown_cameras();
    HudFont::teardown();
    teardown_chat_client();

    ClientState::teardown();
    teardown_voxel_volume();
    HudText::teardown();
    teardown_item_texture();
    Draw::teardown();
    
    // free surfaces
    t_map::teardown_shader();
    HudMap::teardown();
    //vn::teardown();
    Sound::close();
    close_SDL();

    Objects::teardown();    // Entity system
    Components::teardown();
    Objects::teardown_net_interfaces();

    printf("Game closed\n");

    Log::teardown();
}
