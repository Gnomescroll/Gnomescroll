#pragma once

#include <t_map/glsl/palette.hpp>

namespace t_map
{

class CubeProperties* p = NULL;

int _current_cube_id = EMPTY_CUBE + 1;
int _palette_number = 0;

int _side_texture[6];

int DEFAULT_MAX_DAMAGE = 32;

void _finish_cube()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    p->in_use = true;
}

// private method; don't call this one directly
static void cube_def(CubeID id, CubeType type, const char* name, CubeMaterial material)
{
    GS_ASSERT_ABORT(id == NULL_CUBE || id == ERROR_CUBE || id == EMPTY_CUBE);
    
    if (p != NULL)
        _finish_cube();
    
    if (id == NULL_CUBE) id = (CubeID)_current_cube_id++;

    GS_ASSERT_ABORT(is_valid_cube_name(name));
    if (!is_valid_cube_name(name)) return;
    
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return;

    p = &cube_properties[id];

    GS_ASSERT_ABORT(!p->in_use);
    if (p->in_use) return;
    
    p->id = id;    
    strncpy(p->name, name, CUBE_NAME_MAX_LENGTH);
    p->name[CUBE_NAME_MAX_LENGTH] = '\0';
    p->max_damage = DEFAULT_MAX_DAMAGE;
    p->type = type;
    
    switch (type)
    {
        case ErrorCube:
            break;

        case SolidCube:
            break;

        case EmptyCube:
            p->solid = false;
            p->occludes = false;
            p->magic = false;
            break;

        case ItemContainerCube:
            p->item_container = true;
            break;

        default:
            GS_ASSERT_ABORT(false);
            break;
    }

    _palette_number = 0;
    memset(_side_texture, 0, sizeof(_side_texture));
}

// this is the public method
void cube_def(CubeType type, const char* name, CubeMaterial material)
{
    cube_def(NULL_CUBE, type, name, material);
}

void cube_def(CubeType type, const char* name)
{
    cube_def(type, name, CUBE_MATERIAL_NONE);
}

void cube_def_empty(const char* name)
{
    static int i=0;
    GS_ASSERT_ABORT(!(i++));
    cube_def(EMPTY_CUBE, EmptyCube, name, CUBE_MATERIAL_NONE);
}

void cube_def_error(const char* name)
{
    static int i=0;
    GS_ASSERT_ABORT(!(i++));
    cube_def(ERROR_CUBE, ErrorCube, name, CUBE_MATERIAL_NONE);
}

void end_cube_def()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    _finish_cube();
    #if DC_CLIENT
    save_cube_texture();
    #endif
}

void set_max_damage(int max_damage)
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    GS_ASSERT_ABORT(max_damage > 0 && max_damage <= MAX_CUBE_DAMAGE);
    if (max_damage <= 0 || max_damage >= MAX_CUBE_DAMAGE) return;
    p->max_damage = max_damage;
}

#if DC_SERVER
void hud_def(int yhud, int xhud, int sheet_id, int ypos, int xpos) {}
void push_oriented_texture() {}
void push_texture() {}
void iso_texture(int tex_id) {}
void iso_texture(int sheet_id, int ypos, int xpos) {}
void side_texture(int side, int tex_id) {}
void side_texture(int side, int sheet_id, int ypos, int xpos) {}

int sprite_alias(int sheet_id, int ypos, int xpos) { return 0; }
int texture_alias(const char* spritesheet) { return 0; }
#endif

#if DC_CLIENT
void iso_texture(int tex_id)
{
    for (int i=0; i<6; _side_texture[i++] = tex_id);
}

void iso_texture(int sheet_id, int ypos, int xpos)
{
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    if (xpos < 1 || ypos < 1) return;

    int tex_id = blit_cube_texture(sheet_id, xpos, ypos);
    for (int i=0; i<6; _side_texture[i++] = tex_id);
}

void side_texture(int side, int tex_id)
{
    GS_ASSERT_ABORT(side >= 0 && side < 6);
    if (side < 0 || side >= 6) return;
    
    _side_texture[side] = tex_id;
}

void side_texture(int side, int sheet_id, int ypos, int xpos)
{
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    if (xpos < 1 || ypos < 1) return;
    
    int tex_id = blit_cube_texture(sheet_id, xpos, ypos);
    _side_texture[side] = tex_id;
}

void push_texture()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    
    start_cube_palette(p->id);
    for (int i=0; i<6; i++) set_cube_side_texture(p->id, i, _side_texture[i]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, i, _side_texture[i]);
    push_cube_palette();
    _palette_number++;
}

void push_oriented_texture()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    
    /*
        NSWE //normal
        EWNS
        SNEW
        WESN
    */

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    //printf("current_block= %i \n", p->id);
    for (int i=0; i<6; i++) set_cube_side_texture(p->id, i, _side_texture[i]);

    //NORTH
    start_cube_palette(p->id);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, N, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, S, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, W, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, E, _side_texture[5]);
    push_cube_palette();

    //WEST
    start_cube_palette(p->id);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, E, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, W, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, N, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, S, _side_texture[5]);
    push_cube_palette();

    //SOUTH
    start_cube_palette(p->id);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, S, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, N, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, E, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, W, _side_texture[5]);
    push_cube_palette();

    //EAST
    start_cube_palette(p->id);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, W, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, E, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, S, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->id, N, _side_texture[5]);
    push_cube_palette();

    _palette_number += 4;
}

void hud_def(int yhud, int xhud, int sheet_id, int ypos, int xpos)
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;

    if (xpos < 1 || ypos < 1) printf("hud_def error: ypos and xpos must be greater than 1\n");
    if (yhud < 1 || xhud < 1 || xhud > 8 || yhud > 8) printf("hud_def error: yhud and xhud must be between 1 and 8\n");
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    GS_ASSERT_ABORT(xhud >= 1 && yhud >= 1 && xhud <= 8 && yhud <= 8);

    int tex_id = blit_cube_texture(sheet_id, xpos, ypos);

    // check that the texture is use somewhere in the cube
    bool found = false;
    for (int i=0; i<6; i++)
    {
        if (_side_texture[i] == 0) continue;
        if (_side_texture[i] == tex_id)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        printf("%s failed for sheet,y,x %d,%d,%d. No matching texture found for def (check iso_texture)\n", __FUNCTION__, sheet_id, ypos, xpos);
        GS_ASSERT_ABORT(found);
    }

    set_cube_hud(xhud, yhud, p->id, tex_id);
}

int texture_alias(const char* spritesheet) 
{ 
    return load_cube_texture_sheet(spritesheet); 
}

int sprite_alias(int sheet_id, int ypos, int xpos)
{
    if (xpos < 1 || ypos < 1)
        printf("sprite_alias error: xpos and ypos must be greater than 1\n");
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    return blit_cube_texture(sheet_id, xpos, ypos); 
}
#endif

void verify_config()
{
    for (int i=0; i<MAX_CUBES; i++)
    {
        class CubeProperties* p = &cube_properties[i];
        if (!cube_properties[i].in_use) continue;

        GS_ASSERT_ABORT(p->id == (CubeID)i);
        GS_ASSERT_ABORT(p->id != NULL_CUBE);

        GS_ASSERT_ABORT(p->id != EMPTY_CUBE || p->type == EmptyCube);
        GS_ASSERT_ABORT(p->id != ERROR_CUBE || p->type == ErrorCube);
        GS_ASSERT_ABORT(p->id == EMPTY_CUBE || p->type != EmptyCube);
        GS_ASSERT_ABORT(p->id == ERROR_CUBE || p->type != ErrorCube);

        GS_ASSERT_ABORT(p->max_damage <= MAX_CUBE_DAMAGE);
        GS_ASSERT_ABORT(is_valid_cube_name(p->name));
    }
}

}   // t_map
