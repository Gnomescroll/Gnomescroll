#pragma once

const int MAX_MECHS = 0xFF;

const int NULL_MECH_ID = -1;

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
    MECH_SIGN,
    MECH_WALL_OBJECT //can be placed on wall, does not need block under it
} MechClassType;

typedef enum
{
    MECH_RENDER_TYPE_NONE = 0,
    MECH_RENDER_TYPE_0, //X shaped/crystal/
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
    MECH_RENDER_TYPE_3, //oriented on side of block
    MECH_RENDER_TYPE_4, //torch crystal
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
    MCF_OK = 0,
    MCF_BAD_TYPE,
    MCF_NOT_USED,
    MCF_BAD_Z,
    MCF_BAD_SIDE,
    MCF_SOLID_BLOCK,
    MCF_ON_AIR,
    MCF_OCCUPIED,
    MCF_UNHANDLED,
    MCF_NO_WALL,
    MCF_OTHER,      // use for any error too specific to warrant a new label
}   MechCreateFailureCode;

typedef enum
{
    NULL_MECH_SPRITE = 0xFF
}   MechSpriteIndex;

int NULL_MECH_SIDE = 0;
const int MECH_TEXT_SIZE_MAX = 8*8;

struct Mech
{
    int id; //-1 if not in use

    Vec3i position;
    Vec3 center;

    MechType type;       //mech type
    int subtype;        //sprite
    MechRenderType render_type;

    int offset;
    float rotation;

    int side;   //for placement on side of block

    float offset_x;
    float offset_y;

    int growth_ttl;

    void* text; //stuff
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

        float size;     // configure this in dat loader
        float radius;   // compute this from the sprites
        Vec3 dimensions;

    MechAttribute() :
        loaded(false),
        type(NULL_MECH_TYPE),
        class_type(NULL_MECH_CLASS),
        render_type(MECH_RENDER_TYPE_NONE),
        behavior_type(MECH_BEHAVIOR_TYPE_DEFAULT),
        plant(false), growth_ttl(-1), growth_stage(NULL_MECH_TYPE),
        sprite(NULL_MECH_SPRITE),
        item_drop(false), size(1.0f), radius(0.5f), dimensions(vec3_init(1))
    {
        memset(this->name, 0, sizeof(this->name));
    }
};
