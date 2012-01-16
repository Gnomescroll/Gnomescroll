#include "equipment.hpp"

#include <c_lib/SDL/texture_loader.h>
#include <c_lib/SDL/SDL_functions.h>

namespace HudEquipment {

static SDL_Surface* panel_surface;
static GLuint panel_texture;
static SDL_Surface* sprite_surface;
static GLuint sprite_texture;

static const int width = 256;
static const int height = 32;

static const int icon_width = 32;
static const int icon_height = 32;

static const float spritesheet_width = 256.0f;
static const float spritesheet_height = 256.0f;

static const int n_items = width / icon_height;
static int slot = 0;

static const int n_icons = (spritesheet_width / icon_width) * (spritesheet_height / icon_height);
static int slot_icons[n_items];

void init_surface() {

    char panel_fn[] = "media/texture/equipment_panel.png";
    char sprite_fn[] = "media/texture/equipment_icons.png";

    panel_surface = create_surface_from_file(panel_fn);
    sprite_surface = create_surface_from_file(sprite_fn);

    if (sprite_surface == NULL) {
        printf("HudEquipment sprite_surface is NULL\n");
        return;
    }
    if (panel_surface == NULL) {
        printf("HudEquipment panel_surface is NULL\n");
        return;
    }

    Uint32 tex_format = GL_BGRA;
    if (panel_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // Panel
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &panel_texture);
    glBindTexture(GL_TEXTURE_2D, panel_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 4, panel_surface->w, panel_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, panel_surface->pixels );
    glDisable(GL_TEXTURE_2D);

    tex_format = GL_BGRA;
    if (sprite_surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    // Sprite
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &sprite_texture);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 4, sprite_surface->w, sprite_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, sprite_surface->pixels );
    glDisable(GL_TEXTURE_2D);

}

void init() {
    init_surface();
}


void set_slot(int s) {
    if (s >= n_items) { // allow negative slot number for "no slot"
        printf("WARNING: set_slot, slot=%d out of bounds\n", s);
        return;
    }
    slot = s;
}

void set_slot_icon(int slot, int icon_id) {
    if (slot < 0 || slot >= n_items) {
        printf("WARNING: set_slot_icon, slot=%d out of range\n", slot);
        return;
    }
    if (icon_id < 0 || icon_id >= n_icons) {
        printf("WARNING: set_slot_icon, icon_id=%d out of range\n", icon_id);
        return;
    }
    slot_icons[slot] = icon_id;
}

int get_equipment_slot_icon(int slot) {
    return slot_icons[slot];
}

void draw_panel() {

    glBindTexture(GL_TEXTURE_2D, panel_texture);

    static const float z = -0.5f;
    static const int x = 0;
    static const int y = _yresf - height;

    draw_bound_texture(x, y, width, height, z);
}

void draw_icons() {

    glBindTexture(GL_TEXTURE_2D, sprite_texture);

    static const float z = -0.5f;

    float x;
    float y = _yresf - icon_height;
    int i;
    float sx,sy;
    int icon;
    for (i=0; i<n_items; i++) {
        x = i*icon_width;
        icon = get_equipment_slot_icon(i);
        sx = (icon % n_items) * icon_width;
        sy = (icon / n_items) * icon_height;
        draw_bound_texture_sprite(x,y, icon_width, icon_height, z, sx, sy, icon_width, icon_height, spritesheet_width, spritesheet_height);
    }
}

void draw_equipped() {
    static const float z = -0.5f;
    static const float line_width = 2.0f;

    float x0,y0,x1,y1;

    x0 = icon_width * slot;
    x1 = x0 + icon_width;
    y0 = _yresf - icon_height;
    y1 = _yresf;
    
    glLineWidth(line_width);
    glColor4ub(0,0,255,0);  // blue
    glBegin(GL_LINE_STRIP);

    glVertex3f(x0, y0, z);  // Top left
    glVertex3f(x1, y0, z);  // Top right
    glVertex3f(x1, y1, z);  // Bottom right
    glVertex3f(x0, y1, z);  // Bottom left
    glVertex3f(x0, y0, z);

    glEnd();
    glLineWidth(1.0);
    glColor3ub(255,255,255);
}

void draw() {

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //draw_panel();
    draw_icons();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    draw_equipped();
}

//cython
void draw_equipment(int slot) {
    set_slot(slot);
    draw();
}

}
