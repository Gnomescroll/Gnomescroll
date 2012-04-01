#include "map.hpp"

#include <c_lib/common/common.hpp>
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>
#include <c_lib/game/teams.hpp>
#include <c_lib/items/spawner.hpp>

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

/* Map icons */
//static GLuint spawner_icons[N_TEAMS] = {0};
//static GLuint base_icons[N_TEAMS] = {0};
//static GLuint flag_icons[N_TEAMS] = {0};
//void update_team_icons(
    //int team,                                                     // team is optional; team=0 means generate all
    //unsigned char br, unsigned char bg, unsigned char bb,   // base color
    //unsigned char r, unsigned char g, unsigned char b       // replace with
//)
//{
    //char spawner_icon_filename[] = "spawner_map_icon2.png";
    //char base_icon_filename[] = "base_icon.png";
    //char flag_icon_filename[] = "flag_icon.png";

    //// pack filenames and icon arrays for easy looping
    //int i=0;
    //const int N_ICONS = 3;
    //char* icon_filenames[N_ICONS];
    //icon_filenames[i++] = spawner_icon_filename;
    //icon_filenames[i++] = base_icon_filename;
    //icon_filenames[i++] = flag_icon_filename;
    //i=0;
    //GLuint* icons[N_ICONS];
    //icons[i++] = spawner_icons;
    //icons[i++] = base_icons;
    //icons[i++] = flag_icons;

    //for (int j=0; j<N_ICONS; j++)
        //load_colored_icon(icon_filenames[j], &icons[j][team-1], br,bg,bb, r,g,b);
//}

/* Map letter icons */
/*
 * You: * and A on top of each other
 * Teammate: A (ally)
 * Base:    B
 * Flag:    F
 * Spawner  S#
 * Turret   T
 */

const char you_star_symbol[] = "*";
const char you_A_symbol[] = "A";
const char ally_symbol[] = "A";
const char base_symbol[] = "B";
const char flag_symbol[] = "F";
const char spawner_symbol[] = "S%d";
const char turret_symbol[] = "T";

using HudText::Text;
static Text* you_star = NULL;
static Text* you_A = NULL;
static Text* base = NULL;
static Text* flag = NULL;
static Text* enemy_flag = NULL;
static Text* ally[TEAM_MAX_PLAYERS] = {NULL};
static Text* spawner[MAX_SPAWNERS] = {NULL};
//static Text turret[MAX_TURRETS];

static bool text_icons_inited = false;

void init_text_icons()
{
    if (HudText::text_list == NULL)
    {
        printf("ERROR: HudMap::init_text_icons() -- HudText::text_list is NULL\n");
        return;
    }
    you_star = HudText::text_list->create();
    you_star->set_text((char*)you_star_symbol);
    you_A = HudText::text_list->create();
    you_A->set_text((char*)you_A_symbol);

    base = HudText::text_list->create();
    base->set_text((char*)base_symbol);

    flag = HudText::text_list->create();
    flag->set_text((char*)flag_symbol);
    
    enemy_flag = HudText::text_list->create();
    enemy_flag->set_text((char*)flag_symbol);

    for (int i=0; i<(int)TEAM_MAX_PLAYERS; i++)
    {
        ally[i] = HudText::text_list->create();
        ally[i]->set_text((char*)ally_symbol);
    }
        
    char* max_spawners_string = (char*)malloc(sizeof(char) * (10+1));
    sprintf(max_spawners_string, "%d", MAX_SPAWNERS);
    int len = strlen(max_spawners_string);
    for (int i=0; i<MAX_SPAWNERS; i++)
    {
        spawner[i] = HudText::text_list->create();
        spawner[i]->set_format((char*)spawner_symbol);
        spawner[i]->set_format_extra_length(len - 1);
    }
    free(max_spawners_string);

    text_icons_inited = true;
}

static void set_team_icons_color(
    unsigned char r, unsigned char g, unsigned char b, unsigned char a=255
)
{
    //you_star->set_color(r,g,b,a);
    //you_A->set_color(r,g,b,a);
    you_star->set_color(highlight.r, highlight.g, highlight.b, a);
    you_A->set_color(highlight.r, highlight.g, highlight.b, a);
    base->set_color(r,g,b,a);
    flag->set_color(r,g,b,a);
    for (int i=0; i<(int)TEAM_MAX_PLAYERS;
        ally[i++]->set_color(r,g,b,a));
    for (int i=0; i<MAX_SPAWNERS;
        spawner[i++]->set_color(r,g,b,a));
}

static void set_enemy_team_icons_color(
    unsigned char r, unsigned char g, unsigned char b, unsigned char a=255
)
{
    enemy_flag->set_color(r,g,b,a);
}

void update_team(int team)
{
    if (!text_icons_inited)
    {
        printf("WARNING: HudMap::update_team -- text icons are not inited\n");
        return;
    }
    if (!team) return;
    unsigned char r,g,b;
    int failure = ClientState::ctf->get_team_color(team, &r,&g,&b);
    if (failure)
        r=g=b=0;
    if (r == current_color.r && g == current_color.g && b == current_color.b)
        return;
    current_color.r = r;
    current_color.g = g;
    current_color.b = b;
    set_team_icons_color(r,g,b);

    failure = ClientState::ctf->get_enemy_team_color(team, &r,&g,&b);
    if (failure)
        r=g=b=0;
    if (r == current_enemy_color.r && g == current_enemy_color.g && b == current_enemy_color.b)
        return;
    current_enemy_color.r = r;
    current_enemy_color.g = g;
    current_enemy_color.b = b;
    set_enemy_team_icons_color(r,g,b);
}

// create blank surface
void init_surface()
{
    printf("init: hud_map \n");

    // load gradient surface
    const int grad_num = 11;
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    char grad_str[strlen(grad_fmt) -2 +1];
    sprintf(grad_str, grad_fmt, grad_num);
    //printf("Loading gradient %s\n", grad_str);
    
    gradient_surface = create_surface_from_file(grad_str);
    if (gradient_surface==NULL)
    {
        printf("HudMap gradient surface is NULL\n");
        return;
    }

    /* Init blank map surface */
    Uint32 tex_format;
    map_surface = create_surface_from_nothing(width, height);
    if (map_surface==NULL)
    {
        printf("HudMap blank surface is NULL\n");
        return;
    }

    tex_format = GL_BGRA;
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
        glTexImage2D( GL_TEXTURE_2D, 0, 4, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels);
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
        //glTexImage2D( GL_TEXTURE_2D, 0, 4, overlay_surface->w, overlay_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, overlay_surface->pixels );
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
}


static int strip = 0;
//const int strips = 16*16*2;
const int strips = 16*8;

void update_heightmap()
{
    //if (cells == NULL) return;
    //int strip_width = map_dim.x / strips;
    //unsigned char h;
    //for (int i=strip_width*strip; i < strip_width*(strip+1); i++)
        //for (int j=0; j < map_dim.y; j++)
        //{
            //h = get_cached_height(i,j);
            //cells[i + map_dim.x*j] = h;
        //}
}

void update_map_surface()
{
    if (map_surface == NULL) return;
    //if (cells == NULL) return;
    if (t_map::main_map == NULL) return;
    if (!t_map::main_map->height_changed) return;
    //int strip_width = map_dim.x / strips;
    SDL_LockSurface(map_surface);

    Uint32 pix;
    Uint8 r,g,b,a;

    int cx,cy;
    for (int i=0; i<t_map::MAP_CHUNK_WIDTH; i++)
        for (int j=0; j<t_map::MAP_CHUNK_HEIGHT; j++)
        {
            if (!t_map::main_map->chunk_heights_changed[i + j*t_map::MAP_CHUNK_WIDTH]) continue;
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

    //for (int i=strip_width*strip; i<strip_width*(strip+1); i++)
        //for (int j=0; j < map_dim.y; j++)
        //{
            //pix = ((Uint32*)gradient_surface->pixels)[t_map::main_map->column_heights[i + map_dim.x*j]];
            //SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
            //((Uint32*)map_surface->pixels)[i + map_dim.x*j] = SDL_MapRGBA(map_surface->format, b,g,r,a);
        //}

    SDL_UnlockSurface(map_surface);
}

void update_texture(GLuint texture, SDL_Surface* surface)
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_dim.x, map_dim.y, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);
}

//void update_overlay_surface()
//{
    //// blank the surface
    //for (int i=0; i<num_cells; i++)
        //((Uint32*)overlay_surface->pixels)[i] = SDL_MapRGBA(overlay_surface->format, 0,0,0,0);
//}

void draw_2x2_pixel(SDL_Surface* surface, Uint32 pix, int x, int y)
{
    int j=0,k=0;
    for (int i=0; i<4; i++)
    {
        if (i==2)
            j = 1;
        if (i==3)
            k = 1;
        
        if (x+j >= 0 && x+j < width && y+k >= 0 && y+k < height)    // only draw in surface bounds (or could segfault)
        {
            ((Uint32*)surface->pixels)[x+j + map_dim.x*(y+k)] = pix;
        }
    }
}

//void update_agents()
//{
    //if (ClientState::playerAgent_state.you == NULL) return;
    //int team = ClientState::playerAgent_state.you->status.team;
    //if (team == 0) return;

    //int x,y;

    //x = (int)ClientState::playerAgent_state.camera_state.x;
    //y = (int)ClientState::playerAgent_state.camera_state.y;
    
    //// set agent pixel
    //Uint8 r = 210,
          //g = 150,
          //b = 10,
          //a = 255;
    //Uint32 pix = SDL_MapRGBA(overlay_surface->format, b,g,r,a); // bgra, red
    //draw_2x2_pixel(overlay_surface, pix,x,y);

    //// draw teammates
    //ClientState::ctf->get_team_color(team, &r, &g, &b);
    //for (int i=0; i<ClientState::agent_list->n_max; i++)
    //{
        //Agent_state* agent = ClientState::agent_list->a[i];
        //if (agent == NULL) continue;
        //if (agent->status.team != team) continue;
        //if (agent->id == ClientState::playerAgent_state.agent_id) continue;

        //x = (int)agent->s.x;
        //y = (int)agent->s.y;

        //pix = SDL_MapRGBA(overlay_surface->format, b, g, r, a);
        //draw_2x2_pixel(overlay_surface, pix,x,y);
    //}
//}

//void update_indicators(int tex_id)
//{

    //if (overlay_surface == NULL) return;
    //SDL_LockSurface(overlay_surface);

    //update_overlay_surface();
    //update_agents();

    //SDL_UnlockSurface(overlay_surface);
    
    //update_texture(overlay_textures[tex_id], overlay_surface);
//}

void update_terrain_map(int tex_id)
{
    //init_cells();   // updates cells array if map size changed
    //if (cells == NULL) return;
    update_heightmap();
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

//void draw_spawners(float z)
//{
    //// read all spawners
    //// translate coordinates to map screen coordinate
    //float sx,sy;
    //const int w = 8;
    //const int h = 8;
    //for (int team=1; team<(int)N_TEAMS+1; team++)
    //{
        //if (ClientState::playerAgent_state.you != NULL
            //&& ClientState::playerAgent_state.you->status.team != 0
            //&& ClientState::playerAgent_state.you->status.team != team)
            //continue;
        //glBindTexture(GL_TEXTURE_2D, spawner_icons[team-1]);
        //for (int i=0; i<ClientState::spawner_list->n_max; i++)
        //{
            //Spawner* s = ClientState::spawner_list->a[i];
            //if (s == NULL) continue;
            //if (s->get_team() != team) continue;
            //world_to_map_screen_coordinates(s->x, s->y, &sx, &sy);
            //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        //}
    //}
//}

//void draw_bases(float z)
//{
    //float sx,sy;
    //const int w = 8;
    //const int h = 8;

    //int team = 0;
    //if (ClientState::playerAgent_state.you != NULL
        //&& ClientState::playerAgent_state.you != NULL)
        //team = ClientState::playerAgent_state.you->status.team;

    //Base* b = NULL;
    //if (team == 0)
    //{   // draw both
        //glBindTexture(GL_TEXTURE_2D, base_icons[0]);
        //b = ClientState::ctf->one.base;
        //world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
        //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);

        //glBindTexture(GL_TEXTURE_2D, base_icons[1]);
        //b = ClientState::ctf->two.base;
        //world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
        //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
    //}
    //else
    //{
        //glBindTexture(GL_TEXTURE_2D, base_icons[team-1]);
        //if (team == 1)
            //b = ClientState::ctf->one.base;
        //else if (team == 2)
            //b = ClientState::ctf->two.base;

        //if (b != NULL)
        //{
            //world_to_map_screen_coordinates(b->x, b->y, &sx, &sy);
            //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        //}
    //}
//}

//void draw_flags(float z)
//{
    //float sx,sy;
    //const int w = 8;
    //const int h = 8;

    //int team = 0;
    //if (ClientState::playerAgent_state.you != NULL
        //&& ClientState::playerAgent_state.you != NULL)
        //team = ClientState::playerAgent_state.you->status.team;

    //Flag* f = NULL;
    //if (team == 0)
    //{   // draw both
        //glBindTexture(GL_TEXTURE_2D, flag_icons[0]);
        //f = ClientState::ctf->one.flag;
        //world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
        //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);

        //glBindTexture(GL_TEXTURE_2D, flag_icons[1]);
        //f = ClientState::ctf->two.flag;
        //world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
        //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
    //}
    //else
    //{
        //glBindTexture(GL_TEXTURE_2D, flag_icons[team-1]);
        //if (team == 1)
            //f = ClientState::ctf->one.flag;
        //else if (team == 2)
            //f = ClientState::ctf->two.flag;

        //if (f != NULL)
        //{
            //world_to_map_screen_coordinates(f->x, f->y, &sx, &sy);
            //draw_bound_texture(sx - w/2, sy - h/2, w, h, z);
        //}
    //}
//}

void draw_team_text_icons(float z)
{
    if (!text_icons_inited) return;
    using ClientState::playerAgent_state;
    using ClientState::agent_list;
    using ClientState::object_list;
    using ClientState::ctf;
    
    if (playerAgent_state.you == NULL || playerAgent_state.you->status.team == 0)
        return;
        
    float x,y;
    int j=0;
    for (int i=0; i<agent_list->n_max; i++)
    {
        Agent_state* a = agent_list->a[i];
        if (a==NULL) continue;
        if (a->status.team != playerAgent_state.you->status.team) continue;
        if (a == playerAgent_state.you) continue;
        world_to_map_screen_coordinates(a->s.x, a->s.y, &x, &y);
        ally[j]->set_position(x,y);
        ally[j]->set_depth(z);
        ally[j]->draw_centered();
        j++;
    }

    for (int i=0; i<object_list->n_max; i++)
    {
        ObjectPolicyInterface* s = object_list->a[i];
        if (s==NULL) continue;
        if (s->state()->type != OBJ_TYPE_SPAWNER) continue; // todo: object_list should be able to iterate over one type only
        if (s->state()->get_team() != playerAgent_state.you->status.team) continue;
        if (s->state()->team_index < 0)
        {
            printf("WARNING: draw_team_text_icons() -- spawner->team_index %d invalid\n", s->state()->team_index);
            continue;
        }
        if ((int)s->state()->team_index == playerAgent_state.you->status.spawner)
            spawner[j]->set_color(highlight.r, highlight.g, highlight.b);
        else
            spawner[j]->set_color(current_color.r, current_color.g, current_color.b);
        spawner[j]->update_formatted_string(1, s->state()->team_index);
        Vec3 p = s->state()->get_position();
        world_to_map_screen_coordinates(p.x, p.y, &x, &y);
        spawner[j]->set_position(x,y);
        spawner[j]->set_depth(z);
        spawner[j]->draw_centered();
        j++;
    }

    Base* b = ctf->get_base(playerAgent_state.you->status.team);
    world_to_map_screen_coordinates(b->x, b->y, &x, &y);
    base->set_position(x,y);
    base->set_depth(z);
    if (playerAgent_state.you->status.spawner == BASE_SPAWN_ID)
        base->set_color(highlight.r, highlight.g, highlight.b);
    else
        base->set_color(current_color.r, current_color.g, current_color.b);
    base->draw_centered();

    int team = playerAgent_state.you->status.team;
    Flag* f = ctf->get_flag(team);
    if (!f->held)
    {
        world_to_map_screen_coordinates(f->x, f->y, &x, &y);
        flag->set_position(x,y);
        flag->set_depth(z);
        flag->draw_centered();
    }

    team = (team == 1) ? 2 : 1; // TODO, retrieve enemy flag from ctf
    f = ctf->get_flag(team);
    if (!f->held)
    {
        world_to_map_screen_coordinates(f->x, f->y, &x, &y);
        enemy_flag->set_position(x,y);
        enemy_flag->set_depth(z);
        enemy_flag->draw_centered();
    }
}

void draw_text_icons(float z)
{
    if (!text_icons_inited) return;
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;
    float x,y;
    world_to_map_screen_coordinates(
        playerAgent_state.camera_state.x, playerAgent_state.camera_state.y,
        &x, &y
    );
    you_star->set_position(x,y);
    you_star->set_depth(z);
    you_star->draw_character_rotated(playerAgent_state.camera_state.theta - 0.5);

    you_A->set_position(x,y);
    you_A->set_depth(z);
    you_A->draw_character_rotated(playerAgent_state.camera_state.theta - 0.5);
    // -0.5 offset to orient texture properly
}

//void draw_items(float z)
//{
    //draw_bases(z);
    //draw_flags(z);
    //draw_spawners(z);
//}

void draw_text()
{
    const float z = -0.1;
    draw_team_text_icons(z);
    draw_text_icons(z);
}

void draw()
{   //  double buffered texture swap indices
    static int draw_map_texture_index = 0;
    static int update_map_texture_index = 1;
    //static int draw_overlay_texture_index = 0;
    //static int update_overlay_texture_index = 1;
    
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

    //if (update_counter % 2 == 0)
    //{
        //update_indicators(update_overlay_texture_index);
        //draw_overlay_texture_index++;
        //draw_overlay_texture_index%=2;
        //update_overlay_texture_index++;
        //update_overlay_texture_index%=2;
    //}

    update_counter++;

    static const float z = -0.03f;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, map_textures[draw_map_texture_index]);
    draw_bound_texture(screen_x_offset, screen_y_offset, width, height, z);

    //draw_items(z+0.01);

    //glBindTexture(GL_TEXTURE_2D, overlay_textures[draw_overlay_texture_index]);
    //draw_bound_texture(screen_x_offset, screen_y_offset, width, height, z+0.02);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void teardown()
{
    //if (overlay_surface != NULL)
        //SDL_FreeSurface(overlay_surface);

    if (map_surface != NULL)
        SDL_FreeSurface(map_surface);

    if (gradient_surface != NULL)
        SDL_FreeSurface(gradient_surface);

    //if (cells != NULL)
        //free(cells);
}

}
