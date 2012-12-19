#pragma once

const int MAX_MECHS = 0xff;

typedef enum
{
    NULL_MECH_TYPE = MAX_MECHS,
} MechType;

typedef enum
{
    NULL_MECH_CLASS = 0,
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_MYCELIUM,
    MECH_WIRE,
    MECH_SWITCH
} MechClass;

typedef enum
{
    MECH_RENDER_TYPE_NONE = 0,
    MECH_RENDER_TYPE_0, //X shaped/crystal/
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
} MechRenderType;

typedef enum
{
    MECH_BEHAVIOR_TYPE_DEFAULT = 0
    MECH_BEHAVIOR_TYPE_PLANT,
} MechBehaviorType;

//instance
struct MECH
{
    public:
    int id; //-1 if not in use

    int x;
    int y;
    int z;

    MechType mech_type;       //mech type
    int subtype;        //sprite
    MechRenderType render_type;
    
    float size;
    int offset;
    float rotation;

    float offset_x;
    float offset_y;

    int growth_ttl;
};

typedef enum
{
    NULL_MECH_SPRITE = 0xff,
}   MechSpriteIndex;

class MechAttribute
{
    public:
        MechType mech_type;
        MechClass mech_type_class;
        MechRenderType render_type;
        MechBehaviorType behavior_type;

        int growth_ttl;         //starting growth ttl
        MechType growth_stage;  //next growth stage

        MechSpriteIndex sprite_index;
        bool item_drop; // indicates whether has an item drop set
        char name[DAT_NAME_MAX_LENGTH+1];
        bool loaded;

    MechAttribute() :
        mech_type(NULL_MECH_TYPE), mech_type_class(NULL_MECH_CLASS),
        render_type(MECH_RENDER_TYPE_NONE), sprite_index(NULL_MECH_SPRITE),
        growth_ttl(-1), growth_stage(-1),
        item_drop(false), loaded(false)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};
