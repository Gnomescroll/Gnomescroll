#include "map.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.h>

namespace HudMap {

int num_cells = 0;
unsigned char* cells = NULL;

// for texture init
const int width = 512;
const int height = 512;

static SDL_Surface* map_surface = NULL;
static GLuint map_textures[2];

static SDL_Surface* overlay_surface = NULL;
static GLuint overlay_textures[2];

static SDL_Surface* gradient_surface = NULL;

// create blank surface
void init_surface() {
    printf("init: hud_map \n");

    const int grad_num = 4;
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    char grad_str[strlen(grad_fmt) -2 +1];
    sprintf(grad_str, grad_fmt, grad_num);
    gradient_surface = create_surface_from_file(grad_str);
    if (gradient_surface==NULL) {
        printf("HudMap gradient surface is NULL\n");
        return;
    }

    Uint32 tex_format;

    /* Init blank map surface */
    map_surface = create_surface_from_nothing(width, height);
    if (map_surface==NULL) {
        printf("HudMap blank surface is NULL\n");
        return;
    }

    tex_format = GL_BGRA;
    if (map_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;
    
    // texture
    glEnable(GL_TEXTURE_2D);
    for (int i=0; i<2; i++)
    {
        glGenTextures(1, &map_textures[i]);
        glBindTexture(GL_TEXTURE_2D, map_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D( GL_TEXTURE_2D, 0, 4, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels );
    }
    glDisable(GL_TEXTURE_2D);

    /* Init blank indicator overlay surface */
    overlay_surface = create_surface_from_nothing(width, height);
    if (overlay_surface==NULL) {
        printf("Hud indicator overlay blank surface is NULL\n");
        return;
    }

    tex_format = GL_BGRA;
    if (overlay_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // texture
    glEnable(GL_TEXTURE_2D);
    for (int i=0; i<2; i++)
    {
        glGenTextures(1, &overlay_textures[i]);
        glBindTexture(GL_TEXTURE_2D, overlay_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D( GL_TEXTURE_2D, 0, 4, overlay_surface->w, overlay_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, overlay_surface->pixels );
    }
    glDisable(GL_TEXTURE_2D);
}

void init_cells()
{
    int n_cells = map_dim.x * map_dim.y;
    if (n_cells == num_cells) return;   // size same, dont change

    num_cells = n_cells;

    if (!n_cells) { // no cells, free/null cells
        free(cells);
        cells = NULL;
        return;
    }

    if (cells != NULL)
    {
        cells = (unsigned char*)realloc(cells, num_cells*sizeof(unsigned char));
    }
    else
    {
        cells = (unsigned char*)calloc(num_cells, sizeof(unsigned char));
    }
}

void init() {
    init_surface();
    init_cells();
}

void update_heightmap() {
    static int strip = 0;
    const int strips = 16;
    int strip_width = map_dim.x / strips;
    
    if (cells == NULL) return;
    int i,j;
    int h;
    for (i=strip_width*strip; i < strip_width*(strip+1); i++) {
        for (j=0; j < map_dim.y; j++) {
            h = get_height_at(i,j);
            cells[i + map_dim.x*j] = (unsigned char)2*h;
        }
    }

    strip++;
    strip%=strips;
}

void update_map_surface() {
    if (map_surface == NULL) return;
    if (cells == NULL) return;
    SDL_LockSurface(map_surface);
    
    int i;
    Uint32 pix;
    Uint8 r,g,b,a;
    for (i=0; i<num_cells; i++) {
        pix = ((Uint32*)gradient_surface->pixels)[cells[i]];
        SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
        ((Uint32*)map_surface->pixels)[i] = SDL_MapRGBA(map_surface->format, b,g,r,a);
    }

    SDL_UnlockSurface(map_surface);
}

void update_texture(GLuint texture, SDL_Surface* surface) {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map_dim.x, map_dim.y, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);
}

void update_overlay_surface() {

    // blank the surface
    int i;
    for (i=0; i<num_cells; i++) {
        ((Uint32*)overlay_surface->pixels)[i] = SDL_MapRGBA(overlay_surface->format, 0,0,0,0);
    }
}

void update_agents() {
    if (ClientState::playerAgent_state.you == NULL) return;
    int team = ClientState::playerAgent_state.you->status.team;
    if (team == 0) return;

    int x,y;

    x = (int)ClientState::playerAgent_state.camera_state.x;
    y = (int)ClientState::playerAgent_state.camera_state.y;
    
    // set agent pixel
    Uint8 r = 10,
          g = 210,
          b = 10,
          a = 255;
    Uint32 pix = SDL_MapRGBA(overlay_surface->format, b,g,r,a); // bgra, red

    if (x >= 0 && x < width && y >= 0 && y < height)    // only draw in surface bounds (or could segfault)
    {
        ((Uint32*)overlay_surface->pixels)[x + map_dim.x*y] = pix;
    }

    ClientState::ctf.get_team_color(team, &r, &g, &b);
    for (int i=0; i<ClientState::agent_list.n_max; i++)
    {
        Agent_state* agent = ClientState::agent_list.a[i];
        if (agent == NULL) continue;
        if (agent->status.team != team) continue;
        if (agent->id == ClientState::playerAgent_state.agent_id) continue;

        x = (int)agent->s.x;
        y = (int)agent->s.y;

        if (!(x >= 0 && x < width && y >= 0 && y < height))    // only draw in surface bounds (or could segfault)
         continue;

        pix = SDL_MapRGBA(overlay_surface->format, b, g, r, a);
        ((Uint32*)overlay_surface->pixels)[x + map_dim.x*y] = pix;
    }
}

void update_items(){};

void update_indicators(int tex_id) {

    if (overlay_surface == NULL) return;
    SDL_LockSurface(overlay_surface);

    update_overlay_surface();
    update_agents();
    update_items();

    SDL_UnlockSurface(overlay_surface);
    
    update_texture(overlay_textures[tex_id], overlay_surface);
}

void update_terrain_map(int tex_id) {
    init_cells();   // updates cells array if map size changed
    if (cells == NULL) return;
    update_heightmap();
    update_map_surface();
    update_texture(map_textures[tex_id], map_surface);
}

void draw() {
    //  double buffered texture swap indices
    static int draw_map_texture_index = 0;
    static int update_map_texture_index = 1;
    static int draw_overlay_texture_index = 0;
    static int update_overlay_texture_index = 1;
    
    static unsigned int update_counter = 0;

    if(update_counter % 6 == 0)
    {
        update_terrain_map(update_map_texture_index);
        draw_map_texture_index++;
        draw_map_texture_index%=2;
        update_map_texture_index++;
        update_map_texture_index%=2;
    }

    if (update_counter % 2 == 0)
    {
        update_indicators(update_overlay_texture_index);
        draw_overlay_texture_index++;
        draw_overlay_texture_index%=2;
        update_overlay_texture_index++;
        update_overlay_texture_index%=2;
    }

    update_counter++;

    static const float z = -0.5f;
    static const int x = 50;
    static const int y = 50;
    static const int w = 512;
    static const int h = 512;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, map_textures[draw_map_texture_index]);
    draw_bound_texture(x, y, w, h, z*2);

    glBindTexture(GL_TEXTURE_2D, overlay_textures[draw_overlay_texture_index]);
    draw_bound_texture(x,y,w,h,z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void teardown()
{
    if (overlay_surface != NULL)
        SDL_FreeSurface(overlay_surface);

    if (map_surface != NULL)
        SDL_FreeSurface(map_surface);

    if (gradient_surface != NULL)
        SDL_FreeSurface(gradient_surface);

    if (cells != NULL)
        free(cells);
}

// for cython
void draw_map() {draw();}

}
