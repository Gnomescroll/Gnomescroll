#include "map.hpp"

#include <common/common.hpp>
#include <t_map/t_map.hpp>
#include <state/client_state.hpp>
#include <SDL/texture_loader.hpp>
#include <SDL/draw_functions.hpp>

namespace HudMap
{

static Color current_color;
static Color current_enemy_color;
static Color highlight;

//static int num_cells = 0;
//static unsigned char* cells = NULL;

// for texture init
static const int width = 512;
static const int height = 512;
static const int screen_x_offset = 50;  // from left
static const int screen_y_offset = 50;   // from bottom;

static SDL_Surface* map_surface = NULL;
static GLuint map_textures[2];

//static SDL_Surface* overlay_surface = NULL;
//static GLuint overlay_textures[2];

static SDL_Surface* gradient_surface = NULL;

void load_colored_icon(
    char* fn, GLuint* texture,
    unsigned char br, unsigned char bg, unsigned char bb,   // base color
    unsigned char r, unsigned char g, unsigned char b       // replace with
)
{
    // format path
    const char icon_path_fmt[] = "./media/texture/icons/%s";
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
 * Spawner  S#
 * Turret   T
 */

const char you_star_symbol[] = "*";
const char you_A_symbol[] = "A";
const char ally_symbol[] = "A";
const char base_symbol[] = "B";
const char spawner_symbol[] = "S%d";
const char turret_symbol[] = "T";
const char camera_symbol[] = "C";

using HudText::Text;
static Text* you_star = NULL;
static Text* you_A = NULL;
static Text* base = NULL;
static Text* ally[AGENT_MAX] = {NULL};
static Text* spawner[Components::MAX_AGENT_SPAWNER_COMPONENTS] = {NULL};
//static Text turret[MAX_TURRETS];
static Text* camera = NULL;

static bool text_icons_inited = false;

void init_text_icons()
{
    if (HudText::text_list == NULL)
    {
        printf("ERROR: HudMap::init_text_icons() -- HudText::text_list is NULL\n");
        return;
    }
    you_star = HudText::text_list->create();
    GS_ASSERT(you_star != NULL);
    if (you_star == NULL) return;
    you_star->set_text((char*)you_star_symbol);

    you_A = HudText::text_list->create();
    GS_ASSERT(you_A != NULL);
    if (you_A == NULL) return;
    you_A->set_text((char*)you_A_symbol);

    base = HudText::text_list->create();
    GS_ASSERT(base != NULL);
    if (base == NULL) return;
    base->set_text((char*)base_symbol);

    for (int i=0; i<(int)AGENT_MAX; i++)
    {
        ally[i] = HudText::text_list->create();
        GS_ASSERT(ally[i] != NULL);
        if (ally[i] == NULL) return;
        ally[i]->set_text((char*)ally_symbol);
    }
        
    char* spawner_max_string = (char*)malloc(sizeof(char) * (10+1));
    sprintf(spawner_max_string, "%d", Components::MAX_AGENT_SPAWNER_COMPONENTS);
    int len = (int)strlen(spawner_max_string);
    for (int i=0; i<Components::MAX_AGENT_SPAWNER_COMPONENTS; i++)
    {
        spawner[i] = HudText::text_list->create();
        GS_ASSERT(spawner[i] != NULL);
        if (spawner[i] == NULL) break;
        spawner[i]->set_format((char*)spawner_symbol);
        spawner[i]->set_format_extra_length(len - 1);
    }
    free(spawner_max_string);

    camera = HudText::text_list->create();
    GS_ASSERT(camera != NULL);
    if (camera == NULL) return;
    camera->set_text((char*)camera_symbol);

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
    for (int i=0; i<(int)AGENT_MAX; i++)
        if (ally[i] != NULL) ally[i]->set_color(highlight.r, highlight.g, highlight.b,a);
    for (int i=0; i<Components::MAX_AGENT_SPAWNER_COMPONENTS; i++)
        if (spawner[i] != NULL) spawner[i]->set_color(highlight.r, highlight.g, highlight.b,a);
    camera->set_color(highlight.r, highlight.g, highlight.b, a);
}

// create blank surface
void init_surface()
{
    printf("init: hud_map \n");

    // load gradient surface
    const int grad_num = 11;
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    //char grad_str[strlen(grad_fmt) -2 +1];
    MALLOX(char, grad_str, strlen(grad_fmt) -2 +1);

    sprintf(grad_str, grad_fmt, grad_num);
    //printf("Loading gradient %s\n", grad_str);
    
    gradient_surface = create_surface_from_file(grad_str);
    if (gradient_surface==NULL)
    {
        printf("HudMap gradient surface is NULL\n");
        return;
    }

    /* Init blank map surface */
    map_surface = create_surface_from_nothing(width, height);
    if (map_surface==NULL)
    {
        printf("HudMap blank surface is NULL\n");
        return;
    }

    GLenum tex_format = GL_BGRA;
    if (map_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // set surface pixels to 0,0,0,255;
    for (int i=0; i<map_surface->w; i++)
        for (int j=0; j<map_surface->h; j++)
            ((Uint32*)map_surface->pixels)[i + map_surface->w*j] = SDL_MapRGBA(map_surface->format, 0,0,0,255);
    
    // texture
    glEnable(GL_TEXTURE_2D);
    for (int i=0; i<2; i++)
    {
        glGenTextures(1, &map_textures[i]);
        glBindTexture(GL_TEXTURE_2D, map_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels);
    }
    glDisable(GL_TEXTURE_2D);

    ///* Init blank indicator overlay surface */
    //overlay_surface = create_surface_from_nothing(width, height);
    //if (overlay_surface==NULL)
    //{
        //printf("Hud indicator overlay blank surface is NULL\n");
        //return;
    //}

    //tex_format = GL_BGRA;
    //if (overlay_surface->format->Rmask == 0x000000ff)
        //tex_format = GL_RGBA;

    //// texture
    //glEnable(GL_TEXTURE_2D);
    //for (int i=0; i<2; i++)
    //{
        //glGenTextures(1, &overlay_textures[i]);
        //glBindTexture(GL_TEXTURE_2D, overlay_textures[i]);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        ////GL_BGRA
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, overlay_surface->w, overlay_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, overlay_surface->pixels );
    //}
    //glDisable(GL_TEXTURE_2D);
}

//void init_cells()
//{
    //int n_cells = map_dim.x * map_dim.y;
    //if (n_cells == num_cells) return;   // size same, dont change

    //num_cells = n_cells;

    //if (!n_cells)
    //{ // no cells, free/null cells
        //free(cells);
        //cells = NULL;
        //return;
    //}

    //if (cells != NULL)
        //cells = (unsigned char*)realloc(cells, num_cells*sizeof(unsigned char));
    //else
        //cells = (unsigned char*)calloc(num_cells, sizeof(unsigned char));
//}

void init()
{
    init_surface();
    init_text_icons();
    //init_cells();
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
//const int strips = 16*16*2;
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
    for (int i=0; i<t_map::MAP_CHUNK_XDIM; i++)
        for (int j=0; j<t_map::MAP_CHUNK_YDIM; j++)
        {
            if (!t_map::main_map->chunk_heights_changed[i + j*t_map::MAP_CHUNK_XDIM]) continue;
            for (int m=0; m<t_map::TERRAIN_CHUNK_WIDTH; m++)
            {
                cx = i*t_map::TERRAIN_CHUNK_WIDTH + m;
                for (int n=0; n<t_map::TERRAIN_CHUNK_WIDTH; n++)
                {
                    cy = j*t_map::TERRAIN_CHUNK_WIDTH + n;
                    pix = ((Uint32*)gradient_surface->pixels)[t_map::main_map->column_heights[cx + t_map::MAP_WIDTH*cy]];
                    SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
                    ((Uint32*)map_surface->pixels)[cx + t_map::MAP_WIDTH*cy] = SDL_MapRGBA(map_surface->format, b,g,r,a);
                }
            }
        }

    t_map::main_map->reset_heights_read();

    SDL_UnlockSurface(map_surface);
}

void update_texture(GLuint texture, SDL_Surface* surface)
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_dim.x, map_dim.y, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);
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
    *sx += screen_x_offset;
    *sy += screen_y_offset;
}

void draw_text_icons(float z)
{
    if (!text_icons_inited) return;
    using ClientState::playerAgent_state;
    using ClientState::agent_list;
    if (playerAgent_state.you == NULL) return;
            
    float x,y;
    for (int i=0; i<agent_list->n_max; i++)
    {
        Agent_state* a = agent_list->a[i];
        if (a == NULL) continue;
        if (a == playerAgent_state.you) continue;
        Vec3 p = a->get_position();
        world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        ally[i]->set_position(x,y);
        ally[i]->set_depth(z);
        ally[i]->draw_centered();
    }

    //SpawnerProperties* s;
    //for (int i=0; i<spawner_list->max; i++)
    //{
        //s = (SpawnerProperties*)spawner_list->objects[i];
        //if (s==NULL) continue;
        //unsigned int team_index = s->obj->get_team_index();
        //if (s->obj->get_team() != playerAgent_state.you->status.team) continue;
        //if (team_index <= 0 || team_index == TEAM_INDEX_NONE || team_index == NULL_TEAM_INDEX)
            //continue;
        //if ((int)team_index == playerAgent_state.you->status.spawner)
            //spawner[j]->set_color(highlight.r, highlight.g, highlight.b);
        //else
            //spawner[j]->set_color(current_color.r, current_color.g, current_color.b);
        //spawner[j]->update_formatted_string(1, team_index);
        //Vec3 p = s->obj->get_position();
        //world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        //spawner[j]->set_position(x,y);
        //spawner[j]->set_depth(z);
        //spawner[j]->draw_centered();
        //j++;
    //}
    
    using Components::BASE_SPAWN_ID;
    Objects::Object* b = Objects::get(OBJECT_BASE, 0);
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
        if (playerAgent_state.you->status.spawner == BASE_SPAWN_ID)
            base->set_color(highlight.r, highlight.g, highlight.b);
        else
            base->set_color(current_color.r, current_color.g, current_color.b);
        base->draw_centered();
    }

    if (you_star == NULL) return;
    if (you_A == NULL) return;
    if (camera == NULL) return;
    
    if (!text_icons_inited) return;
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;
    world_to_map_screen_coordinates(
        playerAgent_state.camera_state.x, playerAgent_state.camera_state.y,
        &x, &y
    );
    you_star->set_position(x,y);
    you_star->set_depth(z);
    you_star->draw_character_rotated(playerAgent_state.camera_state.theta - 0.5f);

    you_A->set_position(x,y);
    you_A->set_depth(z);
    you_A->draw_character_rotated(playerAgent_state.camera_state.theta - 0.5f);

    if (free_camera == NULL) return;
    if (free_camera->is_current())
    {
        Vec3 p = free_camera->get_position();
        world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        camera->set_position(x,y);
        camera->set_depth(z);
        camera->draw_character_rotated(free_camera->theta - 0.5f);
    }
    
    // -0.5 offset to orient texture properly
}

void draw_text()
{
    const float z = -0.1f;
    draw_text_icons(z);
    draw_text_icons(z);
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

    static const float z = -0.03f;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, map_textures[draw_map_texture_index]);
    draw_bound_texture(screen_x_offset, screen_y_offset, width, height, z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void teardown()
{
    if (map_surface != NULL)
        SDL_FreeSurface(map_surface);

    if (gradient_surface != NULL)
        SDL_FreeSurface(gradient_surface);
}

}
