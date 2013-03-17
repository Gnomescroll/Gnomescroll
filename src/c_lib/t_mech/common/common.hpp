#pragma once

const int MAX_MECHS = 0xFF;

typedef enum
{
    NULL_MECH_TYPE = MAX_MECHS,
} MechType;

bool isValid(MechType type)
{
    return (type >= 0 && type < MAX_MECHS);
}

typedef enum
{
    NULL_MECH_CLASS = 0,
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_MYCELIUM,
    MECH_WIRE,
    MECH_SWITCH,
    MECH_SIGN
} MechClassType;

typedef enum
{
    MECH_RENDER_TYPE_NONE = 0,
    MECH_RENDER_TYPE_0, //X shaped/crystal/
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
    MECH_RENDER_TYPE_3, //oriented on side of block
} MechRenderType;

typedef enum
{
    MECH_BEHAVIOR_TYPE_DEFAULT = 0,
    MECH_BEHAVIOR_TYPE_PLANT,
    MECH_BEHAVIOR_TYPE_LIGHT_PLANT,
    MECH_BEHAVIOR_TYPE_DARK_PLANT,
} MechBehaviorType;

typedef enum
{
    NULL_MECH_SPRITE = 0xFF
}   MechSpriteIndex;

struct Mech
{
    int id; //-1 if not in use

    int x;
    int y;
    int z;

    MechType type;       //mech type
    int subtype;        //sprite
    MechRenderType render_type;

    float size;
    int offset;
    float rotation;

    float offset_x;
    float offset_y;

    int growth_ttl;
};

class MechAttribute
{
    public:
        bool loaded;

        MechType type;
        MechClassType class_type;
        MechRenderType render_type;

        MechBehaviorType behavior_type;
        bool plant;
        int growth_ttl;         //starting growth ttl
        MechType growth_stage;  //next growth stage

        MechSpriteIndex sprite;
        bool item_drop; // indicates whether has an item drop set
        char name[DAT_NAME_MAX_LENGTH+1];

    MechAttribute() :
        loaded(false),
        type(NULL_MECH_TYPE),
        class_type(NULL_MECH_CLASS),
        render_type(MECH_RENDER_TYPE_NONE),
        behavior_type(MECH_BEHAVIOR_TYPE_DEFAULT),
        plant(false), growth_ttl(-1), growth_stage(NULL_MECH_TYPE),
        sprite(NULL_MECH_SPRITE),
        item_drop(false)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};
