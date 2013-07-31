#pragma once

#ifndef DC_CLIENT
# define DC_CLIENT 1
#endif

#ifndef GS_AWESOMIUM
# define GS_AWESOMIUM 0
#endif

#ifndef GS_AUTH
# define GS_AUTH 0
#endif

#ifndef USE_LIVE_AUTH
# define USE_LIVE_AUTH 0
#endif

//#if GS_AUTH && !GS_AWESOMIUM
//# error GS_AUTH is enabled by GS_AWESOMIUM is not
//#endif

#ifndef GS_ASSIMP
# define GS_ASSIMP 0
#endif

#if PRODUCTION
# ifndef GS_AWESOMIUM
#  define GS_AWESOMIUM 0
# endif
#endif

#ifndef ENABLE_WARNING_SOUND
# define ENABLE_WARNING_SOUND 1
#endif

#if PRODUCTION
# define ENABLE_WARNING_SOUND 0
#endif

#ifdef GS_SERIALIZER
# error Do not define GS_SERIALIZER in the client
#endif

#ifdef DC_SERVER
# error Do not include this file in the server
#endif

#ifndef LINUX2WINDOWS
# ifdef _WIN32
//#  define WINVER _WIN32_WINNT_WINXP
//#  define _WIN32_WINNT _WIN32_WINNT_WINXP
#  ifndef NTDDI_VERSION
#   define NTDDI_VERSION NTDDI_WIN2K
#  endif
#  ifndef _WIN32_IE_IE50
#   define _WIN32_IE_IE50 0x0500
#  endif
#  ifndef _WIN32_IE
#   define _WIN32_IE _WIN32_IE_IE50
#  endif
# endif
#endif

#ifdef _WIN32
// include this here, so that winsock2.h is included before windows.h
# include <net_lib/enet/win32.h>
#endif

#ifdef __MSVC__
# include "stdafx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#ifndef _WIN32
# include <sys/types.h>
# include <unistd.h>
#endif

#ifdef __MINGW32__
# include <malloc.h> //alloca function
#endif

#ifdef _WIN32
# include <tchar.h>
# include "windows.h"
# include <shlobj.h>
# undef rad2
# undef interface
#endif

#ifdef __APPLE__
# include <common/osx.hpp>
#endif

// Vars related to internal startup
bool c_lib_inited = false;

/* Common headers */
#include <common/version.h>
#include <common/compat_gl.h>
#include <common/compat_al.h>
#include <common/defines.h>
#include <common/macros.hpp>
#include <common/crash_report/stack_trace.hpp>
#include <common/gs_assert.cpp>
#include <common/mallox.hpp>
#include <common/clang_warnings.h>
#include <common/compression/miniz.c>
#include <common/compression/files.cpp>
#include <common/template/object_list.hpp>
#include <common/template/multi_object_list.hpp>
#include <common/template/simple_object_list.hpp>
#include <common/template/object_pool.hpp>
#include <common/dat/name_map.hpp>
#include <common/dat/attributes.cpp>
#include <common/dat/modifiers.hpp>
#include <common/dat/properties.hpp>
#include <common/json/jsmn.c>
#include <common/settings_export.hpp>
#include <common/dirent.h>

/* configuration options */
#include <options/option_macros.hpp>
#include <options/argparse.cpp>
#include <options/client_options.cpp>

/* Network */
#include <net_lib/_include.hpp>

//utility
#include <common/gl_assert.hpp>
#include <common/macros.hpp>
#include <common/common.cpp>
#include <common/color.hpp>
#include <common/files.cpp>
#include <common/config_file.hpp>
// logging
#include <common/logger.cpp>

// time
#include <common/time/physics_timer.cpp>

/* Physics */
#include <physics/_include.hpp>

/* SDL */
#include <SDL/shader_loader.cpp>
#include <SDL/texture_loader.cpp>
#include <SDL/texture_sheet_loader.cpp>
#include <SDL/draw_functions.cpp>
#include <SDL/SDL_functions.cpp>
#include <SDL/png_save.cpp>

#include <SDL/haldCLUT/hald_clut.hpp> //should be .cpp

//#include <SDL/mesh_loader/obj_loader.cpp>

#include <t_mob/_include.hpp>

/* Draw utils */
#include <common/draw/draw.cpp>
#include <common/draw/voxel.cpp>
#include <common/draw/colored_voxel.cpp>
#include <common/draw/textured_voxel.cpp>

/* Entity system */
#include <entity/_include.cpp>

/* Voxel Models */
#include <voxel/voxel_volume.cpp>
#include <voxel/voxel_render.cpp>
#include <voxel/voxel_hitscan.cpp>
#include <voxel/voxel_dat.cpp>
#include <voxel/voxel_model.cpp>
#include <voxel/voxel_loader.cpp>
#include <voxel/vox_dat_init.cpp>

/* particles */
#include <particle/_include.hpp>

/* Skybox */
#include <camera/skybox/skybox.cpp>
#include <camera/skybox/rayleigh_scattering.hpp>

/* Agents */
#include <agent/_include.hpp>

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

/* mechanisms */
#include <t_mech/_include.hpp>

/* items */
#include <item/_include.hpp>
#include <item/container/_include.hpp>
#include <item/toolbelt/_include.hpp>
#include <item/particle/_include.hpp>

/* hud */
#include <hud/container/_include.hpp>

/* camera */
#include <camera/camera.cpp>
#include <camera/fulstrum_test.cpp>
#include <camera/fulstrum_test2.cpp>

/* HUD */

#include <hud/_include.hpp>

/* input */
#include <input/input.cpp>
#include <input/handlers.cpp>
#include <input/skeleton_editor.cpp>
#include <input/equipped_sprite_adjuster.cpp>

/* sound */
#include <sound/_include.hpp>

/* chat */
#include <chat/_include.hpp>

/* Awesomium */
#include <SDL/awesomium/_include.hpp>

#include <state/client_state.cpp>
#include <state/packet_init.cpp>
#include <state/attributes.cpp>

// authentication
#include <auth/_include.hpp>

// social (clans, badges, etc)
#include <social/_include.hpp>

bool signal_exit = false;

#ifdef linux
# include <signal.h>

void signal_terminate_handler(int sig)
{
    signal_exit = true;
}
#endif

void close_c_lib(); // forward declaration
void atexit_handler()
{
    close_c_lib();
}

void print_working_directory()
{
    #ifdef linux
    // printf working directory
    const size_t DIR_SIZE = 1024;
    char* wd = (char*)calloc((DIR_SIZE+1), sizeof(char));
    char* wdr = getcwd(wd, DIR_SIZE);
    IF_ASSERT(wdr == NULL)
        printf("Failed to get working directory.\n");
    else
        printf("Working directory is: %s\n", wd);
    free(wd);
    #endif
}

void create_system_data_dir()
{
    char* home = NULL;
    get_home_directory(home);
    if (home == NULL)
    {
        const char _home[] = ".";
        home = (char*)calloc(sizeof(_home), sizeof(*home));
        strcpy(home, _home);
    }

    char* data_path = (char*)calloc(NAME_MAX+1, sizeof(char));
    size_t ret = snprintf(data_path, NAME_MAX+1, "%s/%s", home, SYSTEM_DATA_FOLDER);
    GS_ASSERT(ret < NAME_MAX + 1);
    data_path[NAME_MAX] = '\0';
    ClientState::active_system_data_path = data_path;
    if (!file_exists(data_path))
        create_path(data_path);
    printf("Active system data path set to: %s\n", ClientState::active_system_data_path);

    free(home);
}

void register_signals()
{
    int ret = atexit(&atexit_handler);
    GS_ASSERT_ABORT(ret == 0);

    #ifdef linux
    // Set signal handlers

    // SIGTERM  kill `pidof gnomescroll_server`
    struct sigaction sa_term;
    sa_term.sa_handler = &signal_terminate_handler;
    sa_term.sa_flags = 0;
    sigemptyset(&sa_term.sa_mask);
    ret = sigaction(SIGTERM, &sa_term, NULL);
    GS_ASSERT(ret == 0);

    // SIGINT ctrl-C
    ret = sigaction(SIGINT, &sa_term, NULL);
    GS_ASSERT(ret == 0);

    // SIGUSR1 ignore
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = SIG_IGN;
    sa_usr1.sa_flags = 0;
    sigemptyset(&sa_usr1.sa_mask);
    ret = sigaction(SIGUSR1, &sa_usr1, NULL);
    GS_ASSERT(ret == 0);
    #endif
}

void init_configs()
{
    Animations::init_config();
    Badges::register_badges();
    Agents::register_attributes();
    World::register_attributes();

    // DAT LOADING
    // HIGHLY ORDER SENSTITIVE
    ItemContainer::init_config();
    t_map::init_t_properties();
    t_mech::init_properties();

    t_map::load_block_dat();
    t_map::init_for_draw();
    t_map::blit_block_item_sheet();
    t_mech::load_mech_dat();
    ItemContainer::load_config();
    ItemContainer::end_config();
    Item::init_properties();
    Item::load_item_dat();

    t_map::init_block_drop_dat();
    t_map::load_block_drop_dat();

    Item::create_items_from_blocks();
    Item::end_item_dat();

    Item::load_synthesizer();
    Item::load_crafting_dat();
    Item::load_smelting_dat();

    t_mob::load_config();

    Entities::init();
    Components::init_config();

    Attributes::verify();
}

int init_c_lib(int argc, const char* argv[])
{
    /*
        Time startup functions to determine delay/slow down
    */
    GS_ASSERT(!c_lib_inited);

    #ifdef linux
    print_working_directory();
    register_signals();
    #endif

    _test_common();

    create_path(SCREENSHOT_PATH);
    #if !PRODUCTION
    // this path is for build/debug data
    create_path(DATA_PATH);
    #endif

    create_system_data_dir();

    Log::init();
    printf("init c_lib\n");

    AgentHudName::verify_configuration();   // test agent constant parameters

    GS_ASSERT(quadrant_translate_f(500,30) == 542);
    GS_ASSERT(quadrant_translate_f(10,500) == -12);

    Options::init_option_tables();
    LUA::init_options();
    Options::register_options();
    LUA::load_options(); //load game options

    // parse any command line arguments, overriding settings files
    Options::parse_args(argc, argv);

    // check final values for bounds etc
    Options::validate();

    update_camera_settings(Options::view_distance);

    srand((unsigned int)time(NULL));

    Attributes::init();
    Agents::init_attributes();
    World::init_attributes();
    Badges::init();
    Components::init();
    Entities::init_net_interfaces();
    Chat::init_chat_client();

    init_network();
    NetClient::init_net_client();

    Agents::init();
    ClientState::init_lists();

    Auth::init();

    init_color_data();

    _set_resolution(Options::width, Options::height, Options::fullscreen);
    init_video();

    init_image_loader();
    TextureSheetLoader::init();

    Skybox::init_rayleigh_scattering(); // init skybox

    Awesomium::init();

    HudText::init();
    HudMap::init();
    HudFont::init();
    HudCubeSelector::init();

    t_map::init_t_map();
    t_map::generate_light_texture();

    t_mob::init();

    init_configs();

    // This block MUST come after dat loaders. possibly others
    Item::init();
    ItemContainer::init();
    Toolbelt::init();   // toolbelt init depends on item dat being loaded
    TextureSheetLoader::init_greyscale();   //item sheet grey scale textures
    TextureSheetLoader::init_item_texture();
    TextureSheetLoader::save_item_texture();

    t_mech::init();

    HudContainer::init();
    HudContainer::draw_init();

    Particle::init_particles();
    ItemParticle::init();

    Skybox::init();

    VoxDats::init();
    Voxels::init_voxel_volume();

    Sound::init();
    init_input();
    init_cameras();
    Particle::draw_init();

    Animations::init();
    Animations::load_sprite_voxelizer();

    Hud::init();

    Voxels::init_voxel_render_list_shader1();   //used to be called from ClientState::init

    Path::init();

    CHECK_GL_ERROR();

    c_lib_inited = true;
    return 0;
}

void close_c_lib()
{
    #define TEARDOWN_DEBUG 0

    printf("Closing game...\n");

    if (TEARDOWN_DEBUG) printf("t_map teardown\n");
    t_map::end_t_properties();
    t_map::end_t_map();
    t_map::teardown_block_drop_dat();

    if (TEARDOWN_DEBUG) printf("HudContainer draw teardown\n");
    HudContainer::draw_teardown();
    if (TEARDOWN_DEBUG) printf("HudContainer teardown\n");
    HudContainer::teardown();
    if (TEARDOWN_DEBUG) printf("t_mech teardown\n");
    t_mech::teardown_properties();
    t_mech::teardown();
    if (TEARDOWN_DEBUG) printf("particle draw teardown\n");
    Particle::draw_teardown();

    if (TEARDOWN_DEBUG) printf("item particle teardown\n");
    ItemParticle::teardown();

    if (TEARDOWN_DEBUG) printf("skybox teardown\n");
    Skybox::teardown();

    if (TEARDOWN_DEBUG) printf("Rayleigh scattering teardown\n");
    Skybox::teardown_rayleigh_scattering();

    if (TEARDOWN_DEBUG) printf("texture sheet loader teardown\n");
    TextureSheetLoader::teardown();

    if (TEARDOWN_DEBUG) printf("camera teardown\n");
    teardown_cameras();
    if (TEARDOWN_DEBUG) printf("hudfont teardown\n");
    HudFont::teardown();
    if (TEARDOWN_DEBUG) printf("chat client teardown\n");
    Chat::teardown_chat_client();

    if (TEARDOWN_DEBUG) printf("objects teardown\n");
    Entities::teardown();    // Entity system
    if (TEARDOWN_DEBUG) printf("components teardown\n");
    Components::teardown();
    if (TEARDOWN_DEBUG) printf("entity net interfaces teardown\n");
    Entities::teardown_net_interfaces();

    if (TEARDOWN_DEBUG) printf("voxel volume teardown\n");
    Voxels::teardown_voxel_volume();
    if (TEARDOWN_DEBUG) printf("hud teardown\n");
    Hud::teardown();

    // free surfaces
    if (TEARDOWN_DEBUG) printf("t_map teardown\n");
    t_map::teardown_shader();

    if (TEARDOWN_DEBUG) printf("particle teardown particles\n");
    Particle::teardown_particles(); // teardown after Agents::agent_list, because of billboard particle

    if (TEARDOWN_DEBUG) printf("t_mob teardown\n");
    t_mob::teardown();

    if (TEARDOWN_DEBUG) printf("Agents::Agent list teardown\n");
    Agents::teardown();

    if (TEARDOWN_DEBUG) printf("Voxels list teardown\n");
    ClientState::teardown_voxel_lists();

    if (TEARDOWN_DEBUG) printf("item teardown\n");
    Item::teardown();
    Item::teardown_properties();
    if (TEARDOWN_DEBUG) printf("toolbelt teardown\n");
    Toolbelt::teardown();
    if (TEARDOWN_DEBUG) printf("container teardown\n");
    ItemContainer::teardown();
    ItemContainer::teardown_config();

    if (TEARDOWN_DEBUG) printf("Input teardown\n");
    teardown_input();

    if (TEARDOWN_DEBUG) printf("Animations teardown\n");
    Animations::teardown();
    Animations::teardown_config();

    if (TEARDOWN_DEBUG) printf("Badges teardown\n");
    Badges::teardown();

    if (TEARDOWN_DEBUG) printf("sound close\n");
    Sound::close();
    if (TEARDOWN_DEBUG) printf("SDL close\n");
    close_SDL();

    if (TEARDOWN_DEBUG) printf("enet teardown\n");
    teardown_network();

    if (TEARDOWN_DEBUG) printf("auth teardown\n");
    Auth::teardown();

    if (TEARDOWN_DEBUG) printf("awesomium teardown\n");
    Awesomium::teardown();

    if (TEARDOWN_DEBUG) printf("attributes teardown\n");
    Agents::teardown_attributes();
    World::teardown_attributes();
    Attributes::teardown();

    if (TEARDOWN_DEBUG) printf("Pathfinding teardown\n");
    Path::teardown();

    if (TEARDOWN_DEBUG) printf("client state teardown\n");
    ClientState::teardown();

    if (TEARDOWN_DEBUG) printf("LUA teardown\n");
    LUA::teardown();

    if (TEARDOWN_DEBUG) printf("logger teardown\n");
    Log::teardown();

    if (TEARDOWN_DEBUG) printf("Options teardown\n");
    Options::teardown();

    if (TEARDOWN_DEBUG) printf("Assertion teardown\n");
    _GS_ASSERT_TEARDOWN();

    printf("Game closed\n");

    #undef TEARDOWN_DEBUG
}
