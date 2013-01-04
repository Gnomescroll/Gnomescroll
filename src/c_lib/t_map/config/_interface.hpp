#pragma once

#include <SDL/constants.hpp>
#include <t_map/glsl/palette.hpp>

namespace t_map
{

#define CUBE_NAME_FILE_ACTIVE   "cube_names.active"
#define CUBE_NAME_FILE_INACTIVE "cube_names.inactive"

class CubeProperties* p = NULL;

int _current_cube_type = EMPTY_CUBE + 1;
int _palette_number = 0;

int _side_texture[6];

int DEFAULT_MAX_DAMAGE = 32;

void _finish_cube()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    p->loaded = true;
}

// private method; don't call this one directly
static void cube_def(CubeType type, CubeGroup group, const char* name, CubeMaterial material)
{
    GS_ASSERT_ABORT(type == NULL_CUBE || type == ERROR_CUBE || type == EMPTY_CUBE);
    
    if (p != NULL)
        _finish_cube();
    
    if (type == NULL_CUBE) type = (CubeType)_current_cube_type++;

    GS_ASSERT_ABORT(is_valid_cube_name(name));
    if (!is_valid_cube_name(name)) return;
    
    ASSERT_VALID_CUBE_TYPE(type);
    IF_INVALID_CUBE_TYPE(type) return;

    p = &cube_properties[type];

    GS_ASSERT_ABORT(!p->loaded);
    if (p->loaded) return;
    
    p->type = type;    
    strncpy(p->name, name, DAT_NAME_MAX_LENGTH);
    p->name[DAT_NAME_MAX_LENGTH] = '\0';
    p->max_damage = DEFAULT_MAX_DAMAGE;
    p->group = group;
    p->material = material;
    
    switch (group)
    {
        case ErrorCube:
        case SolidCube:
        case ItemContainerCube:
            p->solid = true;
            p->occludes = true;
            p->active = true;
            break;

        case EmptyCube:
            p->solid = false;
            p->occludes = false;
            p->active = false;
            break;

        default:
            GS_ASSERT_ABORT(false);
            break;
    }

    _palette_number = 0;
    memset(_side_texture, 0, sizeof(_side_texture));
}

// this is the public method
void cube_def(CubeGroup group, const char* name, CubeMaterial material)
{
    cube_def(NULL_CUBE, group, name, material);
}

void cube_def(CubeGroup group, const char* name)
{
    cube_def(group, name, CUBE_MATERIAL_NONE);
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

void cube_def_container(const char* name)
{
    // TODO -- cube material for chests?
    cube_def(ItemContainerCube, name, CUBE_MATERIAL_NONE);
}

static void copy_cube_properties(class CubeProperties* a, struct FastCubeProperties* b)
{
    b->active = a->active;
    b->solid = a->solid;
    b->occludes = a->occludes;
    b->transparent = a->transparent;
    b->item_drop = a->item_drop;
    b->item_container = (a->group == ItemContainerCube);
    b->explosive = a->explosive;
}

void end_cube_def()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    _finish_cube();

    // copy CubeProperties data to fast cube properties
    for (int i=0; i<MAX_CUBES; i++)
        if (cube_properties[i].loaded)
            copy_cube_properties(&cube_properties[i], &fast_cube_properties[i]);
    
    #if DC_CLIENT
    TextureSheetLoader::save_cube_texture();
    #endif
}

void set_max_damage(int max_damage)
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    GS_ASSERT_ABORT(max_damage > 0 && max_damage <= MAX_CUBE_DAMAGE);
    if (max_damage <= 0 || max_damage > MAX_CUBE_DAMAGE) return;
    p->max_damage = max_damage;
}

#if DC_SERVER
void hud_def(int yhud, int xhud, SpriteSheet sheet_id, int ypos, int xpos) {}
void hud_def(SpriteSheet sheet_id, int ypos, int xpos) {}
void push_oriented_texture() {}
void push_texture() {}
void iso_texture(int tex_id) {}
void iso_texture(SpriteSheet sheet_id, int ypos, int xpos) {}
void side_texture(int side, int tex_id) {}
void side_texture(int side, SpriteSheet sheet_id, int ypos, int xpos) {}

int sprite_alias(SpriteSheet sheet_id, int ypos, int xpos) { return 0; }
#endif

#if DC_CLIENT
void iso_texture(int tex_id)
{
    for (int i=0; i<6; _side_texture[i++] = tex_id);
}

void iso_texture(SpriteSheet sheet_id, int ypos, int xpos)
{
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    if (xpos < 1 || ypos < 1) return;

    int tex_id = TextureSheetLoader::blit_cube_texture(sheet_id, xpos, ypos);
    GS_ASSERT_ABORT(tex_id != NULL_SPRITE);
    if (tex_id == NULL_SPRITE) return;
    for (int i=0; i<6; _side_texture[i++] = tex_id);
}

void side_texture(int side, int tex_id)
{
    GS_ASSERT_ABORT(side >= 0 && side < 6);
    if (side < 0 || side >= 6) return;
    
    _side_texture[side] = tex_id;
}

void side_texture(int side, SpriteSheet sheet_id, int ypos, int xpos)
{
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    if (xpos < 1 || ypos < 1) return;
    
    int tex_id = TextureSheetLoader::blit_cube_texture(sheet_id, xpos, ypos);
    GS_ASSERT_ABORT(tex_id != NULL_SPRITE);
    if (tex_id == NULL_SPRITE) return;
    _side_texture[side] = tex_id;
}

void push_texture()
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;
    
    start_cube_palette(p->type);
    for (int i=0; i<6; i++) set_cube_side_texture(p->type, i, _side_texture[i]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, i, _side_texture[i]);
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

    //printf("current_block= %i \n", p->type);
    for (int i=0; i<6; i++) set_cube_side_texture(p->type, i, _side_texture[i]);

    //NORTH
    start_cube_palette(p->type);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, N, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, S, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, W, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, E, _side_texture[5]);
    push_cube_palette();

    //WEST
    start_cube_palette(p->type);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, E, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, W, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, N, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, S, _side_texture[5]);
    push_cube_palette();

    //SOUTH
    start_cube_palette(p->type);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, S, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, N, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, E, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, W, _side_texture[5]);
    push_cube_palette();

    //EAST
    start_cube_palette(p->type);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, T, _side_texture[0]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, B, _side_texture[1]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, W, _side_texture[2]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, E, _side_texture[3]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, S, _side_texture[4]);
    for (int i=0; i<6; i++) set_cube_palette_texture(p->type, N, _side_texture[5]);
    push_cube_palette();

    _palette_number += 4;
}

static void hud_def(int yhud, int xhud, int tex_id)
{
    GS_ASSERT_ABORT(p != NULL);
    if (p == NULL) return;

    if (yhud < 1 || xhud < 1 || xhud > 8 || yhud > 8) printf("hud_def error: yhud and xhud must be between 1 and 8\n");
    GS_ASSERT_ABORT(xhud >= 1 && yhud >= 1 && xhud <= 8 && yhud <= 8);

    // check that the texture is use somewhere in the cube
    bool found = false;
    for (int i=0; i<6; i++)
        if (_side_texture[i] == tex_id)
        {
            found = true;
            break;
        }
    if (!found)
    {
        printf("%s failed for texture id %d. No matching texture found (texture must be used in iso_texture or side_texture)\n", __FUNCTION__, tex_id);
        GS_ASSERT_ABORT(found);
    }

    HudCubeSelector::set_cube_hud(xhud, yhud, p->type, tex_id);
}

void hud_def(int yhud, int xhud, SpriteSheet sheet_id, int ypos, int xpos)
{
    int tex_id = TextureSheetLoader::blit_cube_texture(sheet_id, xpos, ypos);
    GS_ASSERT_ABORT(tex_id != NULL_SPRITE);
    if (tex_id == NULL_SPRITE) return;
    hud_def(yhud, xhud, tex_id);
}

void hud_def(SpriteSheet sheet_id, int ypos, int xpos)
{
    int tex_id = TextureSheetLoader::blit_cube_texture(sheet_id, xpos, ypos);
    
    // check that the texture is use somewhere in the cube
    bool found = false;
    for (int i=0; i<6; i++)
        if (_side_texture[i] == tex_id)
        {
            found = true;
            break;
        }
    if (!found)
    {
        printf("%s failed for texture id %d. No matching texture found (texture must be used in iso_texture or side_texture)\n", __FUNCTION__, tex_id);
        GS_ASSERT_ABORT(found);
    }

    HudCubeSelector::set_cube_hud(p->type, tex_id);
}

int sprite_alias(SpriteSheet sheet_id, int ypos, int xpos)
{
    if (xpos < 1 || ypos < 1)
        printf("sprite_alias error: xpos and ypos must be greater than 1\n");
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    int tex_id = TextureSheetLoader::blit_cube_texture(sheet_id, xpos, ypos);
    GS_ASSERT_ABORT(tex_id != NULL_SPRITE);
    return tex_id;
}
#endif

// Use this to remove or rename a block
void change_block(const char* original, const char* replacement)
{
    GS_ASSERT_ABORT(is_valid_cube_name(original));
    GS_ASSERT_ABORT(is_valid_cube_name(replacement));        
    bool mapped = cube_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

void verify_config()
{
    // check validity of individual cubes
    for (int i=0; i<MAX_CUBES; i++)
    {
        class CubeProperties* p = &cube_properties[i];
        if (!cube_properties[i].loaded) continue;

        GS_ASSERT_ABORT(p->type == (CubeType)i);
        GS_ASSERT_ABORT(p->type != NULL_CUBE);

        GS_ASSERT_ABORT(p->type != EMPTY_CUBE || p->group == EmptyCube);
        GS_ASSERT_ABORT(p->type != ERROR_CUBE || p->group == ErrorCube);
        GS_ASSERT_ABORT(p->type == EMPTY_CUBE || p->group != EmptyCube);
        GS_ASSERT_ABORT(p->type == ERROR_CUBE || p->group != ErrorCube);

        GS_ASSERT_ABORT(p->max_damage <= MAX_CUBE_DAMAGE);
        GS_ASSERT_ABORT(is_valid_cube_name(p->name));
    }

    // check uniqueness among cubes
    for (int i=0; i<MAX_CUBES-1; i++)
    for (int j=i+1; j<MAX_CUBES; j++)
    {
        class CubeProperties* a = &cube_properties[i];
        class CubeProperties* b = &cube_properties[j];
        if (!a->loaded || !b->loaded) continue;
        if (a->container_type == NULL_CONTAINER_TYPE || b->container_type == NULL_CONTAINER_TYPE) continue;
        GS_ASSERT_ABORT(a->container_type != b->container_type);
    }

    GS_ASSERT_ABORT(cube_name_map->condensed);

    // check inactive names against active
    for (int i=0; i<MAX_CUBES; i++)
        if (cube_properties[i].loaded)
        {
            GS_ASSERT_ABORT(cube_name_map->get_mapped_name(cube_properties[i].name) == NULL);
        }

    // check inactive name destinations against active
    for (size_t i=0; i<cube_name_map->size; i++)
    {
        GS_ASSERT_ABORT(get_cube_type(cube_name_map->get_replacement(i)) != ERROR_CUBE);
    }

    #if DC_SERVER || !PRODUCTION
    // either both files must be missing or both must exist
    bool active_dat = file_exists(DATA_PATH CUBE_NAME_FILE_ACTIVE);
    bool inactive_dat = file_exists(DATA_PATH CUBE_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT((active_dat && inactive_dat) || (!active_dat && !inactive_dat));

    if (active_dat && inactive_dat)
    {   // check that all names declared a valid with respect to past name definitions
        // but only if the files are present
        GS_ASSERT_ABORT(name_changes_valid(DATA_PATH CUBE_NAME_FILE_ACTIVE, DATA_PATH CUBE_NAME_FILE_INACTIVE,
            DAT_NAME_MAX_LENGTH, cube_properties, MAX_CUBES, cube_name_map));
    }
    #endif
}

void save_cube_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(cube_properties, MAX_CUBES, DAT_NAME_MAX_LENGTH, DATA_PATH CUBE_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = cube_name_map->save(DATA_PATH CUBE_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}

}   // t_map
