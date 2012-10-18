#pragma once

typedef enum
{
    MECH_NONE = 0,
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

//instance
struct MECH
{
    public:
    int id; //-1 if not in use

    int x;
    int y;
    int z;

    int mech_type;       //mech type
    int subtype;        //sprite
    MechRenderType render_type;
    
    float size;
    int offset;
    float rotation;

    float offset_x;
    float offset_y;
};

struct MECH_ATTRIBUTE
{
    int mech_type;
    MechClass mech_type_class;
    MechRenderType render_type;
    int sprite_index;
    bool item_drop; // indicates whether has an item drop set
};
