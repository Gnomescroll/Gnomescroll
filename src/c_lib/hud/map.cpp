#include "map.hpp"

#include <common/common.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/t_map.hpp>
#include <state/client_state.hpp>
#include <SDL/texture_loader.hpp>
#include <SDL/draw_functions.hpp>

namespace HudMap
{

static Color current_color;
static Color current_enemy_color;
static Color highlight;

// for texture init
static const int width = 512;
static const int height = 512;
static const int screen_x_offset = 0;  // from left
//static const int screen_y_offset = 0;   // from top

static SDL_Surface* map_surface = NULL;
static GLuint map_textures[2] = {0};

static SDL_Surface* gradient_surface = NULL;

void load_colored_icon(
    char* fn, GLuint* texture,
    unsigned char br, unsigned char bg, unsigned char bb,   // base color
    unsigned char r, unsigned char g, unsigned char b       // replace with
)
{
    // format path
    const char icon_path_fmt[] = "./media/sprites/icons/%s";
    char* path = (char*)calloc(strlen(icon_path_fmt) + strlen(fn) - 2 + 1, sizeof(char));
    sprintf(path, icon_path_fmt, fn);
    load_colored_texture(path, texture, br,bg,bb, r,g,b);
    free(path);
}

/* Map letter icons */
/*
 * You: * and A on top of each other
 * Agent: A
 * Base:    B
 */

const char you_star_symbol[] = "*";
const char you_A_symbol[] = "A";
const char ally_symbol[] = "A";
const char base_symbol[] = "B";
const char camera_symbol[] = "C";

using HudText::Text;
static Text* you_star = NULL;
static Text* you_A = NULL;
static Text* base = NULL;
static Text* ally[MAX_AGENTS] = {NULL};
static Text* camera = NULL;

static bool text_icons_inited = false;

void init_text_icons()
{
    GS_ASSERT(HudText::text_list != NULL);
    if (HudText::text_list == NULL) return;

    you_star = HudText::text_list->create();
    GS_ASSERT(you_star != NULL);
    if (you_star == NULL) return;
    you_star->set_text(you_star_symbol);

    you_A = HudText::text_list->create();
    GS_ASSERT(you_A != NULL);
    if (you_A == NULL) return;
    you_A->set_text(you_A_symbol);

    base = HudText::text_list->create();
    GS_ASSERT(base != NULL);
    if (base == NULL) return;
    base->set_text(base_symbol);

    for (int i=0; i<(int)MAX_AGENTS; i++)
    {
        ally[i] = HudText::text_list->create();
        GS_ASSERT(ally[i] != NULL);
        if (ally[i] == NULL) return;
        ally[i]->set_text(ally_symbol);
    }

    camera = HudText::text_list->create();
    GS_ASSERT(camera != NULL);
    if (camera == NULL) return;
    camera->set_text(camera_symbol);

    text_icons_inited = true;
}

void set_icon_colors()
{
    const unsigned char a = 255;
    if (you_star == NULL) return;
    if (you_A == NULL) return;
    if (base == NULL) return;
    if (camera == NULL) return;
    
    you_star->set_color(highlight.r, highlight.g, highlight.b, a);
    you_A->set_color(highlight.r, highlight.g, highlight.b, a);
    base->set_color(highlight.r, highlight.g, highlight.b,a);
    for (int i=0; i<(int)MAX_AGENTS; i++)
        if (ally[i] != NULL) ally[i]->set_color(highlight.r, highlight.g, highlight.b,a);
    camera->set_color(highlight.r, highlight.g, highlight.b, a);
}

// create blank surface
void init_surface()
{
    // load gradient surface
    const int grad_num = 11;
    const char grad_fmt[] = "media/sprites/gradient/heightmap_gradient_%02d.png";
    MALLOX(char, grad_str, strlen(grad_fmt) -2 +1);

    sprintf(grad_str, grad_fmt, grad_num);
    
    gradient_surface = create_surface_from_file(grad_str);
    GS_ASSERT(gradient_surface != NULL);
    if (gradient_surface == NULL) return;

    /* Init blank map surface */
    map_surface = create_surface_from_nothing(width, height);
    GS_ASSERT(map_surface != NULL);
    if (map_surface == NULL) return;

    GLenum tex_format = GL_BGRA;
    if (map_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // set surface pixels to 0,0,0,255;
    for (int i=0; i<map_surface->w; i++)
        for (int j=0; j<map_surface->h; j++)
            ((Uint32*)map_surface->pixels)[i + map_surface->w*j] = SDL_MapRGBA(map_surface->format, 0,0,0,255);
    
    // texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, map_textures);
    for (int i=0; i<2; i++)
    {
        GS_ASSERT(map_textures[i] != 0);
        if (map_textures[i] == 0) continue;
        glBindTexture(GL_TEXTURE_2D, map_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels);
    }
    glDisable(GL_TEXTURE_2D);
    CHECK_GL_ERROR();
}

void init()
{
    init_surface();
    init_text_icons();
    current_color.r = 255;
    current_color.g = 255;
    current_color.b = 255;
    
    current_enemy_color.r = 255;
    current_enemy_color.g = 255;
    current_enemy_color.b = 255;

    highlight.r = 247;  // yellow/gold
    highlight.g = 247;
    highlight.b = 10;

    set_icon_colors();
}


static int strip = 0;
const int strips = 16*8;

void update_map_surface()
{
    if (map_surface == NULL) return;
    if (t_map::main_map == NULL) return;
    if (!t_map::main_map->height_changed) return;
    SDL_LockSurface(map_surface);

    Uint32 pix;
    Uint8 r,g,b,a;

    int cx,cy;
    for (int i=0; i<MAP_CHUNK_XDIM; i++)
    for (int j=0; j<MAP_CHUNK_YDIM; j++)
    {
        if (t_map::main_map->chunk_heights_status[i + j*MAP_CHUNK_XDIM] != CHUNK_HEIGHT_CHANGED) continue;
        for (int m=0; m<TERRAIN_CHUNK_WIDTH; m++)
        {
            cx = i*TERRAIN_CHUNK_WIDTH + m;
            for (int n=0; n<TERRAIN_CHUNK_WIDTH; n++)
            {
                cy = j*TERRAIN_CHUNK_WIDTH + n;
                pix = ((Uint32*)gradient_surface->pixels)[t_map::main_map->column_heights[cx + MAP_WIDTH*cy]];
                SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
                ((Uint32*)map_surface->pixels)[cx + MAP_WIDTH*cy] = SDL_MapRGBA(map_surface->format, b,g,r,a);
            }
        }
    }

    t_map::main_map->reset_heights_read();

    SDL_UnlockSurface(map_surface);
}

void update_texture(GLuint texture, SDL_Surface* surface)
{
    GS_ASSERT(texture != 0);
    if (texture == 0) return;
    GS_ASSERT(surface != NULL);
    if (surface == NULL) return;

    GS_ASSERT(map_dim.x != 0 && map_dim.y != 0);
    if (map_dim.x == 0 || map_dim.y == 0) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_dim.x, map_dim.y, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    glDisable(GL_TEXTURE_2D);

    CHECK_GL_ERROR();
}

void draw_2x2_pixel(SDL_Surface* surface, Uint32 pix, int x, int y)
{
    int j=0,k=0;
    for (int i=0; i<4; i++)
    {
        if (i==2) j = 1;
        if (i==3) k = 1;
        if (x+j >= 0 && x+j < width && y+k >= 0 && y+k < height)    // only draw in surface bounds (or could segfault)
            ((Uint32*)surface->pixels)[x+j + map_dim.x*(y+k)] = pix;
    }
}

void update_terrain_map(int tex_id)
{
    update_map_surface();
    update_texture(map_textures[tex_id], map_surface);
    strip++;
    strip%=strips;
}

void world_to_map_screen_coordinates(float x, float y, float *sx, float *sy)
{
    float x_scale = ((float)width)/((float)map_dim.x);
    float y_scale = ((float)height)/((float)map_dim.y);
    *sx = x * x_scale;
    *sy = y * y_scale;
    *sx += _xresf-screen_x_offset-width;
    *sy += _yresf-height;
}

void draw_text_icons(float z)
{
    if (!text_icons_inited) return;
    using ClientState::playerAgent_state;
    using Agents::agent_list;
    if (playerAgent_state.you == NULL) return;
            
    float x,y;
    for (unsigned int i=0; i<agent_list->max; i++)
    {
        Agent* a = &agent_list->objects[i];
        if (a->id == agent_list->null_id) continue;
        if (a == playerAgent_state.you) continue;
        Vec3 p = a->get_position();
        world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        ally[i]->set_position(x,y);
        ally[i]->set_depth(z);
        ally[i]->draw_centered();
    }

    using ClientState::playerAgent_state;

    Objects::Object* b = NULL;
    if (playerAgent_state.you != NULL)
    {
        if (playerAgent_state.you->status.spawner == BASE_SPAWN_ID)
            b = Objects::get(OBJECT_BASE, 0);   // TODO -- remove Base
        else
            b = Objects::get(OBJECT_AGENT_SPAWNER, playerAgent_state.you->status.spawner);
    }

    if (b != NULL)
    {
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)b->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics != NULL)
        {
            Vec3 p = physics->get_position();
            world_to_map_screen_coordinates(p.x, p.y, &x, &y);
            base->set_position(x,y);
            base->set_depth(z);
        }
        base->set_color(highlight.r, highlight.g, highlight.b);
        base->draw_centered();
    }

    if (you_star == NULL) return;
    if (you_A == NULL) return;
    if (camera == NULL) return;
    
    if (!text_icons_inited) return;
    if (playerAgent_state.you == NULL) return;
    world_to_map_screen_coordinates(
        playerAgent_state.camera_state.x, playerAgent_state.camera_state.y,
        &x, &y
    );
    you_star->set_position(x,y);
    you_star->set_depth(z);
    you_star->draw_character_rotated_centered(playerAgent_state.camera_state.theta - 0.5f);

    you_A->set_position(x,y);
    you_A->set_depth(z);
    you_A->draw_character_rotated_centered(playerAgent_state.camera_state.theta - 0.5f);

    if (free_camera != NULL)
    {
        Vec3 p = free_camera->get_position();
        world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        camera->set_position(x,y);
        camera->set_depth(z);
        camera->draw_character_rotated_centered(free_camera->theta - 0.5f);
    }
    
    // -0.5 offset to orient texture properly
}

void draw_text()
{
    const float z = -0.1f;
    draw_text_icons(z);
    CHECK_GL_ERROR();
}

void draw()
{   //  double buffered texture swap indices

    static int draw_map_texture_index = 0;
    static int update_map_texture_index = 1;
    
    static unsigned int update_counter = 0;

    const int tick_update_rate = 3;
    if(update_counter % tick_update_rate == 0)
    {
        update_terrain_map(update_map_texture_index);
        draw_map_texture_index++;
        draw_map_texture_index%=2;
        update_map_texture_index++;
        update_map_texture_index%=2;
    }

    update_counter++;

    GS_ASSERT(map_textures[draw_map_texture_index] != 0);
    if (map_textures[draw_map_texture_index] == 0) return;

    static const float z = -0.03f;

    glColor4ub(255,255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, map_textures[draw_map_texture_index]);

    draw_bound_texture(_xresf-screen_x_offset-width, _yresf-height, width, height, z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    CHECK_GL_ERROR();
}

void teardown()
{
    if (map_surface != NULL)
        SDL_FreeSurface(map_surface);

    if (gradient_surface != NULL)
        SDL_FreeSurface(gradient_surface);
}

}   // HudMap
