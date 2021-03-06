/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace Animations
{

// TODO: move this somewhere. its for caching/restoring cull mode state
extern GLint cull_face_mode;

class AlignmentNode
{
    public:
        float dz;
        float dxy;
        float depth;

    AlignmentNode() :
        dz(0), dxy(0), depth(0)
    {}

    void print()
    {
        printf("dz: %f\n", this->dz);
        printf("dxy: %f\n", this->dxy);
        printf("depth: %f\n", this->depth);
    }
};

class EquippedItemAlignment
{
    public:
        class AlignmentNode focal;
        class AlignmentNode origin;

    void print()
    {
        printf("Sprite alignment:\n");
        printf("Focal:\n");
        this->focal.print();
        printf("Origin:\n");
        this->origin.print();
    }
};

class EquippedItemConfig
{
    public:
        char filename[GS_FN_MAX+1];
        float scale;
        class EquippedItemAlignment alignment;

    EquippedItemConfig() :
        scale(1)
    {}

    void parse();

    void set_filename(const char* filename)
    {
        size_t wrote = snprintf(this->filename, GS_FN_MAX+1, MEDIA_PATH "config/%s", filename);
        this->filename[GS_FN_MAX] = '\0';
        GS_ASSERT(wrote < GS_FN_MAX);
        GS_ASSERT(file_exists(this->filename));
    }

    void print()
    {
        printf("Sprite config:\n");
        printf("config file: %s\n", this->filename);
        printf("scale: %f\n", this->scale);
        this->alignment.print();
    }

};

extern class EquippedItemConfig voxelized_sprite_config;

void draw_equipped_voxel_item_other_agent(AgentID agent_id, ItemType item_type);
void draw_equipped_sprite_item_other_agent(AgentID agent_id, ItemType item_type);
void draw_equipped_voxelized_sprite_item_other_agent(AgentID agent_id, ItemType item_type);

void draw_equipped_item(ItemType item_type);
void begin_equipped_item_animation(ItemType item_type, bool continuous);
void tick_equipped_item_animation();
void stop_equipped_item_animation();

void draw_placement_outline(ItemType item_type);

// controls for adjusting the sprite
void move_focal_depth(float delta);            // distance from agent plane
void move_focal_vertical(float delta);        // vertical distance
void move_focal_horizontal(float delta);    // horizontal distance

void move_origin_vertical(float delta);         // vertical distance along agent plane
void move_origin_horizontal(float delta);    // horizontal distance along agent plane
void move_origin_depth(float delta);        // distance from agent plane

void print_sprite_alignment_config();

void cycle_current_config();

void dilate_equipped_sprite(float delta);

void init_weapon_sprite();

// gl wrappers
bool draw_voxel_gl_begin(GLint cull_mode);
void draw_voxel_gl_end();
bool draw_sprite_gl_begin();
void draw_sprite_gl_end();

}   // Animations
