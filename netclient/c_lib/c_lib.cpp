#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

#include <c_lib.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Common headers */
#include "defines.h"
#include "compat_gl.h"
#include "compat_al.h"


/* Compression */
#include <c_lib/common/compression/miniz.c>

/* configuration options */
#include <c_lib/option_macros.hpp>
#include <c_lib/options.cpp>

/* Network */
#include <net_lib/net_lib.h>

//utility
#include <c_lib/common/macros.hpp>
#include <c_lib/common/gl_assert.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/common/common.cpp>
#include <c_lib/common/quicksort.hpp>
#include <c_lib/common/files.cpp>

// logging
#include <c_lib/common/logger.cpp>

// time
#include <c_lib/time/physics_timer.c>
#include <c_lib/time/frame_rate_limiter.cpp>

/* Vectors, Ray Tracers, Physics */
#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>
#include <c_lib/physics/mat3.hpp>
#include <c_lib/physics/mat4.hpp>
#include <c_lib/physics/affine.hpp>

#include <c_lib/physics/color_matrix.cpp>
#include <c_lib/ray_trace/ray_trace.cpp>
#include <c_lib/ray_trace/hitscan.cpp>
#include <c_lib/physics/common.cpp>
#include <c_lib/physics/verlet.cpp>

/* Voxel Models */
#include <c_lib/voxel/voxel_volume.cpp>
#include <c_lib/voxel/voxel_render.cpp>
#include <c_lib/voxel/voxel_hitscan.cpp>
#include <c_lib/voxel/voxel_dat.cpp>
#include <c_lib/voxel/voxel_model.cpp>
#include <c_lib/voxel/voxel_loader.cpp>
#include <c_lib/voxel/vox_dat_init.cpp>

/* Entity system */
#include <c_lib/entity/entity.cpp>

/* particles */
#include <c_lib/particles/particle_lib.cpp>

/* Items */
#include <c_lib/items/constants.hpp>
#include <c_lib/items/items.cpp>

/* Skybox */
#include <c_lib/camera/skybox/skybox.cpp>

/* Game Stuff */
#include <c_lib/game/game.cpp>
#include <c_lib/game/teams.cpp>
#include <c_lib/game/ctf.cpp>
#include <c_lib/game/packets.cpp>

/* Agents */
#include <c_lib/agent/agent_include.h>

/* Monsters */
#include <c_lib/monsters/packets.cpp>
#include <c_lib/monsters/monsters.cpp>

/* Weapons */
#include <c_lib/weapons/weapons.cpp>
#include <c_lib/weapons/packets.cpp>

/* animations */
#include <c_lib/animations/animations.cpp>
#include <c_lib/animations/hitscan.cpp>
#include <c_lib/animations/hitscan_laser.cpp>

/* Profiling */

#include <c_lib/common/profiling/texture_surface.cpp>
#include <c_lib/common/profiling/frame_graph.cpp>
#include <c_lib/common/profiling/map_vbo_graph.cpp>

/* LUA */

#include <c_lib/common/lua/lua.cpp>

/* map */

#include <c_lib/t_map/t_map_class.cpp>
#include <c_lib/t_map/t_map.cpp>
#include <c_lib/t_map/t_properties.cpp>

#include <c_lib/t_map/t_vbo.cpp>
#include <c_lib/t_map/t_vbo_draw.cpp>
#include <c_lib/t_map/t_vbo_update.cpp>

#include <c_lib/t_map/glsl/cache.cpp>
#include <c_lib/t_map/glsl/shader.cpp>
#include <c_lib/t_map/glsl/texture.cpp>

#include <c_lib/t_map/net/t_CtoS.cpp>
#include <c_lib/t_map/net/t_StoC.cpp>

/* mechanisms */

#include <c_lib/t_mech/state.cpp>
#include <c_lib/t_mech/draw.cpp>

//#include <c_lib/t_map/t_viz.c>
//#include <c_lib/t_map/t_vbo.c>

/* camera */
#include <c_lib/camera/camera.cpp>
#include <c_lib/camera/fulstrum_test.cpp>
#include <c_lib/camera/skybox.cpp>

/* SDL */
#include <c_lib/SDL/shader_loader.cpp>
#include <c_lib/SDL/texture_loader.cpp>
#include <c_lib/SDL/texture_sheet_loader.cpp>
#include <c_lib/SDL/draw_functions.cpp>
#include <c_lib/SDL/particle_functions.c>
#include <c_lib/SDL/SDL_functions.c>

//#ifdef linux
//#include <c_lib/SDL/IMG_savepng.c>
//#endif

/* HUD */

//#include <c_lib/hud/font/font_loader.hpp> /* TEMP */
//#include <c_lib/hud/font/struct.hpp>

#include <c_lib/hud/reticle.cpp>
#include <c_lib/hud/cube_selector.cpp>
#include <c_lib/hud/inventory.cpp>
#include <c_lib/hud/font.cpp>

#include <c_lib/hud/text.cpp>
#include <c_lib/hud/map.cpp>
#include <c_lib/hud/equipment.cpp>
#include <c_lib/hud/compass.cpp>
#include <c_lib/hud/hud.cpp>

/* input */
#include <c_lib/input/input.cpp>
#include <c_lib/input/handlers.cpp>
#include <c_lib/input/skeleton_editor.cpp>

/* sound */
#include <c_lib/sound/wav.cpp>
#include <c_lib/sound/triggers.cpp>
#include <c_lib/sound/csv_parser.cpp>
#include <c_lib/sound/fmod.cpp>
#include <c_lib/sound/openal.cpp>
#include <c_lib/sound/sound.cpp>

/* chat */
#include <c_lib/chat/globals.hpp>
#include <c_lib/chat/packets.cpp>
#include <c_lib/chat/client.cpp>

/* client side map gen / utils */
//#include <c_lib/map_gen/hopcroft-karp.cpp>
//#include <c_lib/map_gen/dragon.cpp>

#include <c_lib/state/client_state.cpp>
#include <c_lib/state/packet_init.cpp>

//#include <c_lib/SDL/pngfuncs.c>
//#include <c_lib/SDL/v.cpp>

//page size
#include <unistd.h>
//(size_t) sysconf(_SC_PAGESIZE);

#include <c_lib/main.cpp>

/*
    init_t_map()
    init_cube_properties()
    init_cube_side_texture()
    set_hud_cube_selector()
    init_for_draw()
*/

int init_c_lib() {
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: Attempt to init c_lib more than once\n");
        return 1;
    }
    
    Log::init();

    //printf("System page size= %li \n", sysconf(_SC_PAGESIZE) );
    printf("init c_lib\n");


    srand(time(NULL));   // seed the RNG

    init_video();
    init_image_loader();
    TextureSheetLoader::init();
    t_map::init_t_map();
    lua_load_block_dat(); /* Load Map Tiles */
    t_map::init_for_draw();

    t_mech::draw_init();
    t_mech::state_init();

    Sound::init();
    //Sound::test();

    Skybox::init();

    HudText::init();
    HudFont::init();
    HudMap::init();
    HudInventory::init();
    HudEquipment::init();
    HudReticle::init();
    HudCubeSelector::init();
    Compass::init();
    Hud::init();
    //vn::init();

    ClientState::init_lists();
    ClientState::init();

    init_vox_dats();
    init_voxel_volume();
    
    init_network();
    init_net_client();
    
    init_input();
    init_handlers();
    init_cameras();
    init_chat_client();
    init_particles();
    
    Animations::init_hitscan();
    Animations::init_hitscan_laser();
    
    return 0;
}


void close_c_lib() {
    printf("Closing game...\n");

/*
    LOOK AT NEXT LINE
*/
    exit(0);  

    t_map::end_t_map();
    t_map::end_t_vbo();

    t_mech::draw_teardown();
    t_mech::state_teardown();

    Skybox::teardown();

    TextureSheetLoader::teardown();

    shutdown_net_client();
    teardown_cameras();
    HudFont::teardown();
    teardown_chat_client();

    ClientState::teardown();
    ClientState::teardown_lists();
    teardown_voxel_volume();
    HudText::teardown();

    // free surfaces
    t_map::teardown_shader();
    HudMap::teardown();
    //vn::teardown();
    Sound::close();
    close_SDL();

    printf("Game closed\n");

    Log::teardown();
}
